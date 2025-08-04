#include <iostream>
#include <nng.h>
#include <nng/protocol/pubsub0/sub.h>
#include <thread>
#include <chrono>

int main() {


    nng_socket sock;
    int rv;

    if ((rv = nng_sub0_open(&sock)) != 0) {
        std::cerr << "Failed to open sub socket\n";
    }

    if ((rv = nng_setopt(sock, NNG_OPT_SUB_SUBSCRIBE, "", 0)) != 0) {
        std::cerr << "Failed to set subscribe option\n";
        nng_close(sock);
    }

    if ((rv = nng_dial(sock, "tcp://127.0.0.1:5554", NULL, 0)) != 0) {
        std::cerr << "Failed to connect to publisher\n";
        nng_close(sock);
    }

    std::cout << "Connected to publisher. Waiting for messages...\n";



    while (true) {

        char* buf = nullptr;
        size_t sz;

        if ((rv = nng_recv(sock, &buf, &sz, NNG_FLAG_ALLOC)) != 0) {
            std::cerr << "NNG receive failed: " << nng_strerror(rv) << "\n";
            continue;
        }

        std::string data(buf, sz); // You can parse this into your Person message
        nng_free(buf, sz);

        std::cout << "Data received from NNG: " << data << "\n";
    }
    return 0;
}
