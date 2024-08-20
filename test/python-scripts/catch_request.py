#!/usr/bin/env python3

import socket
import select

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', 8080))
    server_socket.listen(1)

    print("Server is listening on port 8080...")
    connections = []

    while True:
        readable, _, _ = select.select([server_socket] + connections, [], [], 0.5)
        
        for s in readable:
            if s is server_socket:
                # Nova conexão
                client_connection, client_address = server_socket.accept()
                connections.append(client_connection)
                print(f"Connected to {client_address}")
            else:
                # Conexão existente
                raw_request = s.recv(10240)

                if raw_request:
                    print("Received raw request:")
                    print(repr(raw_request.decode('utf-8', errors='replace')))
                else:
                    # Se não houver dados, significa que o cliente fechou a conexão
                    print(f"Client {s.getpeername()} disconnected")
                    connections.remove(s)
                    s.close()

                # Se você quiser enviar uma resposta imediatamente
                http_response = """HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nReceived"""
                s.sendall(http_response.encode('utf-8'))

if __name__ == "__main__":
    start_server()
