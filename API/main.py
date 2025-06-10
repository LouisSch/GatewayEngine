import socket
import json
import time

def send_orders(orders, host="127.0.0.1", port=9000):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((host, port))

        file = sock.makefile("r")

        for i in range(len(orders)):
            message = json.dumps(orders[i]) + "\n"
            sock.sendall(message.encode("utf-8"))
            response = file.readline().strip()
            print(f"Response: {response}")
            time.sleep(0.5)
def main():
    orders = [
        {"id": "buy1", "symbol": "ETHUSD", "quantity": 5, "price": 3100.5, "side": "BUY"},
        {"id": "sell1", "symbol": "ETHUSD", "quantity": 3, "price": 3100.5, "side": "SELL"},
        {"id": "buy2", "symbol": "BTCUSD", "quantity": 2, "price": 67000.0, "side": "BUY"},
        {"id": "sell2", "symbol": "BTCUSD", "quantity": 4, "price": 3001.2, "side": "SELL"},
    ]

    send_orders(orders)

if __name__ == "__main__":
    main()