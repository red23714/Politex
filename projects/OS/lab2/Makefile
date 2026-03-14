CC=gcc
BUILD=build
SRC=src
FIRST=1
SECOND=2

all: tcpclient tcpserver udpclient udpserver

simple_tcp:
	$(CC) $(SRC)/simple_tcp.c -o $(BUILD)/simple_tcp
	./$(BUILD)/simple_tcp

simple_udp:
	$(CC) $(SRC)/simple_udp.c -o $(BUILD)/simple_udp
	./$(BUILD)/simple_udp

simple_server_tcp:
	$(CC) $(SRC)/simple_server_tcp.c -o $(BUILD)/simple_server_tcp
	./$(BUILD)/simple_server_tcp

simple_server_udp:
	$(CC) $(SRC)/simple_server_udp.c -o $(BUILD)/simple_server_udp
	./$(BUILD)/simple_server_udp

tcpclient:
	$(CC) $(SRC)/$(FIRST)/tcpclient.cpp -o $(BUILD)/tcpclient

tcpserver:
	$(CC) $(SRC)/$(FIRST)/tcpserver.cpp -o $(BUILD)/tcpserver

udpclient:
	$(CC) $(SRC)/$(SECOND)/udpclient.cpp -o $(BUILD)/udpclient

udpserver:
	$(CC) $(SRC)/$(SECOND)/udpserver.cpp -o $(BUILD)/udpserver

clean:
	rm -r ./build/*
