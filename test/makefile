HOME = .
SRC_DIR = $(HOME)
INC_DIR = $(HOME)/../include
OBJ_DIR = $(HOME)/../obj
BIN_DIR = $(HOME)/../bin
LIB_DIR = $(HOME)/../lib


CFILES = $(wildcard ${SRC_DIR}/*.c)
CXXFILES = $(wildcard ${SRC_DIR}/*.cpp)
SRC = $(CFILES) $(CXXFILES)
OBJ = $(patsubst %.c,${OBJ_DIR}/%.o,$(notdir ${CFILES})) $(patsubst %.cpp,${OBJ_DIR}/%.o,$(notdir ${CXXFILES}))
BIN_TARGET = $(patsubst %.c,${BIN_DIR}/%.out,$(notdir ${CFILES})) $(patsubst %.cpp,${BIN_DIR}/%.out,$(notdir ${CXXFILES}))

CFLAGS =  -O3 -Wall

.PHONY: default all clean

default: all
all: $(BIN_TARGET)

$(BIN_DIR)/%.out: $(SRC_DIR)/%.o ../lib/libsnnsim.a
	$(CXX) $^ -o $@ 


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	$(CXX) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR) -L$(LIB_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR) -L$(LIB_DIR)

clean:
	rm -f $(OBJ) $(BIN_TARGET)
