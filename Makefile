std = -std=c++11

cppFiles = -c main.cpp src/ie_engine.cpp src/ie_const.cpp src/ie_time.cpp \
            src/ie_camera.cpp src/ie_lighting.cpp src/ie_wavefront.cpp \
            src/ie_utils.cpp src/ie_assets.cpp src/ie_shader.cpp \
            src/ie_config.cpp

objectFiles = main.o ie_engine.o ie_const.o ie_time.o ie_camera.o \
              ie_lighting.o ie_wavefront.o ie_utils.o ie_assets.o \
              ie_shader.o ie_config.o

srcFiles = -lSDL2 -lSDL2main -lSDL2_image -lGL -lGLU

outputFile = -o engine

compile:
	g++ $(std) $(cppFiles) 
	g++ $(std) $(objectFiles) $(srcFiles) $(outputFile)
	rm *.o
test:
	./engine
