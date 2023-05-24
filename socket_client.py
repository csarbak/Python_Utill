import socket
import sys 

def client_program(port = 5000):
    host = socket.gethostname()  # as both code is running on same pc

    #client_socket = socket.socket()  # instantiate
    try: 
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
    except socket.error as e: 
        print ("Error creating socket: %s" % e) 
        sys.exit(1) 

    try: 
        client_socket.connect((host, port)) 
    except socket.gaierror as e: 
        print ("Address-related error connecting to server: %s" % e) 
        sys.exit(1) 
    except socket.error as e: 
        print ("Connection error: %s" % e) 
        sys.exit(1) 

    print("\nType 'bye' to end \n")

    message = input(" what transactionTime? ")  # take input

    while message.lower().strip() != 'bye':
        try: 
            client_socket.send(message.encode())  # send message
        except socket.error as e: 
            print ("Error sending data: %s" % e) 
            sys.exit(1) 

        try: 
            data = client_socket.recv(1024).decode()  # receive response
        except socket.error as e: 
            print ("Error receiving data: %s" % e) 
            sys.exit(1) 
        

        print('Received from server: ' + data + "\n")  # show in terminal

        message = input(" time again: ?\n ")  # again take input

    client_socket.close()  # close the connection


if __name__ == '__main__':
    port = int (input("enter port number: "))
    client_program(port)