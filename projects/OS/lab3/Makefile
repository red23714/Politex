CC=g++
BUILD=build
SRC=src

example_1:
	$(CC) $(SRC)/example_1.cpp -D_REENTRANT -lpthread -o $(BUILD)/example_1
	$(BUILD)/example_1

3:
	$(CC) $(SRC)/3.cpp -D_REENTRANT -lpthread -o $(BUILD)/3
	./$(BUILD)/3

4:
	$(CC) $(SRC)/4.cpp -D_REENTRANT -lpthread -o $(BUILD)/4
	./$(BUILD)/4 2000 20
	./$(BUILD)/4 5000 100
clean:
	rm -r ./build/*
