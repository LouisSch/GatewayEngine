#include "SymbolOrderBookManager.hpp"
#include "SocketServer.hpp"
#include <iostream>

using namespace std;

Side parseSide(const string& s) {
    return (s == "BUY") ? Side::BUY : Side::SELL;
}

int main() {
    SymbolOrderBookManager manager;
    SocketServer server(9000, manager);

    server.start();
    
    std::cin.get();

    server.stop();

    return 0;
}