CC=g++
BUILD=build
SRC=src

example_1:
	$(CC) $(SRC)/example_1.cpp -D_REENTRANT -lpthread -o $(BUILD)/example_1
	$(BUILD)/example_1

3:
	$(CC) $(SRC)/msort.cpp -D_REENTRANT -lpthread -o $(BUILD)/3
	./$(BUILD)/3

4:
	$(CC) $(SRC)/phil.cpp -D_REENTRANT -lpthread -o $(BUILD)/4
	./$(BUILD)/4 1100 1000
clean:
	rm -r ./build/*
