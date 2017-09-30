std = -std=c++11

cppFiles = -c main.cpp src/ie_engine.cpp src/ie_const.cpp src/ie_time.cpp \
            src/ie_camera.cpp src/ie_lighting.cpp src/ie_wavefront.cpp \
            src/ie_utils.cpp src/ie_assetmanager.cpp src/ie_shader.cpp \
            src/ie_config.cpp src/ie_vram.cpp src/ie_render.cpp \
            src/ie_memory.cpp

objectFiles = main.o ie_engine.o ie_const.o ie_time.o ie_camera.o \
              ie_lighting.o ie_wavefront.o ie_utils.o ie_assetmanager.o \
              ie_shader.o ie_config.o ie_vram.o ie_render.o ie_memory.o

srcFiles = -lSDL2 -lSDL2main -lSDL2_image -lGL -lGLU

outputFile = engine
outputFlag = -o $(outputFile)

compile:
	g++ $(std) $(cppFiles) 
	g++ $(std) $(objectFiles) $(srcFiles) $(outputFlag)
	rm *.o

test:
	./engine

debug:
	g++ -g $(std) $(cppFiles) 
	g++ -g $(std) $(objectFiles) $(srcFiles) $(outputFlag)
	gdb $(outputFile)
