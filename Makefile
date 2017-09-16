std = -std=c++11
cppFiles = -c main.cpp -c lib/engine.cpp -c lib/utils.cpp -c lib/ie_const.cpp -c lib/ie_time.cpp -c lib/ie_camera.cpp -c lib/ie_lighting.cpp
objectFiles = main.o engine.o utils.o ie_const.o ie_time.o ie_camera.o ie_lighting.o 
libFiles = -lSDL2 -lSDL2main -lSDL2_image -lGL -lGLU
outputFile = -o engine

compile:
	g++ $(std) $(cppFiles) 
	g++ $(std) $(objectFiles) $(libFiles) $(outputFile)
	 rm *.o
test:
	./engine
