// g++ socket_client_txt_input.cpp -std=c++17 -o client && ./client CME txt

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sstream>
#include <arpa/inet.h>
#include <unistd.h>
#include "nlohmann/json.hpp"

#define SERVER_IP "127.0.0.1"
#define PORT 5000

using json = nlohmann::json;

void client_program(int port, const std::string &exchange, const std::string &outputType)
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0)
    {
        perror("Address-related error connecting to server");
        exit(1);
    }

    if (connect(client_socket, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
    {
        perror("Connection error");
        exit(1);
    }

    if (send(client_socket, exchange.c_str(), exchange.size(), 0) == -1)
    {
        perror("Error sending data");
        exit(1);
    }

    char buffer[1024] = {0};
    if (recv(client_socket, buffer, 1024, 0) == -1)
    {
        perror("Error receiving data");
        exit(1);
    }
    std::string response(buffer);

    if (send(client_socket, outputType.c_str(), outputType.size(), 0) == -1)
    {
        perror("Error sending data");
        exit(1);
    }

    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, 1024, 0) == -1)
    {
        perror("Error receiving data");
        exit(1);
    }
    std::string data(buffer);

    std::cout << "\nType 'bye' to end\n";

    std::string message;
    std::cout << "What Symbol? ";
    while (std::getline(std::cin, message))
    {
        if (message == "bye")
        {
            break;
        }

        if (send(client_socket, message.c_str(), message.size(), 0) == -1)
        {
            perror("Error sending data");
            exit(1);
        }

        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, 1024, 0) == -1)
        {
            perror("Error receiving data");
            exit(1);
        }
        std::string response(buffer);

        std::string info;
        if (outputType == "json")
        {
            json jsonData;
            try
            {
                jsonData = json::parse(response);
                int i = 0;
                for (const auto &[key, value] : jsonData.items())
                {
                    std::string strValue = value.get<std::string>();
                    if (strValue.empty() || std::isspace(strValue[0]))
                    {
                        strValue = "EMPTY";
                    }
                    std::ostringstream oss;
                    oss << key << " = " << strValue << "\n";
                    info += oss.str();
                    i++;
                }
            }
            catch (const json::parse_error &e)
            {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }
        else
        {
            info = response;
        }

        if (info.empty())
        {
            info = "Wrong Data Entered";
        }
        std::cout << "The info:\n"
                  << info << std::endl;

        std::cout << "Symbol again? ";
    }

    close(client_socket);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Invalid number of arguments.\n";
        std::cerr << "Usage: ./client <Exchange_name> <outputType>\n";
        std::cerr << "Exchange_name: Either NASDAQ, CME, or NYSE\n";
        std::cerr << "outputType: txt or json\n";
        return 1;
    }

    int port = PORT;
    std::string exchange = argv[1];
    std::string outputType = argv[2];

    client_program(port, exchange, outputType);

    return 0;
}
