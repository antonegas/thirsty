include .env

default : 
	g++ $(wildcard source/*.cpp) $(wildcard source/*.c) $(wildcard source/SDL/*.cpp) $(wildcard source/mappers/*.cpp) -o main -I $(LIBRARIES)/include/ -I headers/ -I headers/common/ -I headers/common/$(OS)/ -L $(LIBRARIES)/lib/ -l SDL3 $(FLAGS) -std=c++20

run : default
	./main.exe