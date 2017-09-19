//#include "lib/engine.h"
#include "lib/ie_wavefront.h"//////
#include "lib/ie_assets.h"///////
#include <iostream> //////

int main(int argc, char ** argv)
{
  ie::WavefrontObjectFileReader objReader("data/Cube.obj"); /////
  ie::WavefrontObjectFilePackage pack = objReader.getFilePackage();////
  objReader.read("data/Cubet.obj");
  ie::WavefrontObjectFilePackage pack2 = objReader.getFilePackage();
  ie::AssetManager am;////
  am.unwrapWavefrontObjectFilePackage(pack);
  am.unwrapWavefrontObjectFilePackage(pack2);
  std::cout << am.materialAssets[2].name << std::endl;
  //Engine newgame;
  return 0;
}
