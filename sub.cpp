#include <zmq.hpp>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string_view>
#include "testSchema.pb.h"
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>

int main() {
    zmq::context_t ctx;
    zmq::socket_t subscriber(ctx,zmq::socket_type::sub);
    subscriber.connect("ipc:///tmp/zmq_socket");
    subscriber.set(zmq::sockopt::subscribe,"");
 
    nng_socket sock;
    int rv;
    if ((rv = nng_pub0_open(&sock)) != 0) {
        std::cout<<"Failed to open pub socket\n";
        return 1;
    }

    if ((rv = nng_listen(sock, "tcp://0.0.0.0:5555", NULL, 0)) != 0) {
        std::cout<<"Failed to listen.\n";
        nng_close(sock);
        return 1;
    }
    std::cout<<"Publisher listening on tcp://0.0.0.0:5555\n";

    std::cout << "Bridging ZMQ->NNG (ZMQ sub to NNG pub) running...\n";

    while (true) {
        zmq::message_t message;
        subscriber.recv(message);

        const void* data = message.data();
        size_t len = message.size();
        
        nng_msg* nng_msg = nullptr;
        rv = nng_msg_alloc(&nng_msg,len);
        if (rv != 0) {
            std::cout << "NNG msg alloc failed." << "\n";
            continue;
        }

        memcpy(nng_msg_body(nng_msg), data, len);

        rv = nng_sendmsg(sock, nng_msg, 0);
        if (rv != 0) {
            std::cout << "NNG send failed. "<<"\n";
            nng_msg_free(nng_msg); // safe to free on failure
        } else {
            std::cout << "Forwarded message of size " << len << "\n";
        }

        std::string serialized(static_cast<char*>(message.data()),message.size());
        Person received;
        if(received.ParseFromString(serialized)){
          std::cout<<"Received Person:\n";
          std::cout<<"Name: "<<received.name()<<"\n";
          std::cout<<"ID: "<<received.id()<<"\n";
        }
    }
    nng_close(sock);
    return 0;
}
