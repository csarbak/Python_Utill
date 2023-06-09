// g++ socket_client.cpp -std=c++17 -o client && ./client
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define BUFFER_SIZE 1024

void client_program(int port)
{
    const char *host = SERVER_IP;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &(server_addr.sin_addr)) <= 0)
    {
        perror("Address-related error connecting to server");
        exit(1);
    }

    if (connect(client_socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
    {
        perror("Connection error");
        exit(1);
    }

    std::cout << "\nType 'bye' to end\n\n";

    char buffer[BUFFER_SIZE] = {0};
    std::string message;

    std::cout << "What transactionTime? ";
    std::getline(std::cin, message);

    while (message != "bye")
    {
        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
        {
            perror("Error sending data");
            exit(1);
        }

        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) == -1)
        {
            perror("Error receiving data");
            exit(1);
        }
        std::string data(buffer);

        std::cout << "Received from server: " << data << "\n\n";

        std::cout << "TransactionTime again? ";
        std::getline(std::cin, message);
    }

    close(client_socket);
}

int main()
{
    client_program(PORT);
    return 0;
}
