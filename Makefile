CC = gcc
CFLAGS = -Wall -std=c11 -g
CDIR = -DCDIR=\"cards\"
LDFLAGS = -Lbin
INC = include/
SRC = src/
BIN = bin/


parser: $(BIN)libvcparser.so


test: exec


exec: $(SRC)main.o $(BIN)libvcparser.so
	$(CC) $(CFLAGS) $(CDIR) $(LDFLAGS) -o exec $(SRC)main.o -lvcparser


$(SRC)main.o: $(SRC)main.c $(INC)VCParser.h $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) $(CDIR) -I$(INC) -c $(SRC)main.c -o $(SRC)main.o


$(BIN)libvcparser.so: $(SRC)VCParser.o $(SRC)LinkedListAPI.o $(SRC)VCHelpers.o | $(BIN)
	$(CC) -shared -o $(BIN)libvcparser.so $(SRC)VCParser.o $(SRC)LinkedListAPI.o $(SRC)VCHelpers.o


$(SRC)VCParser.o: $(SRC)VCParser.c $(INC)VCParser.h $(INC)LinkedListAPI.h $(INC)VCHelpers.h
	$(CC) -I$(INC) $(CFLAGS) $(CDIR) -c -fpic $(SRC)VCParser.c -o $(SRC)VCParser.o


$(SRC)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) -I$(INC) $(CFLAGS) $(CDIR) -c -fpic $(SRC)LinkedListAPI.c -o $(SRC)LinkedListAPI.o


$(SRC)VCHelpers.o: $(SRC)VCHelpers.c $(INC)VCHelpers.h $(INC)VCParser.h $(INC)LinkedListAPI.h
	$(CC) -I$(INC) $(CFLAGS) $(CDIR) -c -fpic $(SRC)VCHelpers.c -o $(SRC)VCHelpers.o


$(BIN):
	mkdir -p $(BIN)


clean:
	rm -rf parser $(SRC)*.o $(BIN)libvcparser.so
