import socket
import sys
import json
import argparse
from sys import argv
import time

SERVER_IP = socket.gethostname()
PORT = 5000


def client_program(port, exchange, outPut):
    host = SERVER_IP  # as both code is running on same pc

    # client_socket = socket.socket()  # instantiate
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print("Error creating socket: %s" % e)
        sys.exit(1)

    try:
        client_socket.connect((host, port))
    except socket.gaierror as e:
        print("Address-related error connecting to server: %s" % e)
        sys.exit(1)
    except socket.error as e:
        print("Connection error: %s" % e)
        sys.exit(1)

    client_socket.send(exchange.encode())  # send message
    data = None
    while not data:
        data = client_socket.recv(1024).decode()

    client_socket.send(outPut.encode())  # send message
    data = None
    while not data:
        data = client_socket.recv(1024).decode()

    print("\nType 'bye' to end \n")

    message = input(" what Symbol? ")  # take input

    while message.lower().strip() != "bye":
        try:
            client_socket.send(message.encode())  # send message
        except socket.error as e:
            print("Error sending data: %s" % e)
            sys.exit(1)

        try:
            data = client_socket.recv(1024).decode()  # receive response
            if outPut == "json":
                data = json.loads(data)  # data loaded
        except socket.error as e:
            print("Error receiving data: %s" % e)
            sys.exit(1)

        info = ""
        if outPut == "json":
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

    client_socket.close()  # close the connection


if __name__ == "__main__":
    # port = int(input("enter port number: "))
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
    client_program(PORT, args.Exchange_name, args.outputType)
