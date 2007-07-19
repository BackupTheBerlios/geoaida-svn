#include <cassert>
#include <gaimage.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace Ga;

void beginTest(const std::string& test)
{
  std::cout << test << std::endl;
}

int main()
{
  beginTest("Creating/assigning image of a specified size");
  Image //img;
  img = Image(typeid(float), 100, 200, 3);
  assert(img.sizeX() == 100);
  assert(img.sizeY() == 200);
  beginTest("Setting pixels on different channels");
  img.setPixel(0, 0, 0.0, 0);
  img.setPixel(0, 0, 1.0, 1);
  img.setPixel(0, 0, 2.0, 2);
  assert(img.getPixel(0, 0, 0) == 0.0);
  assert(img.getPixel(0, 0, 1) == 1.0);
  assert(img.getPixel(0, 0, 2) == 2.0);
  beginTest("Using iterators");
  *img.begin() = 5;
  assert(*img.begin() == 5);
}
