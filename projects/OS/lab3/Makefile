CC=g++
BUILD=build
SRC=src

1_win:
	x86_64-w64-mingw32-g++ $(SRC)/expr.cpp -o $(BUILD)/expr.exe -static -static-libgcc -static-libstdc++
	wine $(BUILD)/expr.exe

1:
	$(CC) $(SRC)/expr.cpp -D_REENTRANT -lpthread -g -o $(BUILD)/expr
	$(BUILD)/expr

2:
	$(CC) $(SRC)/qsort.cpp -D_REENTRANT -lpthread -g -o $(BUILD)/qsort
	$(BUILD)/qsort

2_win:
	x86_64-w64-mingw32-g++ $(SRC)/qsort.cpp -o $(BUILD)/qsort.exe -static -static-libgcc -static-libstdc++
	wine $(BUILD)/qsort.exe

3:
	$(CC) $(SRC)/msort.cpp -D_REENTRANT -lpthread -g -o $(BUILD)/msort
	$(BUILD)/msort


clean:
	rm -r ./build/*
