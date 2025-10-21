#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <concepts>

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

#define Some(x) std::optional<decltype(x)>(x);

namespace io {   

template<typename T>
concept Display = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream&>;
};

void println(Display auto d) {
    std::cout << d << "\n";
}

};

template<typename To>
constexpr auto implicit_cast = [](To from) {
    return from;
};

};
