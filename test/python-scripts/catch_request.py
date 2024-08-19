#!/usr/bin/env python3

import socket

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', 8080))
    server_socket.listen(1)

    print("Server is listening on port 8080...")

    while True:
        client_connection, client_address = server_socket.accept()
        raw_request = client_connection.recv(1024)

        print("Received raw request:")
        print(repr(raw_request.decode('utf-8', errors='replace')))

        http_response = """HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nReceived"""
        client_connection.sendall(http_response.encode('utf-8'))
        client_connection.close()

if __name__ == "__main__":
    start_server()
