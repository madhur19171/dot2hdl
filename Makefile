CC=gcc
CXX=g++
CFLAGS=
CPPFLAGS=-O3 -g -Wall -static -fpermissive
LDFLAGS=

SRC_DIR	:=	src
BIN_DIR	:=	bin
BUILD_DIR	:=	build

SOURCES	:=	$(shell find $(SRC_DIR) -name "*.cpp")	# Finds all C++ Sources
BPRINTER_DIR	:=	$(shell find $(SRC_DIR) -name "*bprinter")
INC_DIR	:=	$(SRC_DIR)/shared 
OBJECTS	:=	$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))	# Gives a list of all the Object Files that will be created for sources
OBJ_DIR	:=	$(sort $(dir $(OBJECTS)))	# A list of directories where object files will be created. Sort removes duplicates

TARGET=$(BIN_DIR)/dot2hdl

all: makefolder $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CPPFLAGS) $^ -o $@
	
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	echo $@
	$(CXX) -I$(INC_DIR) -I$(BPRINTER_DIR) $(CPPFLAGS) -c $< -o $@

makefolder:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	
echoall:
	echo $(SOURCES)
	echo $(OBJECTS)
	echo $(OBJ_DIR)
	
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	
.PHONY: makefolder all clean
