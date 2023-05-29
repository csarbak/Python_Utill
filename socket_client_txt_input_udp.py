import socket
import sys
import json
import argparse
from sys import argv

SERVER_IP = socket.gethostname()
FOREIGN_PORT = 5000


def client_program(exchange, outPut):
    # client_socket = socket.socket()  # instantiate
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except socket.error as e:
        print("Error creating socket: %s" % e)
        sys.exit(1)

    # foregn_port = int(input("foregn_port? "))
    # local_port = int(input("local port? "))
    server_addr = (SERVER_IP, FOREIGN_PORT)
    # client_socket.bind(("", local_port))

    client_socket.sendto(exchange.encode("utf-8"), server_addr)
    data = None
    while not data:
        data = client_socket.recvfrom(1024)

    client_socket.sendto(outPut.encode("utf-8"), server_addr)
    data = None
    while not data:
        data = client_socket.recv(1024).decode()

    print("\nType 'bye' to end \n")

    message = input(" what Symbol? ")  # take input

    while message.lower().strip() != "bye":
        try:
            client_socket.sendto(message.encode("utf-8"), server_addr)  # send message
        except socket.error as e:
            print("Error sending data: %s" % e)
            sys.exit(1)

        try:
            bytesAddressPair = client_socket.recvfrom(1024)  # receive response
        except socket.error as e:
            print("Error receiving data: %s" % e)
            sys.exit(1)

        data = bytesAddressPair[0].decode("utf-8")
        if outPut != "txt":
            data = json.loads(data)  # data loaded

            info = ""
            i = 0
            for k, v in data.items():
                if str(v).isspace() or v == "":
                    v = "EMPTY"
                adding = f"{k} =  {v}\n"
                info = info + adding
                i += 1
        else:
            info = data

        if not data:
            info = "Wrong Data Entered"
        print("The info: \n" + str(info) + "\n")  # show in terminal

        message = input(" Symbol again: ?\n ")  # again take input

    try:
        client_socket.sendto(message.encode("utf-8"), server_addr)  # send message
    except socket.error as e:
        print("Error sending data: %s" % e)
        sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="""This is a very basic client program using tcp"""
    )

    parser.add_argument(
        "Exchange_name",
        type=str,
        help="Either NASDAQ, CME ,OR NYSE",
        action="store",
        choices=["NASDAQ", "CME", "NYSE"],
    )

    parser.add_argument(
        "outputType",
        type=str,
        help="This is info goes in the updated table",
        action="store",
        default="txt",
        nargs="?",
        choices=["txt", "json"],
    )

    args = parser.parse_args(argv[1:])
    client_program(args.Exchange_name, args.outputType)
