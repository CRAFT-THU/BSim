HOME = .
SRC_DIR = $(HOME)/src
INC_DIR = $(HOME)/include
OBJ_DIR = $(HOME)/obj
BIN_DIR = $(HOME)/bin
LIB_DIR = $(HOME)/lib

TARGET = libsnnsim.a
BIN_TARGET = $(LIB_DIR)/$(TARGET)

SPACE := $(eval) $(eval)
DIRS := $(shell find $(SRC_DIR) -maxdepth 4 -type d)
VPATH = $(subst  $(SPACE),:,$(DIRS)):.
CFILES = $(foreach dir, $(DIRS), $(wildcard ${dir}/*.c))
CXXFILES = $(foreach dir, $(DIRS), $(wildcard ${dir}/*.cpp))
SRC = $(CFILES) $(CXXFILES)
OBJ += $(patsubst %.c,${OBJ_DIR}/%.c.o,$(notdir ${CFILES})) $(patsubst %.cpp,${OBJ_DIR}/%.cpp.o,$(notdir ${CXXFILES}))

CFLAGS =  -O2 -Wall -std=c++11

.PHONY: default all clean test

default: all
all: $(BIN_TARGET)

$(BIN_TARGET): $(OBJ)
	ar cr $@ $^


$(OBJ_DIR)/%.cpp.o: %.cpp 
	$(CXX) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR)

$(OBJ_DIR)/%.c.o: %.c 
	$(CXX) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR)

test:
	cd ./test &&  make

clean:
	rm -f  *.o $(OBJ_DIR)/* $(BIN_DIR)/* $(BIN_TARGET)
	cd ./test && make clean
