HOME = .
SRC_DIR = $(HOME)/src
INC_DIR = $(HOME)/include
OBJ_DIR = $(HOME)/obj
BIN_DIR = $(HOME)/bin

TARGET = libsnnsim.a
BIN_TARGET = $(BIN_DIR)/$(TARGET)

SRC = ($(wildcard ${DIR_SRC}/*.c)  $(wildcard ${DIR_SRC}/*.cpp))
OBJ = ($(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC})) $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})))

CFLAGS =  -O3 -Wall

.PHONY: default all clean

default: all
all: $(BIN)

$(BIN_TARGET): $(OBJ)
	ar $@ $^


%.o: %.cpp 
	$(CXX) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR)

%.o: %.c 
	$(CC) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR)

clean:
	rm -f  *.o $(OBJS) $(BIN)

all:
	echo your template files need work
