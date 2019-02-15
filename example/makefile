CC         = g++
FLAGS      = -std=c++17 -Wall -I ./src/ -ggdb -O3 -D DEBUG=1 -DNO_X11=0 -fopenmp

LIBS       = -lstdc++ -lm -lGL -lGLU -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lportaudio

SRC_PATH   = .
LIB_PATH   = ../src
OBJ_PATH   = ./obj

SOURCES  := $(wildcard $(LIB_PATH)/*.cpp)
INCLUDES := $(wildcard $(SRC_PATH)/*.h $(LIB_PATH)/*.h)

all: zdlib_demo_water zdlib_demo_orbits zdlib_demo_sound

zdlib_demo_orbits: $(OBJ_PATH)/orbits.o 
	@echo "Linking..."
	$(CC) -o $@ $(OBJ_PATH)/orbits.o $(SOURCES) $(LIBS) $(FLAGS)
	@echo "Done!"

$(OBJ_PATH)/orbits.o: $(SRC_PATH)/orbits.cpp
	@echo "Compiling ORBITS..."
	$(CC) -I $(LIB_PATH) -c -o $@ $< $(FLAGS) $(LIBS)


zdlib_demo_water: $(OBJ_PATH)/water.o 
	@echo "Linking..."
	$(CC) -o $@ $(OBJ_PATH)/water.o $(SOURCES) $(LIBS) $(FLAGS)
	@echo "Done!"

$(OBJ_PATH)/water.o: $(SRC_PATH)/water.cpp
	@echo "Compiling WATER..."
	$(CC) -I $(LIB_PATH) -c -o $@ $< $(FLAGS) $(LIBS)

zdlib_demo_sound: $(OBJ_PATH)/sound.o 
	@echo "Linking..."
	$(CC) -o $@ $(OBJ_PATH)/sound.o $(SOURCES) $(LIBS) $(FLAGS)
	@echo "Done!"

$(OBJ_PATH)/sound.o: $(SRC_PATH)/sound.cpp
	@echo "Compiling SOUND..."
	$(CC) -I $(LIB_PATH) -c -o $@ $< $(FLAGS) $(LIBS)

.PHONY: clean run
clean:
	rm -rf obj/*
	mkdir -p obj
	rm zdlib_demo_*	
	@echo "Cleaned!"

run:
	make -j4 && ./zdlib_demo_*
