std = -std=c++11
cppFiles = -c main.cpp -c lib/engine.cpp -c lib/utils.cpp
objectFiles = utils.o engine.o main.o
libFiles = -lSDL2 -lSDL2main -lSDL2_image -lGL -lGLU
outputFile = -o engine

compile:
	g++ $(std) $(cppFiles) 
	g++ $(std) $(objectFiles) $(libFiles) $(outputFile)
	rm *.o
test:
	./engine
