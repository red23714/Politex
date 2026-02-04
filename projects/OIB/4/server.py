import socket

def server_program():
    host = '0.0.0.0'
    port = 8080

    server_socket = socket.socket()
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    server_socket.bind((host, port))
    server_socket.listen(1)
    conn, address = server_socket.accept()
    print("Connection from: " + str(address))

    data = input('file name: ')
    conn.send(data.encode())
    
    conn.close()

server_program()