/***************************************************************************
                          gaimage.h  -  geoAIDA image class
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : Martin Pahl, Jürgen Bückner, Oliver Stahlhut,
                           Julian Raschke
    email                : pahl@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <typeinfo>
#include "gaimage.h"
#include "gaimaget.h"
#include "gaimagebase.h"
#include "gaimageio.h"

namespace Ga
{

Image::Image(const std::type_info& t, int x, int y, int noChannels)
{
  #define TRY_TYPE(type) \
    if (t == typeid(type)) pImage_ = new ImageT<type>(x, y, noChannels);

  TRY_TYPE(bool) else
  TRY_TYPE(char) else
  TRY_TYPE(signed char) else
  TRY_TYPE(unsigned char) else
  TRY_TYPE(signed short) else
  TRY_TYPE(unsigned short) else
  TRY_TYPE(signed int) else
  TRY_TYPE(unsigned int) else
  TRY_TYPE(float) else
    throw std::logic_error(std::string("Unsupported type ") + t.name());
  
  #undef TRY_TYPE
}

Image::Image(const std::string& filename)
: pImage_(0)
{ 
  std::auto_ptr<ImageIO> io = ImageIO::reopen(filename);
  
  // Use other constructor to create image representation, then throw the
  // temporary Image away.
  // This looks strange, but if you think about it, swap() is a great tool. -- jlnr
  Image(io->pixType(), io->sizeX(), io->sizeY(), io->channels()).swap(*this);
  
  try
  {  
    // Read image, will throw an exception on failure.
    pImage()->read(*io);
    // I've seen all the gi_ tools check for empty images, so I added this
    // check. -- jlnr
    if (sizeX() == 0 || sizeY() == 0)
      throw std::runtime_error("Image " + filename + " is empty");
  }
  catch (...)
  {
    delete pImage_;
    throw;
  }
}

Image::Image(const Image& rhs)
{
  pImage_ = rhs.pImage_->copyObject();
}

Image& Image::operator=(const Image& rhs)
{
  // Temporary Image & swap; same trick as above. -- jlnr
  if (this != &rhs)
    Image(rhs).swap(*this);
  return *this;
}

Image::~Image()
{
  delete pImage_;
}

void Image::swap(Image& other)
{
  std::swap(pImage_, other.pImage_);
}

ImageBase* Image::pImage() const {
  return pImage_;
}

const class std::type_info& Image::typeId() const {
  return pImage_->typeId();
}

int Image::sizeX() const {
  return pImage_->sizeX();
}

int Image::sizeY() const {
  return pImage_->sizeY();
}

int Image::noPixels() const {
  return pImage_->noPixels();
}
	
int Image::noChannels() const {
  return pImage_->noChannels();
}

Image Image::getChannel(int channel)
{
  Image result(typeId());
  pImage()->getChannel(*result.pImage(),channel);
  return result;
}

FileType Image::fileType() const {
  return pImage_->fileType();
}

void Image::setFileType(FileType t) {
  pImage_->setFileType(t);
}

bool Image::read(const char* filename) {
	try {
    Image(filename).swap(*this);
	}
	catch (...) {
    return false;
	}
  return true;
}

void Image::write(const char* filename, int channel) {
  pImage()->write(filename, channel);
}

double Image::getPixel(int x, int y, int channel) const
{
  return pImage_->getPixelAsDouble(x,y,channel);
}

void Image::setPixel(int x, int y, double val, int channel, bool clip) {
  return pImage_->setPixelToDouble(x, y, val, channel, clip);
}

void Image::fill(double value) {
  pImage_->fill(value);
}

void Image::fillRow(int row, int startX, int endX, double val, int channel)
{
  pImage_->fillRow(row, startX, endX, val, channel);
}

double Image::findMaxValue(int channel) const
{
  return pImage_->findMaxValue(channel);
}

double Image::findMinValue(int channel) const
{
  return pImage_->findMinValue(channel);
}

Image::ConstIterator Image::constBegin(int row, int channel) const {
  return ConstIterator(*this, channel, row * sizeX());
};

Image::ConstIterator Image::constEnd(int row, int channel) const {
  return constBegin(row + 1, channel);
};

Image::ConstIterator Image::constEnd() const {
  return constBegin(sizeY(), 0);
};

Image::Iterator Image::begin(int row, int channel)  {
  return Iterator(*this, channel, row * sizeX());
};

Image::Iterator Image::end(int row, int channel) {
  return begin(row + 1, channel);
};

Image::Iterator Image::end() {
  return begin(sizeY(), 0);
};

} // namespace Ga
