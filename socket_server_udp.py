import socket
import json

SERVER_IP = socket.gethostname()


def server_program():
    # get the hostname
    host = SERVER_IP

    print("host name: " + str(host))
    recv_port = int(input("recv_port: "))
    # server_location = str(input("domain name or ip address of the server: "))
    # port = int (input("This is the port to connect to the server on , should be clients port: "))
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # get instance
    # server_addr = (server_location, port)
    server_socket.bind(("", recv_port))
    # server_socket.connect(server_addr)

    # configure how many client the server can listen simultaneously
    while True:
        info = ""
        # receive data stream. it won't accept data packet greater than 1024 bytes
        bytesAddressPair = server_socket.recvfrom(1024)
        time = bytesAddressPair[0].decode("utf-8")
        if time == "bye":
            break
        data = [
            json.loads(line) for line in open("test_data.json", "r", encoding="utf-8")
        ]
        for line in data:
            if line["transactTime"] == str(time):
                print(f"type : {type(line)} and line : {line}")
                info = str(line)
                break

        if not time:
            # if data is not received break
            break
        # print("from connected user: " + str(data))
        if info == "":
            info = "no data or wrong data entered"
        server_socket.sendto(
            info.encode("utf-8"), bytesAddressPair[1]
        )  # send data to the client


if __name__ == "__main__":
    server_program()
