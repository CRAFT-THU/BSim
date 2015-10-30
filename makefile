HOME = .
SRC_DIR = $(HOME)/src
INC_DIR = $(HOME)/include
OBJ_DIR = $(HOME)/obj
BIN_DIR = $(HOME)/bin
LIB_DIR = $(HOME)/lib

TARGET = libsnnsim.a
BIN_TARGET = $(LIB_DIR)/$(TARGET)

CFILES = $(wildcard ${SRC_DIR}/*.c)
CXXFILES = $(wildcard ${SRC_DIR}/*.cpp)
SRC = $(CFILES) $(CXXFILES)
OBJ = $(patsubst %.c,${OBJ_DIR}/%.c.o,$(notdir ${CFILES})) $(patsubst %.cpp,${OBJ_DIR}/%.cpp.o,$(notdir ${CXXFILES}))

CFLAGS =  -O3 -Wall

.PHONY: default all clean test

default: all
all: $(BIN_TARGET)

$(BIN_TARGET): $(OBJ)
	ar cr $@ $^


$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp 
	$(CXX) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR)

$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c 
	$(CXX) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR)

test:
	cd ./test &&  make

clean:
	rm -f  *.o $(OBJ_DIR)/* $(BIN_DIR)/* $(BIN_TARGET)
	cd ./test && make clean
