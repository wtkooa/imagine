std = -std=c++11
cppFiles = -c main.cpp -c lib/ie_engine.cpp -c lib/ie_const.cpp -c lib/ie_time.cpp -c lib/ie_camera.cpp -c lib/ie_lighting.cpp -c lib/ie_wavefront.cpp -c lib/ie_utils.cpp -c lib/ie_assets.cpp -c lib/ie_shader.cpp 
objectFiles = main.o ie_engine.o ie_const.o ie_time.o ie_camera.o ie_lighting.o ie_wavefront.o ie_utils.o ie_assets.o ie_shader.o
libFiles = -lSDL2 -lSDL2main -lSDL2_image -lGL -lGLU
outputFile = -o engine

compile:
	g++ $(std) $(cppFiles) 
	g++ $(std) $(objectFiles) $(libFiles) $(outputFile)
	 rm *.o
test:
	./engine
