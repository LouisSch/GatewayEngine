#pragma once

#include <thread>

#include "Order.hpp"
#include "SymbolOrderBookManager.hpp"
#include "ThreadPool.hpp"

class SocketServer {
private:
    int port;
    int server_fd;
    bool running;
    std::thread server_thread;
    SymbolOrderBookManager& bookManager;
    ThreadPool threadPool;
    std::mutex coutMutex;

    void serverLoop();
    void handleClient(int client_socket);

    template<typename... Args>
    void safePrint(Args&&... args) {
        std::lock_guard<std::mutex> lock(coutMutex);
        (std::cout << ... << args) << std::endl;
    }
public:
    SocketServer(int port, SymbolOrderBookManager& bookManager, size_t nThreads = 5);
    void start();
    void stop();
};