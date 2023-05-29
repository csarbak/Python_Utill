import socket
import json

TEXT_FILE = "../Sql_Python/nasdaqtraded_230512105005.txt"
SERVER_IP = socket.gethostname()
PORT = 5000


def load_server_program(dic, port, ccfile):
    # get the hostname
    host = SERVER_IP

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # get instance
    # look closely. The bind() function takes tuple as argument
    server_socket.bind((host, port))  # bind host address and port together

    # configure how many client the server can listen simultaneously
    server_socket.listen(4)
    conn, address = server_socket.accept()  # accept new connection
    print("Connection from: " + str(address))
    outputType = "txt"
    while True:
        # receive data stream. it won't accept data packet greater than 1024 bytes

        symbol = conn.recv(1024).decode()
        print(f"symbol: {symbol}")

        if not symbol or symbol == "bye":
            break
        symbol = str(symbol)

        if symbol == "CME" or symbol == "NASDAQ" or symbol == "NYSE":
            ex_name = str(symbol)
            conn.send(ex_name.encode())  # send data to the client

            continue

        if symbol == "txt" or symbol == "json":
            outputType = str(symbol)
            conn.send(outputType.encode())  # send data to the client
            continue

        ccfile.seek(0)
        notFound = True
        for aline in ccfile:
            values = aline.split("|")
            if values[1] == str(symbol):
                notFound = False
                i = 0
                for k, v in dic.items():
                    dic[k] = values[i]
                    i += 1
                break

        if outputType == "json":
            if notFound == False:
                info = json.dumps(dic)  # data serialized
            else:
                info = json.dumps({})
        else:
            if notFound == False:
                info = str(dic)  # data serialized
            else:
                info = "Empty please try again"

        conn.send(info.encode())  # send data to the client

    conn.close()  # close the connection


def init_server(fileObject):
    for aline in fileObject:
        values = aline.split("|")
        keys = values
        dic = dict(zip(keys, [None] * len(keys)))
        break

    return dic


if __name__ == "__main__":
    ccfile = open(TEXT_FILE, "r")
    # port = int(input("enter port: "))
    dic = init_server(ccfile)
    load_server_program(dic, PORT, ccfile)
    ccfile.close
