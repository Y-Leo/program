LDFLAGS=-L /home/angus/program/OJ/ctemplate/lib
DEBUGFLAGS=-g
BIN=../bin/oj_server
CC=g++

.PHONY:$(BIN)
$(BIN):oj_server.cpp
	$(CC) $^ -o $@ $(DEBUGFLAGS) $(LDFLAGS) -lpthread -lctemplate -ljsoncpp -std=c++11

.PHONY:clean
clean:
	rm -f $(BIN)
