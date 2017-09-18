//#include "lib/engine.h"
#include "lib/ie_wavefront.h"//////
#include "lib/ie_assets.h"///////
#include <iostream> //////

int main(int argc, char ** argv)
{
  ie::WavefrontObjectFileReader objReader("data/Cube.obj"); /////
  ie::WavefrontObjectFilePackage pack = objReader.getFilePackage();////
  ie::AssetManager am;////
  am.unwrapWavefrontObjectFilePackage(pack);
  //Engine newgame;
  return 0;
}
