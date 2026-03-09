CC=gcc
BUILD=build
SRC=src
FIRST=1

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
	$(CC) $(SRC)/$(FIRST)/* -o $(BUILD)/tcpclient
	./$(BUILD)/tcpclient 192.168.50.7:9000 file1.txt

clean:
	rm -r ./build/*
