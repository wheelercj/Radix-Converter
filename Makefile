OBJS = Radix-Converter/radix.h Radix-Converter/main.cpp Radix-Converter/radix.cpp

CC = g++

OBJ_NAME = Main

all :
	$(CC) $(OBJS) -std=c++11 -o $(OBJ_NAME)
	
