EXECUTABLE = zdlib_test

CC         = g++
FLAGS      = -std=c++17 -Wall -I ./src/ -ggdb $(EM_FLAGS)  -D DEBUG=1 -DNO_X11=0

#EM_FLAGS   = -s USE_GLFW=3 -s DEMANGLE_SUPPORT=1 -s GL_FFP_ONLY=1 --preload-file img
#EXECUTABLE = zdlib_test.js
#CC	  = em++

LIBS       = -lstdc++ -lm -lGL -lGLU -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

SRC_PATH   = .
LIB_PATH   = ../src
OBJ_PATH   = ./obj

SOURCES  := $(wildcard $(SRC_PATH)/*.cpp $(LIB_PATH)/*.cpp)
INCLUDES := $(wildcard $(SRC_PATH)/*.h $(LIB_PATH)/*.h)
OBJECTS  := $(SOURCES:$(SRC_PATH)/%.cpp=$(OBJ_PATH)/%.o)

$(EXECUTABLE): $(OBJECTS) 
	@echo "Linking..."
	$(CC) -o $@ $(OBJECTS) $(LIBS) $(FLAGS)
	@echo "Done!"

$(OBJECTS): $(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp 
	@echo "Compiling $@..."
	$(CC) -I $(LIB_PATH) -c -o $@ $< $(FLAGS)

.PHONY: clean run
clean:
	rm -rf obj/*
	mkdir -p obj
	rm $(EXECUTABLE)	
	@echo "Cleaned!"

run:
	make -j4 && ./${EXECUTABLE}
