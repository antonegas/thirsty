include .env

SOURCE_FOLDERS=source source/SDL source/mappers headers/common headers/common/$(OS) source/objects
INCLUDE_FOLDERS=$(LIBRARIES)/include headers headers/objects headers/common headers/common/$(OS)

default : 
	g++ $(foreach dir,$(SOURCE_FOLDERS),$(wildcard $(dir)/*.cpp) $(wildcard $(dir)/*.c)) -o main $(foreach dir,$(INCLUDE_FOLDERS),-I $(dir)) -L $(LIBRARIES)/lib/ -l SDL3 $(FLAGS) -std=c++20

run : default
	./main.exe