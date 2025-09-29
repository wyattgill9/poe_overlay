#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "example.h"

int main() {
    Test test;
    test.test();

    // TODO: make https & use openssl
    httplib::Client cli("http://mobalytics.gg/poe-2/builds/lightning-arrow-farmer-fubgun");
    auto res = cli.Get("/");

    std::cout<< res->status << "\n";
    std::cout<< res->body << "\n";

    return 0;
}
