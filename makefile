CC      		:= gcc

TARGET			:= BaseI6_updater
VERSION			:= 1.2

LIB_DIR			:= lib
INC_DIR			:= inc
SRC_DIR			:= src
OBJ_DIR			:= obj
BIN_DIR			:= bin
VER_DIR			:= $(BIN_DIR)/$(VERSION)

DEFINES			:= -DDATE='"'$$DATE'" -D_DEBUG'
STATICLIBS  := 

### ARCH DETECTION ###
ifeq ($(ARCH),)
	ifeq ($(PROCESSOR_ARCHITECTURE),)
		ARCH := $(shell uname -m)
	else
		ARCH := PROCESSOR_ARCHITECTURE
	endif
endif

# Windows
ifneq ($(PROCESSOR_ARCHITEW6432),)
	PROCESSOR_ARCHITECTURE = $(PROCESSOR_ARCHITEW6432)
endif

# Non-windows
ifeq ($(PROCESSOR_ARCHITECTURE),)
	ifneq ($(filter x86_64%,$(ARCH)),)
		ARCH := x86_64
	else ifneq ($(filter %86,$(ARCH)),)
		ARCH := x86
	else ifneq ($(filter %arm,$(ARCH)),)
		ARCH := arm
	else ifneq ($(filter arm%,$(ARCH)),)
		ARCH := arm
	else ifneq ($(filter %ARM,$(ARCH)),)
		ARCH := arm
	else ifneq ($(filter ARM%,$(ARCH)),)
		ARCH := arm
	endif
endif

# Clean up various flavors
ifeq ($(ARCH),ia32)
	ARCH := x86
else ifeq ($(ARCH),IA32)
	ARCH := x86
else ifeq ($(ARCH),ia64)
	ARCH := x86_64
else ifeq ($(ARCH),IA64)
	ARCH := x86_64
else ifeq ($(ARCH),amd64)
	ARCH := x86_64
else ifeq ($(ARCH),AMD64)
	ARCH := x86_64
else ifeq ($(ARCH),x64)
	ARCH := x86_64
else ifeq ($(ARCH),ARM)
	ARCH := arm
endif
### END ARCH DETECTION ###


### OS DETECTION ###
# Set up compiler flags for OS
ifeq ($(OS),)
	OS := $(shell uname -s)
endif
ifeq ($(OS),Windows_NT)
	OS = Windows
endif

ifeq ($(OS),Windows)
	TARGET = $(TARGET).exe
	DEFINES	+= -DWIN
else ifeq ($(OS),Linux)
	DEFINES	+= -DLINUX
else ifeq ($(OS), Darwin)
	DEFINES	+= -DOSX
endif

ifeq ($(ARCH),x86_64)
	BITS = 64
	ifeq ($(OS), Windows)
		CC = x86_64-w64-mingw32-gcc
	endif
	DEFINES += -DX86_64
else ifeq ($(ARCH),x86)
	BITS = 32
	ifeq ($(OS), Windows)
		CC = i686-w64-mingw32-gcc
	endif
	CCFLAGS += -m32
	LDFLAGS += -m32
	DEFINES += -DX86
else ifeq ($(ARCH),arm)
	BITS = 32
	ifeq ($(OS), Windows)
		CC = NEED_MING_ARM_COMPILER
	else
		CC = NEED_ARM_COMPILER
	endif
	DEFINES += -DARM
endif

SYSTEM	= $(OS)_$(ARCH)
### END OS DETECTION ###


SYS_DIR = $(VER_DIR)/$(SYSTEM)

LIBS			= 
INCLUDES	= -I/usr/local/include/

CFLAGS=-std=c99 -Wall -I$(INC_DIR)
LDFLAGS=-Wall -I. -lm -lserialport

SRC= $(wildcard $(SRC_DIR)/*.c)
OBJ= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


all: directories $(SYS_DIR)/$(TARGET)


directories: $(BIN_DIR) $(OBJ_DIR) $(SYS_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(SYS_DIR):
	mkdir -p $(SYS_DIR)	


$(SYS_DIR)/$(TARGET): $(OBJ)
	@$(CC) $(CFLAGS) $(DEFINES) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	@echo "Linking:" $< "successfully linked."

$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@ $(LIBS)
	@echo "Compiling:" $< "successfully compiled."


run: all
	./$(SYS_DIR)/$(TARGET) -n 3 -b 115200 -a -r
	
clean:
	@rm -f $(OBJ)
	@echo "Objects successfully cleaned"

cleanall: clean
	@rm -f $(SYS_DIR)/$(TARGET)
	@echo "Binaries successfully cleaned"


.PHONY: clean cleanall
