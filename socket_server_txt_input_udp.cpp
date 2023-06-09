// g++ socket_server_txt_input_udp.cpp -std=c++17 -o server && ./server

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "nlohmann/json.hpp"

#define TEXT_FILE "../Sql_Python/nasdaqtraded_230512105005.txt"
#define SERVER_IP "127.0.0.1"
#define PORT 5000

using json = nlohmann::json;

void load_server_program(std::map<std::string, std::string> &dic, int port, std::ifstream &ccfile)
{
    std::string host = SERVER_IP;

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    struct sockaddr_in server_address
    {
    };
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &(server_address.sin_addr)) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(1);
    }

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Error binding socket");
        exit(1);
    }

    std::cout << "Server listening on UDP port " << port << std::endl;

    std::string outputType = "txt";

    while (true)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Receive symbol
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);
        ssize_t receivedBytes = recvfrom(server_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_address, &client_address_length);
        if (receivedBytes == -1)
        {
            perror("Error receiving symbol");
            exit(1);
        }
        buffer[receivedBytes] = '\0';

        std::string symbol(buffer);

        if (symbol.empty() || symbol == "bye")
        {
            break;
        }

        symbol = std::string(symbol);

        if (symbol == "CME" || symbol == "NASDAQ" || symbol == "NYSE")
        {
            std::string ex_name = std::string(symbol);
            if (sendto(server_socket, ex_name.c_str(), ex_name.size(), 0, (struct sockaddr *)&client_address, client_address_length) == -1)
            {
                perror("Error sending exchange name");
                exit(1);
            }
            continue;
        }

        if (symbol == "txt" || symbol == "json")
        {
            outputType = std::string(symbol);
            if (sendto(server_socket, outputType.c_str(), outputType.size(), 0, (struct sockaddr *)&client_address, client_address_length) == -1)
            {
                perror("Error sending output type");
                exit(1);
            }
            continue;
        }

        ccfile.seekg(0, std::ios::beg);
        bool notFound = true;
        bool onlyOnce = true;
        std::string line;
        std::string lineCopy;
        std::string lineFirst;
        while (std::getline(ccfile, line))
        {
            if (onlyOnce)
            {
                lineFirst = line;
                onlyOnce = false;
            }
            lineCopy = line;
            std::string token;
            size_t pos = 0;
            int i = 0;

            while ((pos = line.find('|')) != std::string::npos)
            {
                token = line.substr(0, pos);
                line.erase(0, pos + 1);

                if (i == 1 && token == symbol)
                {

                    notFound = false;
                }
                i++;
            }

            if (!notFound)
            {
                pos = 0;
                size_t posF = 0;
                while (((pos = lineCopy.find('|')) != std::string::npos) && ((posF = lineFirst.find('|')) != std::string::npos))
                {
                    dic[lineFirst.substr(0, posF)] = lineCopy.substr(0, pos);
                    lineFirst.erase(0, posF + 1);
                    lineCopy.erase(0, pos + 1);
                }
                break;
            }
        }

        std::string info;
        if (outputType == "json")
        {
            if (!notFound)
            {
                info = json(dic).dump();
            }
            else
            {
                info = json({}).dump();
            }
        }
        else
        {
            if (!notFound)
            {
                info = "{ ";
                for (const auto &[key, value] : dic)
                {
                    info += "\"" + key + "\": \"" + value + "\", ";
                }
                info.pop_back(); // Remove trailing comma
                info.pop_back(); // Remove space after comma
                info += " }";
            }
            else
            {
                info = "Empty please try again";
            }
        }

        if (sendto(server_socket, info.c_str(), info.size(), 0, (struct sockaddr *)&client_address, client_address_length) == -1)
        {
            perror("Error sending data");
            exit(1);
        }
    }

    close(server_socket);
}

std::map<std::string, std::string> init_server(std::ifstream &fileObject)
{
    std::map<std::string, std::string> dic;

    std::string line;
    if (std::getline(fileObject, line))
    {
        size_t pos = 0;
        std::string token;
        while ((pos = line.find('|')) != std::string::npos)
        {
            token = line.substr(0, pos);
            line.erase(0, pos + 1);
            dic[token] = "";
        }
    }

    return dic;
}

int main()
{
    std::ifstream ccfile(TEXT_FILE);
    if (!ccfile.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::map<std::string, std::string> dic = init_server(ccfile);
    load_server_program(dic, PORT, ccfile);

    ccfile.close();

    return 0;
}
