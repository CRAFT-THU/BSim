HOME = .
CPU_DIR = $(HOME)/cpu
GPU_DIR = $(HOME)/gpu
INC_DIR = $(HOME)/../include
OBJ_DIR = $(HOME)/../obj
BIN_DIR = $(HOME)/../bin
LIB_DIR = $(HOME)/../lib


CFILES = $(wildcard ${CPU_DIR}/*.cpp)
GFILES = $(wildcard ${GPU_DIR}/*.cpp)
COBJ = $(patsubst %.cpp,${OBJ_DIR}/%.cpu.o,$(notdir ${CFILES})) 
GOBJ = $(patsubst %.cpp,${OBJ_DIR}/%.gpu.o,$(notdir ${GFILES}))
BIN_TARGET = $(patsubst %.cpp,${BIN_DIR}/%.cpu.out,$(notdir ${CFILES})) $(patsubst %.cpp,${BIN_DIR}/%.gpu.out,$(notdir ${GFILES}))

CFLAGS =  -g -Wall

.PHONY: default all clean

default: all
all: $(BIN_TARGET)

$(BIN_DIR)/%.cpu.out: $(OBJ_DIR)/%.cpu.o ../lib/libsnnsim.a
	$(CXX) $^ -o $@ 

$(BIN_DIR)/%.gpu.out: $(OBJ_DIR)/%.gpu.o ../lib/libsnnsim_gpu.a
	$(CXX) $^ -o $@ -lcudart -L$(CUDA_HOME)/lib64

$(OBJ_DIR)/%.cpu.o: $(CPU_DIR)/%.cpp 
	$(CXX) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR) -L$(LIB_DIR)

$(OBJ_DIR)/%.gpu.o: $(GPU_DIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ -I$(INC_DIR) -L$(LIB_DIR)

clean:
	rm -f $(GOBJ) $(COBJ) $(BIN_TARGET)
