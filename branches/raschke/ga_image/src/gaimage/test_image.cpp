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
  Image img;
  img = Image(typeid(double), 100, 200);
  assert(img.sizeX() == 100);
  assert(img.sizeY() == 200);
}