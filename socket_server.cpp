// g++ socket_server.cpp -std=c++17 -o server && ./server

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <arpa/inet.h>
#include <unistd.h>
#include "nlohmann/json.hpp"

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define JSON_FILE "test_data.json"

using json = nlohmann::json;

void server_program(int port)
{
    std::string host = SERVER_IP;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_socket, 2) < 0)
    {
        perror("Listen failed");
        exit(1);
    }

    std::cout << "Server listening on port " << port << std::endl;

    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = accept(server_socket, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_len);
    if (client_socket < 0)
    {
        perror("Accept failed");
        exit(1);
    }

    std::cout << "Client connected" << std::endl;

    char buffer[1024] = {0};
    while (true)
    {
        std::string time;
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer) - 1, 0) == -1)
        {
            perror("Error receiving data");
            exit(1);
        }
        time = buffer;

        if (time.empty() || time == "bye")
            break;

        std::ifstream file(JSON_FILE);
        if (!file)
        {
            std::cerr << "Error opening file: test_data.json" << std::endl;
            break;
        }

        std::string line;
        std::string info = "No data or wrong data entered";
        while (std::getline(file, line))
        {
            try
            {
                json data = json::parse(line);
                std::cout << "json line : " << data << std::endl;

                if (data["transactTime"] == time)
                {
                    info = data.dump();
                    break;
                }
            }
            catch (const json::parse_error &e)
            {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }
        file.close();

        if (send(client_socket, info.c_str(), info.size(), 0) == -1)
        {
            perror("Error sending data");
            exit(1);
        }
    }

    close(client_socket);
    close(server_socket);
}

int main()
{
    server_program(PORT);
    return 0;
}
