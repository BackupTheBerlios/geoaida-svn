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
  img.read("/Users/jlnr/Projekte/GeoAIDA test/Input.ppm");
  printf("Random pixel: %lf %lf %lf\n",
    img.getPixel(0, 1, 0),
    img.getPixel(0, 1, 1),
    img.getPixel(0, 1, 2));
  img.write("/Users/jlnr/Projekte/GeoAIDA test/Input Duplicate.ppm");
  
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
}
