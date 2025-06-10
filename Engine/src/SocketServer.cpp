#include "SocketServer.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <unistd.h>
#include <iostream>

using json = nlohmann::json;

SocketServer::SocketServer(int port, SymbolOrderBookManager& bookManager, size_t nThreads) : port(port), server_fd(-1), running(false), bookManager(bookManager), threadPool(nThreads) {}

void SocketServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) throw std::runtime_error("Socket creation error.");

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd);
        throw std::runtime_error("[SocketServer] Bind error on socket server start.");
    }

    if (listen(server_fd, 3) < 0) {
        close(server_fd);
        throw std::runtime_error("[SocketServer] Listening error on socket server start.");
    }

    running = true;
    server_thread = std::thread(&SocketServer::serverLoop, this);
}

void SocketServer::stop() {
    running = false;
    close(server_fd);
    if (server_thread.joinable()) server_thread.join();
}

void SocketServer::serverLoop() {
    safePrint("[SocketServer] Listening on port ", port, "...");

    while (running) {
        sockaddr_in client_addr{};
        socklen_t addrLen = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrLen);

        if (client_socket < 0) {
            safePrint("[SocketServer] Error while accepting client.");
            continue;
        } 

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        threadPool.enqueue(&SocketServer::handleClient, this, client_socket);
    }
}

void SocketServer::handleClient(int client_socket) {
    std::string buffer;
    char chunk[1024];

    while (true) {
        ssize_t bytesRead = recv(client_socket, chunk, sizeof(chunk) - 1, 0);
        // unsigned int bytesRead = read(client_socket, buffer, sizeof(buffer));

        if (bytesRead <= 0) {
            safePrint("[SocketServer] Client disconnected");
            break;
        }

        // if (bytesRead >= sizeof(chunk)) bytesRead = sizeof(chunk) - 1;
        chunk[bytesRead] = '\0';
        buffer += chunk;

        size_t pos;
        while ((pos = buffer.find('\n')) != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 1);

            if (line.empty()) continue;

            try {
                json j = json::parse(line);
            
                safePrint("[SocketServer] Order received: ", j.dump());

                if (!j.contains("id") || !j.contains("symbol") || !j.contains("price") || !j.contains("quantity") || !j.contains("side")) {
                    throw std::runtime_error("Missing required order fields.");
                }

                Order order;
                order.id = j["id"].get<std::string>();
                order.symbol = j["symbol"].get<std::string>();
                order.price = j["price"].get<double>();
                order.quantity = j["quantity"].get<int>();
                std::string sideStr = j["side"].get<std::string>();
                order.side = (sideStr == "BUY") ? Side::BUY : Side::SELL;
                order.timestamp = std::chrono::steady_clock::now();

                bookManager.addOrder(order);
                
                json response = { {"status", "Order received and processed."} };
                std::string responseStr = response.dump() + "\n";
                send(client_socket, responseStr.c_str(), responseStr.size(), 0);
            } catch(const std::exception& e) {
                safePrint("[SocketServer] Invalid order format or parse error: ", e.what());

                json errorResponse = { {"error", e.what()} };
                std::string errorStr = errorResponse.dump() + "\n";
                send(client_socket, errorStr.c_str(), errorStr.size(), 0);
            }
        }

        // try {
        //     json j = json::parse(buffer);
            
        //     safePrint("[SocketServer] Order received: ", j.dump());

        //     if (!j.contains("id") || !j.contains("symbol") || !j.contains("price") || !j.contains("quantity") || !j.contains("side")) {
        //         throw std::runtime_error("Missing required order fields.");
        //     }

        //     Order order;
        //     order.id = j["id"].get<std::string>();
        //     order.symbol = j["symbol"].get<std::string>();
        //     order.price = j["price"].get<double>();
        //     order.quantity = j["quantity"].get<int>();
        //     std::string sideStr = j["side"].get<std::string>();

        //     if (sideStr == "BUY") order.side = Side::BUY;
        //     else if (sideStr == "SELL") order.side = Side::SELL;
        //     else throw std::invalid_argument("Invalid side: must be BUY or SELL.");

        //     order.timestamp = std::chrono::steady_clock::now();

        //     bookManager.addOrder(order);
            
        //     json response = { {"status", "Order received and processed."} };
        //     std::string responseStr = response.dump() + "\n";
        //     send(client_socket, responseStr.c_str(), responseStr.size(), 0);
        // } catch (const std::exception& e) {
        //     safePrint("[SocketServer] Invalid order format or parse error: ", e.what());

        //     json errorResponse = { {"error", e.what()} };
        //     std::string errorStr = errorResponse.dump() + "\n";
        //     send(client_socket, errorStr.c_str(), errorStr.size(), 0);
        // }
    }

    close(client_socket);
}