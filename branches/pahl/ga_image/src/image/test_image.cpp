#include <cassert>
#include <gaimage.h>
#include <gaimaget.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace Ga;

void beginTest(const std::string& test)
{
  std::cout << test << std::endl;
}

int main()
{	
  beginTest("Creating/assigning image of a specified size");
  Image img(typeid(float), 100, 200, 3);
  assert(img.sizeX() == 100);
  assert(img.sizeY() == 200);
  
  beginTest("Using ImageT iterators");
  ImageT<float>& imgT = dynamic_cast<ImageT<float>&>(*img.pImage());
  {
    float pixels[100 * 200];
    std::copy(pixels, pixels + 100, imgT.begin(0, 0));
    beginTest("Leaving one scope");
  }
  beginTest("Leaving another scope");
  
  beginTest("Loading a PPM file from disk");
  img.read("../tests/fixtures/face.ppm");

  beginTest("Setting pixels on different channels");
  img.setPixel(0, 0, 10, 0);
  img.setPixel(0, 0, 20, 1);
  img.setPixel(0, 0, 30, 2);
  
  assert(img.getPixel(0, 0, 0) == 10);
  assert(img.getPixel(0, 0, 1) == 20);
  assert(img.getPixel(0, 0, 2) == 30);
  
  beginTest("Using Image iterators");
  *img.begin() = 5;
  assert(*img.begin() == 5);  

  beginTest("Creating empty image and loading a PGM file to it");
  Image img2(typeid(unsigned char));
  img2.read("../tests/fixtures/face.ppm");
  assert(img.sizeX() == img2.sizeX());
  assert(img.sizeY() == img2.sizeY());
  assert(img.noChannels() == img2.noChannels());
  assert(img.getPixel(10, 10, 1) == img2.getPixel(10, 10, 1));
}
