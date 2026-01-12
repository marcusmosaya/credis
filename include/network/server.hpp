#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>
#include <map>
#include <engine/engine.hpp>
class TCPServer {
private:
    int server_fd;
    int clients_number;
    const int max_clients=100;
    int port;
    bool running;
    std::vector<std::thread> clientThreads;
    std::mutex coutMutex;
    std::map <std::string,std::string> store;
    Engine engine;
    void handleClient(int clientSocket, std::mutex &coutMutex);
public:
    TCPServer(int portNum);
    bool start();
    void stop();
};
