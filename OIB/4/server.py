import socket

def server_program():
    host = '192.168.56.1'
    port = 5000

    server_socket = socket.socket()

    server_socket.bind((host, port))
    server_socket.listen(2)
    conn, address = server_socket.accept()
    print("Connection from: " + str(address))

    data = input('file name: ')
    conn.send(data.encode())
    
    conn.close()

server_program()