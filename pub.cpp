#include <zmq.hpp>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string_view>
#include "testSchema.pb.h"


int main() {
    zmq::context_t ctx;
    zmq::socket_t sock(ctx,zmq::socket_type::pub);
    sock.bind("ipc:///tmp/zmq_socket");

    Person olivia;
    olivia.set_name("Olivia");
    olivia.set_id(0);

    std::string serialized;
    olivia.SerializeToString(&serialized);

    while (true) {
        sock.send(zmq::buffer(serialized),zmq::send_flags::dontwait);
        std::cout << "Sent Protobuf Message: " << olivia.name() << std::endl;
        sleep(1);
    }

    return 0;
}
