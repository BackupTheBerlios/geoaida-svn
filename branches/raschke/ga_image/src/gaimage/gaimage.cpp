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

namespace Ga {
  
// File-local helper functions.
namespace {
  const std::type_info& pixTypeOfImgType(IMGTYPE type)
  {
    switch (type)
    {
      case _PFM_FLOAT:  return typeid(float);
      case _PFM_UINT16: return typeid(unsigned short);
      case _PFM_SINT16: return typeid(signed short);
      case _PFM_UINT:   return typeid(unsigned);
      case _PFM_SINT:   return typeid(signed);
      case _PGM:        return typeid(unsigned char);
      case _PPM:        return typeid(unsigned char);
      case _PBM:        return typeid(bool);
      default:          throw std::logic_error("Unsupported image type");
    }
  }
  
  int channelsOfImgType(IMGTYPE type)
  {
    if (type == _PPM)
      return 3;
    return 1;
  }
}

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
  FILE *fp = fopen(filename.c_str(), "r");
  if (!fp)
    throw std::runtime_error("Cannot open file " + filename);
  try
  {  
    int cols,rows;
    IMGTYPE type=readImageType(fp,&cols,&rows);
    // Use other constructor to create image representation    
    Image(pixTypeOfImgType(type), cols, rows, channelsOfImgType(type)).swap(*this);
  	if (!pImage()->read(fp))
      throw std::runtime_error("Couldn't load image " + filename);
    if (isEmpty())
      throw std::runtime_error("Image " + filename + " is empty");
  }
  catch (...)
  {
    fclose(fp);
    throw;
  }
}

/** copy constructor: defining and initialize from a matrix
    usage: \code Image A = B; Image A = B + 2 * C; return A \endcode */
Image::Image(const Image& rhs)
{
  if (rhs.pImage_)
    pImage_ = rhs.pImage_->copyObject();
  else
    pImage_=0;
}

Image& Image::operator=(const Image& rhs)
{
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

const class std::type_info& Image::typeId() const {
  assert(pImage_!=0);
  return pImage_->typeId();
}

IMGTYPE Image::typeImage() const {
  assert(pImage_!=0);
  return pImage_->typeImage();
}

void Image::typeImage(IMGTYPE t) {
  assert(pImage_);
  pImage_->typeImage(t);
}

#undef GenClasses
#undef GenClassesIf

bool Image::isEmpty() const {
  if (pImage_==0) return true;
  if (sizeX()==0 || sizeY()==0) return true;
  return false;
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

IMGTYPE Image::readImageType(FILE *fp,int* cols, int* rows) {
    int pxmtype;
    IMGTYPE type=_UNKNOWN;
    int c, r;
    float minval,maxval;
    long pos=ftell(fp);
    if (pfm_readpfm_header(fp, &c, &r, &minval, &maxval, &pxmtype))
	type=(IMGTYPE)pxmtype;
    fseek(fp,pos,SEEK_SET);
    if (type==_UNKNOWN) {
	xelval max_x;
	pnm_readpnminit(fp, &c, &r, &max_x, &pxmtype);
	fseek(fp,pos,SEEK_SET);
	if (pxmtype == PBM_FORMAT || pxmtype == RPBM_FORMAT ||
	    pxmtype == PPM_FORMAT || pxmtype == RPPM_FORMAT ) {
	    switch(pxmtype) {
		case PBM_FORMAT:
		case RPBM_FORMAT:
		    type=_PBM;
		    break;
		case PGM_FORMAT:
		case RPGM_FORMAT:
		    type=_PGM;
		    break;
		case PPM_FORMAT:
		case RPPM_FORMAT:
		    type=_PPM;
	    }
  	}
    }
    if (cols) *cols=c;
    if (rows) *rows=r;
    return type;
}

void Image::write(const char* filename, int channel) {
  FILE *fp=fopen(filename,"w");
  assert(fp);	
  write(fp, channel);
  fclose(fp);
}

void Image::write(FILE *fp, int channel) {
  assert(pImage_);
  assert(fp);
  pImage()->write(fp, channel, 0);
}

void Image::set(int x, int y, double val, int channel, bool clip) {
  assert(pImage_);
  return pImage_->setFloat(x,y,val,channel,clip);
};


void Image::fillRow(int row, int startX, int endX, double val, int channel, bool clip)
{
  throw "NYI";
  /*assert(pImage_);
  assert(channel<noChannels());
  if (clip) {
    if ((row<0) || (row>=sizeY())) return;
    if (startX<=0) startX=0;
    if (endX>=sizeX()) endX=sizeX()-1;
  }
  pImage_->fillRow(begin(row,channel),startX,endX,val);*/
}

//------------------------------ Access of matrix data ----------------------
int Image::sizeX() const {
  if (pImage_) return pImage_->sizeX();
  else return 0;
}

int Image::sizeY() const {
  if (pImage_) return pImage_->sizeY();
  else return 0;
}

int Image::sizeImage() const {
  if (pImage_) return pImage_->sizeImage();
  return 0;
}
	
int Image::noChannels() const {
  if (pImage_) return pImage_->noChannels();
  else return 0;
}

double Image::getFloat(int x, int y, int channel) const
{
  assert(pImage_!=0);
  return pImage_->getFloat(x,y,channel,0);
}

double Image::findMaxValue(int& x, int& y, int channel)
{
  return pImage_->findMaxValue(x,y,channel);
}

double Image::findMaxValue(int channel)
{
  int x, y;
  return pImage_->findMaxValue(x,y,channel);
}

double Image::findMinValue(int& x, int& y, int channel)
{
  return pImage_->findMinValue(x,y,channel);
}

double Image::findMinValue(int channel)
{
  int x, y;
  return pImage_->findMinValue(x, y, channel);
}

void Image::fill(double value) {
  assert(pImage_!=0);
  pImage_->fill(value);
}

/** set a pixel */
void Image::set(int x, int y, int val, int channel) {
  assert(pImage_!=0);
  pImage()->setInt(x,y,val,channel);
}

/** set a pixel */
void Image::set(void *ptr, int val) {
  assert(pImage_!=0);
  pImage()->setInt(ptr,val);
}

/** get a pixel */
int Image::getInt(int x, int y, int channel) const {
  assert(pImage_!=0);
  return pImage()->getInt(x,y,channel);
}

/** get a pixel */
int Image::getInt(int x, int y, int channel, int neutral) const {
  assert(pImage_!=0);
  return pImage()->getInt(x,y,channel,neutral);
}

/** get a pixel */
int Image::getInt(const void *ptr) const {
  assert(pImage_!=0);
  return pImage()->getInt(ptr);
}

//------------------------------ Iterators ----------------------
void* Image::begin(int row, int channel)  {
  assert(pImage_);
  return pImage_->beginVoid(row,channel);
};

const void* Image::constBegin(int row, int channel) const {
  assert(pImage_);
  return pImage_->constBeginVoid(row,channel);
};

const void* Image::end(int row, int channel) const {
  assert(pImage_);
  return pImage_->endVoid(row,channel);
};


const void* Image::end() const {
  assert(pImage_);
  return pImage_->endVoid();
};


double Image::getFloat(const void *it) const {
  assert(pImage_);
  return pImage_->getFloat(it);
};


void Image::set(void* it, double val) {
  assert(pImage_);
  return pImage_->setFloat(it,val);
};


void Image::nextCol(const void*& ptr) const {
  assert(pImage_);
  pImage_->nextCol(ptr);
}

void Image::nextCol(void*& ptr) const {
  assert(pImage_);
  pImage_->nextCol((const void*&)ptr);
}

void Image::nextCol(const void*& ptr, int offset) const {
  assert(pImage_);
  pImage_->nextCol(ptr,offset);
}

void Image::nextCol(void*& ptr, int offset) const {
  assert(pImage_);
  pImage_->nextCol((const void*&)ptr,offset);
}

Image Image::getChannel(int channel)
{
  Image result(typeId());
  pImage()->getChannel(*result.pImage(),channel);
  return result;
}

ImageBase* Image::pImage() const {
  return pImage_;
}

} // namespace Ga


