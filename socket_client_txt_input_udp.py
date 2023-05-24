import socket
import sys
import json

SERVER_IP = socket.gethostname()


def client_program():
    # client_socket = socket.socket()  # instantiate
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except socket.error as e:
        print("Error creating socket: %s" % e)
        sys.exit(1)

    foregn_port = int(input("foregn_port? "))
    # local_port = int(input("local port? "))
    server_addr = (SERVER_IP, foregn_port)
    # client_socket.bind(("", local_port))

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
        data = json.loads(data)  # data loaded

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

    try:
        client_socket.sendto(message.encode("utf-8"), server_addr)  # send message
    except socket.error as e:
        print("Error sending data: %s" % e)
        sys.exit(1)


if __name__ == "__main__":
    client_program()
