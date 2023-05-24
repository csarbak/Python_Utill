import socket
import sys

SERVER_IP = socket.gethostname()


def client_program():
    # client_socket = socket.socket()  # instantiate
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except socket.error as e:
        print("Error creating socket: %s" % e)
        sys.exit(1)
    foregn_ip = input(
        " This is the domain name or ip address of the server? "
    )  # take input
    foregn_port = int(input("foregn_port? "))
    local_port = int(input("local port? "))
    server_addr = (SERVER_IP, foregn_port)
    client_socket.bind(("", local_port))

    print("\nType 'bye' to end \n")

    message = str(input(" what transactionTime? "))  # take input

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

        msg = bytesAddressPair[0].decode("utf-8")

        print(msg)

        message = input(" time again: ?\n ")  # again take input

    client_socket.close()  # close the connection


if __name__ == "__main__":
    client_program()
