#include "network/server.hpp"
void TCPServer::handleClient(int clientSocket, std::mutex &coutMutex) {
        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "[Client disconnected]\n";
                close(clientSocket);
                break;
            }
            
            std::string response;
            std::lock_guard<std::mutex> lock(coutMutex);
            // Echo message back
            response=engine.process(buffer);
            std::cout<<"clinet socket: "<<clientSocket<<"\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }
    }

TCPServer::TCPServer(int portNum) : port(portNum), running(false), server_fd(-1),engine(){
}

bool TCPServer::start() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            perror("Socket creation failed");
            return false;
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        if (bind(server_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Bind failed");
            return false;
        }

        if (listen(server_fd, 5) < 0) {
            perror("Listen failed");
            return false;
        }

        running = true;
        engine.load_db();
        std::cout << "[Server started on port " << port << "]\n";
        while (running) {
            sockaddr_in clientAddr{};
            socklen_t addrLen = sizeof(clientAddr);
            int clientSocket = accept(server_fd, (struct sockaddr*)&clientAddr, &addrLen);
            if (clientSocket < 0) {
                perror("Accept failed");
                continue;
            }

            std::lock_guard<std::mutex> lock(coutMutex);
            
            std::cout << "[New client connected]\n";

            clientThreads.emplace_back(&TCPServer::handleClient,this, clientSocket, std::ref(coutMutex));
        }

        return true;
    }
void TCPServer::stop() {
        running = false;
        close(server_fd);
        for (auto &t : clientThreads) {
            if (t.joinable())
                t.join();
        }
        std::cout << "[Server stopped]\n";
    }
