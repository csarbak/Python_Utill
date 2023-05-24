import socket
import json

TEXT_FILE = "Sql_Python/nasdaqtraded_230510141735.txt"
SERVER_IP = socket.gethostname()


def load_server_program(dic, ccfile):
    # get the hostname
    host = SERVER_IP

    print("host name: " + str(host))
    recv_port = int(input("recv_port: "))
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # get instance
    server_socket.bind(("", recv_port))

    while True:
        info = ""
        # receive data stream. it won't accept data packet greater than 1024 bytes
        bytesAddressPair = server_socket.recvfrom(1024)
        symbol = bytesAddressPair[0].decode("utf-8")

        if not symbol or symbol == "bye":
            break

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

        if notFound == False:
            info = json.dumps(dic)  # data serialized
        else:
            info = json.dumps({})

        server_socket.sendto(
            info.encode("utf-8"), bytesAddressPair[1]
        )  # send data to the client


def init_server(fileObject):
    for aline in fileObject:
        values = aline.split("|")
        keys = values
        dic = dict(zip(keys, [None] * len(keys)))
        break

    return dic


if __name__ == "__main__":
    ccfile = open(TEXT_FILE, "r")
    dic = init_server(ccfile)
    load_server_program(dic, ccfile)
    ccfile.close
