CC = g++

LIB_DIR = ../
LIB_FILE_NAME = PatchFile

EXAMPLE_FILE_NAME = 1_use_cmd
#EXAMPLE_FILE_NAME = 2_use_methods

all: build

build: $(LIB_DIR)$(LIB_FILE_NAME).o $(EXAMPLE_FILE_NAME).o
	$(CC) $(LIB_DIR)$(LIB_FILE_NAME).o $(EXAMPLE_FILE_NAME).o -o $(EXAMPLE_FILE_NAME)

$(LIB_FILE_NAME).o: $(LIB_DIR)$(LIB_FILE_NAME).cpp
	$(CC) -c $(LIB_DIR)$(LIB_FILE_NAME).cpp

$(EXAMPLE_FILE_NAME).o: $(EXAMPLE_FILE_NAME).cpp
	$(CC) -c $(EXAMPLE_FILE_NAME).cpp

clean:
	rm -f *.o && rm -f $(LIB_DIR)/*.o
