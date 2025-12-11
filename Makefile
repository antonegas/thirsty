include .env

NES_SOURCE_FOLDERS=source source/SDL source/mappers
SOURCE_FOLDERS=source headers/common headers/common/$(OS) source/objects $(foreach dir,$(NES_SOURCE_FOLDERS),nes/$(dir))
NES_INCLUDE_FOLDERS=headers headers/SDL headers/mappers
INCLUDE_FOLDERS=$(LIBRARIES)/include headers headers/objects headers/common headers/common/$(OS) $(foreach dir,$(NES_INCLUDE_FOLDERS),nes/$(dir))

default : 
	g++ $(foreach dir,$(SOURCE_FOLDERS),$(wildcard $(dir)/*.cpp) $(wildcard $(dir)/*.c)) -o main $(foreach dir,$(INCLUDE_FOLDERS),-I $(dir)) -L $(LIBRARIES)/lib/ -l SDL3 $(FLAGS) -std=c++20

run : default
	./main.exe