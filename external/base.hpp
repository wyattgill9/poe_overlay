#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <iostream>
#include <optional>
#include <concepts>
#include <thread>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace detail {  

template<typename T>
concept Display = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream&>;
};

template<typename T>
concept IsPointer = std::is_pointer_v<T>;

//
// Created by Anders Cedronius on 2023-06-27.
//
template<typename T, uint64_t RING_BUFFER_SIZE, uint64_t L1_CACHE_LNE>
class FastQueue {
    static_assert(sizeof(T) == 8, "Only 64 bit objects are supported");
    static_assert(sizeof(void*) == 8, "The architecture is not 64-bits");
    static_assert((RING_BUFFER_SIZE & (RING_BUFFER_SIZE + 1)) == 0, "RING_BUFFER_SIZE must be a number of contiguous bits set from LSB. Example: 0b00001111 not 0b01001111");

public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-volatile"
    template<typename... Args>
    void push(Args&&... args) noexcept {
        while (mRingBuffer[mWritePosition & RING_BUFFER_SIZE].mObj) if (mExitThreadSemaphore) [[unlikely]] return;
        new(&mRingBuffer[mWritePosition++ & RING_BUFFER_SIZE].mObj) T{ std::forward<Args>(args)... };
    }

    inline void pop(T& aOut) noexcept {
        std::atomic_thread_fence(std::memory_order_consume);
        while (!(aOut = mRingBuffer[mReadPosition & RING_BUFFER_SIZE].mObj)) {
            if ((mExitThread == mReadPosition) && mExitThreadSemaphore) [[unlikely]] {
                aOut = nullptr;
                return;
            }
        }
        mRingBuffer[mReadPosition++ & RING_BUFFER_SIZE].mObj = nullptr;
    }

    //Stop queue (Maybe called from any thread)
    void stopQueue() {
        mExitThread = mWritePosition;
        mExitThreadSemaphore = true;
    }

#pragma GCC diagnostic pop

private:
     struct AlignedDataObjects {
        alignas(L1_CACHE_LNE * 2) T mObj = nullptr;
    };
    alignas(L1_CACHE_LNE) volatile uint64_t mReadPosition = 0;
    alignas(L1_CACHE_LNE) volatile std::atomic<uint64_t> mWritePosition = 0;
    alignas(L1_CACHE_LNE) volatile uint64_t mExitThread = 0;
    alignas(L1_CACHE_LNE) volatile bool mExitThreadSemaphore = false;
    std::array<AlignedDataObjects, RING_BUFFER_SIZE+1> mRingBuffer;
    alignas(L1_CACHE_LNE) volatile uint8_t mBorderDown[L1_CACHE_LNE]{};
};

};

namespace base {

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;

using f64 = double;
using f32 = float;

template<typename T>
struct Option {
private:
    std::optional<T> inner;

public:
    Option() = default;
    Option(T&& value)      : inner(std::move(value)) {}
    Option(const T& value) : inner(value)            {}

    bool is_some() {
        return inner.has_value();
    }

    bool is_none() {
        return !inner.has_value();
    }

    T& unwrap() {
        if (!inner) throw std::runtime_error("called unwrap() on None");
        return *inner;
    }
   
    T unwrap_or(T&& fallback) const {
        return inner.value_or(fallback);
    }

    operator bool() const {
        return inner.has_value();
    }

    T& operator*() {
        return inner.value();
    }
};

#define Some(x) Option {x}
template<typename T>
Option<T> None() { return Option<T>{}; }

namespace io {   
    void println(detail::Display auto d) {
        std::cout << d << "\n";
    }
};

namespace containers {

// ----- SPSC QUEUE -----
//
// ---- MY WRAPPER ----

template<detail::IsPointer T, u64 SIZE = 1023, u64 CACHE_LINE = 64>
struct BoundedPointerSPSC {
private:
    detail::FastQueue<T, SIZE, CACHE_LINE> queue;
    std::thread consumer_thread;
    bool stopped = false;

public:
    BoundedPointerSPSC() = default;
    ~BoundedPointerSPSC() { stop(); }

    BoundedPointerSPSC(const BoundedPointerSPSC&) = delete;
    BoundedPointerSPSC& operator=(const BoundedPointerSPSC&) = delete;
    BoundedPointerSPSC(BoundedPointerSPSC&&) = delete;
    BoundedPointerSPSC& operator=(BoundedPointerSPSC&&) = delete;

    template<typename... Args>
    void push(Args&&... args) {
        queue.push(std::forward<Args>(args)...);
    }
    
    Option<T> pop() {
        T item;
        queue.pop(item);

        if (item == nullptr && stopped) {
            return None<T>();
        }

        return Some(item);
    }

    Option<T> try_pop() {
        return pop();
    }

    void stop() {
        if (!stopped) {
            stopped = true;
            queue.stopQueue();
        }
    }

    bool is_stopped() const {
        return stopped;
    }

    void start_consumer(std::function<void(T)> callback) {
        consumer_thread = std::thread([this, callback = std::move(callback)]() {
            while (true) {
                auto item = pop();
                if (item.is_none()) break;
                callback(item.unwrap());
            }
        });
    }

    void join() {
        if (consumer_thread.joinable()) {
            consumer_thread.join();
        }
    }
};

};

template<typename To>
constexpr auto implicit_cast = [](To from) {
    return from;
};

};
