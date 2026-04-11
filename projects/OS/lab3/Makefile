CC=g++
BUILD=build
SRC=src

1:
	$(CC) $(SRC)/expr.cpp -D_REENTRANT -lpthread -g -o $(BUILD)/expr
	$(BUILD)/expr
clean:
	rm -r ./build/*
