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
    //std::copy(pixels, pixels + 100, imgT.begin(0, 0));
    ImageT<float>::Iterator it1 = imgT.begin(0, 0);
    /*ImageT<float>::Iterator it2(it1);
    ImageT<float>::Iterator it3(it2);
    ImageT<float>::Iterator it4(it3);
    ImageT<float>::Iterator it5(it4);*/
    for (int i = 0; i < 100*200; ++i)
      { *it1 = pixels[i]; ++it1; }
    beginTest("Leaving one scope");
  }
  beginTest("Leaving another scope");
  
  /*beginTest("Loading a PPM file from disk");
  img.read("/Users/jlnr/Projekte/GeoAIDA test/Input.ppm");
  
  beginTest("Setting pixels on different channels");
  img.setPixel(0, 0, 0.5, 0);
  img.setPixel(0, 0, 1.0, 1);
  img.setPixel(0, 0, 2.0, 2);
  
  assert(img.getPixel(0, 0, 0) == 0.5);
  assert(img.getPixel(0, 0, 1) == 1.0);
  assert(img.getPixel(0, 0, 2) == 2.0);
  
  beginTest("Using Image iterators");
  *img.begin() = 5;
  assert(*img.begin() == 5);*/
}
