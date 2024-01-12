CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lcurl
INCLUDE_PATH = -I dependencies/SDL2/include -I dependencies/curl/include
LIBRARY_PATH = -L dependencies/SDL2/lib -L dependencies/curl/lib
BIN_DIR = bin

shooterMultiplayer: src/main.cxx
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/shooterMultiplayer src/main.cxx $(INCLUDE_PATH) $(LIBRARY_PATH) $(LDFLAGS)

