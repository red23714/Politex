CC=gcc
BUILD=build
SRC=src

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


clean:
	rm -r ./build/*
