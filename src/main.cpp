#include <iostream>
#include <network/server.hpp>
int main() {
    TCPServer server(8080);
    if (server.start()) {
        std::cout << "Running";
        std::cin.get();
        server.stop();
    }
    return 0;
}