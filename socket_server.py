import socket
import json

def server_program(port=5000):
    # get the hostname
    host = socket.gethostname()

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # get instance
    # look closely. The bind() function takes tuple as argument
    server_socket.bind((host, port))  # bind host address and port together

    # configure how many client the server can listen simultaneously
    server_socket.listen(2)
    conn, address = server_socket.accept()  # accept new connection
    print("Connection from: " + str(address))
    while True:
        info=""
        # receive data stream. it won't accept data packet greater than 1024 bytes
        time = conn.recv(1024).decode()
        data = [json.loads(line)
            for line in open('test_data.json', 'r', encoding='utf-8')]
        for line in data:
            if line["transactTime"] == str(time):
                info= str(line)
                break



        if not time:
            # if data is not received break
            break
        #print("from connected user: " + str(data))
        if info=="":
            info="no data or wrong data entered"
        conn.send(info.encode())  # send data to the client

    conn.close()  # close the connection


if __name__ == '__main__':
    port = int (input("enter port: "))
    server_program(port)