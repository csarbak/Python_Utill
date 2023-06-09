// g++ socket_server_txt_input_multi.cpp -std=c++17 -o server && ./server
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include "nlohmann/json.hpp"
#include "config/include/tao/config.hpp"

#define TEXT_FILE "psxtraded_230510141735.txt"
#define OUT_FILE "example.txt"
#define SERVER_IP "127.0.0.1"
#define PORT 5000

using json = nlohmann::json;
std::map<std::string, std::string> loadDic(std::ifstream &fileObject)
{
    std::map<std::string, std::string> dic;

    std::string line;
    while (std::getline(fileObject, line))
    {
        std::string token;
        std::string ogLine = line;
        size_t pos = 0;
        int i = 0;

        while ((pos = line.find('|')) != std::string::npos)
        {
            token = line.substr(0, pos);
            line.erase(0, pos + 1);

            if (i == 1)
            {
                dic[token] = ogLine;
                break;
            }
            i++;
        }
    }
    return dic;
}

void handleClient(int client_socket, std::map<std::string, std::string> &dicFile)
{
    std::string outputType = "txt";
    std::map<std::string, std::string> dic;

    while (true)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Receive symbol
        ssize_t receivedBytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (receivedBytes == -1)
        {
            perror("Error receiving symbol");
            exit(1);
        }
        buffer[receivedBytes] = '\0';

        std::string symbol = buffer;
        std::cout << "symbol: " << symbol << std::endl;

        if (symbol.empty() || symbol == "bye")
        {
            break;
        }

        if (symbol == "CME" || symbol == "NASDAQ" || symbol == "NYSE")
        {
            std::string ex_name = std::string(symbol);
            if (send(client_socket, ex_name.c_str(), ex_name.size(), 0) == -1)
            {
                perror("Error sending exchange name");
                exit(1);
            }
            continue;
        }

        if (symbol == "txt" || symbol == "json")
        {
            outputType = std::string(symbol);
            if (send(client_socket, outputType.c_str(), outputType.size(), 0) == -1)
            {
                perror("Error sending output type");
                exit(1);
            }
            continue;
        }
        bool notFound = true;
        if (dicFile.find(symbol) == dicFile.end())
        {
            // not found
            std::cout << "not found: " << std::endl;
        }
        else
        {
            // found
            notFound = false;
            size_t pos = 0;
            size_t posF = 0;
            std::string lineCopy = dicFile[symbol];
            std::string lineFirst = dicFile["Symbol"];
            while (((pos = lineCopy.find('|')) != std::string::npos) && ((posF = lineFirst.find('|')) != std::string::npos))
            {
                dic[lineFirst.substr(0, posF)] = lineCopy.substr(0, pos);
                lineFirst.erase(0, posF + 1);
                lineCopy.erase(0, pos + 1);
            }
            dic[lineFirst.substr(0, posF)] = lineCopy.substr(0, pos);
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

        if (send(client_socket, info.c_str(), info.size(), 0) == -1)
        {
            perror("Error sending data");
            exit(1);
        }
    }

    close(client_socket);
}

void load_server_program(int port, std::ifstream &ccfile, std::map<std::string, std::string> &dicFile)
{
    std::string host = SERVER_IP;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
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

    if (listen(server_socket, 4) == -1)
    {
        perror("Error listening on socket");
        exit(1);
    }
    std::ofstream myfile;
    myfile.open(OUT_FILE);
    myfile << "Writing this to a file.\n";
    while (true)
    {
        int client_socket;
        struct sockaddr_in client_address
        {
        };
        socklen_t client_address_length = sizeof(client_address);

        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == -1)
        {
            perror("Error accepting connection");
            exit(1);
        }

        std::cout << "Connection from: " << inet_ntoa(client_address.sin_addr) << std::endl;
        myfile << "Connection from: " << inet_ntoa(client_address.sin_addr) << std::endl;

        std::thread client_thread(handleClient, client_socket, std::ref(dicFile));
        client_thread.detach();
    }

    close(server_socket);
    myfile.close();
}

int main()
{
    const tao::config::value config = tao::config::from_file("foo.cfg");
    std::ifstream ccfile(TEXT_FILE);
    if (!ccfile.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::map<std::string, std::string> dicFile = loadDic(ccfile);
    ccfile.close();
    load_server_program(PORT, ccfile, dicFile);

    return 0;
}
