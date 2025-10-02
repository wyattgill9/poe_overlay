#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "passive_tree.h"

int main() {
    PassiveTree tree;

    tree.add_node(0, "Hi");
    
    httplib::Client cli("https://mobalytics.gg");
    auto res = cli.Get("/poe-2/builds/lightning-arrow-farmer-fubgun");

    if(res) {
    //     std::cout<< res->status << "\n";
        std::cout<< res->body   << "\n";
    }

    return 0;
}
