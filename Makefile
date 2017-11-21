std = -std=c++11

cppFiles = -c main.cpp src/ie_engine.cpp src/ie_const.cpp src/ie_time.cpp \
            src/ie_wavefront.cpp src/ie_mesh.cpp src/ie_vram.cpp\
            src/ie_utils.cpp src/ie_asset_manager.cpp src/ie_shader.cpp \
            src/ie_config.cpp src/ie_import.cpp src/ie_glsl.cpp\
            src/ie_material.cpp src/ie_creation.cpp src/ie_terrain.cpp\
            src/ie_controller.cpp src/ie_terrain_generator.cpp\
            src/ie_asset.cpp src/ie_texture.cpp src/ie_log.cpp\
            src/ie_file_system.cpp
            

objectFiles = main.o ie_engine.o ie_const.o ie_time.o ie_mesh.o ie_material.o\
              ie_wavefront.o ie_utils.o ie_asset_manager.o ie_vram.o\
              ie_shader.o ie_config.o ie_glsl.o ie_terrain.o\
              ie_import.o ie_creation.o ie_terrain_generator.o\
              ie_controller.o ie_asset.o ie_texture.o ie_log.o\
              ie_file_system.o
              

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
