import socket
import sys
import json

SERVER_IP = socket.gethostname()


def client_program(port=5000):
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
            data = json.loads(data)  # data loaded
        except socket.error as e:
            print("Error receiving data: %s" % e)
            sys.exit(1)

        info = ""
        i = 0
        for k, v in data.items():
            if str(v).isspace() or v == "":
                v = "EMPTY"
            adding = f"{k} =  {v}\n"
            info = info + adding
            i += 1

        if not data:
            info = "Wrong Data Entered"
        print("The info: \n" + str(info) + "\n")  # show in terminal

        message = input(" Symbol again: ?\n ")  # again take input

    client_socket.close()  # close the connection


if __name__ == "__main__":
    port = int(input("enter port number: "))
    client_program(port)
