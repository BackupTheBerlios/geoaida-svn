/***************************************************************************
                          gaimagebase.h  -  GeoAIDA template image class
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : Jürgen Bückner, Oliver Stahlhut, Martin Pahl,
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

#ifndef __GAIMAGEBASE_H
#define __GAIMAGEBASE_H

#include <typeinfo>
#include "gadefines.h"
#include <assert.h>

namespace Ga {

enum IMGTYPE {
    _PFM_FLOAT=0,
    _PFM_SINT,
    _PFM_UINT,
    _PFM_SINT16,
    _PFM_UINT16,
		_PGM,
		_PBM,
		_PPM,	
		_UNKNOWN
};

template <class PixType> class ImageT;

class ImageBase
{
public:
  ImageBase();
  virtual ~ImageBase() {}
  virtual ImageBase* copyObject()=0;

  int sizeX() const { return sizeX_; }
  int sizeY() const { return sizeY_; }
  int sizeImage() const { return nSize_; }
  virtual int noChannels() const=0;

  // Hairy, kill
  ImageBase(const ImageBase& rval);
	IMGTYPE typeImage() const { return imageType_; }
	void typeImage(IMGTYPE t) { imageType_ = t;}
  virtual const class std::type_info& typeId() const =0;

  virtual double matrixMax(int& x, int& y, int ch=0) =0;
  virtual double matrixMin(int& x, int& y, int ch=0) =0;

  // Iterators
  virtual void* beginVoid(int row=0, int channel=0)=0;
  virtual const void* constBeginVoid(int row=0, int channel=0) const =0;
  virtual const void* endVoid(int row, int channel=0) const =0;
  virtual const void* endVoid() const =0;
  virtual double getFloat(const void *it) const=0;
  virtual void setFloat(void* it, double val)=0;
  virtual void* nextCol(const void*& ptr) const=0;
  virtual void* nextCol(const void*& ptr, int offset) const=0;
  virtual int getInt(const void *ptr) const {return 0;}
  virtual void setInt(void *ptr, int val) {return;}
  
  virtual double getFloat(int x, int y, int channel=0, double neutral=0) const=0;
  virtual void setFloat(int x, int y, double val, int channel=0, bool clip=false)=0;
  virtual void fillRow(void *it, int startX, int endX, double val, int channel=0) =0;
  virtual void setInt(int x, int y, int val, int channel=0) {return;}
  virtual int getInt(int x, int y, int channel=0) const {return 0;}
  virtual int getInt(int x, int y, int channel, int neutral) const {return 0;}

  virtual void alloc( int x, int y, int noChannels )=0;
  virtual void initialize( int x, int y, int noChannels )=0;
  virtual void deinitialize(void)=0;
  virtual void resize(int rx, int ry, int noChannels=1)=0;
  virtual void fill(double value)=0;
  
  virtual void getChannel(ImageBase& pic, int channel=0)=0;
  virtual bool read(FILE *fp)=0;
  virtual bool write(FILE *fp, int channel=0, const char* comment=0)=0;
  bool write(const char *filename, int channel=0);
  bool read(const char* filename);

protected:
	IMGTYPE imageType_;	
  int sizeX_, sizeY_;
  size_t nSize_;
};

inline ImageBase::ImageBase()
{
  sizeX_=0;
  sizeY_=0;
  nSize_=0;
  imageType_=_UNKNOWN;
}

inline ImageBase::ImageBase(const ImageBase& rval)
{
  sizeX_=rval.sizeX_;
  sizeY_=rval.sizeY_;
  nSize_=rval.nSize_;
  imageType_=rval.imageType_;
}

inline bool ImageBase::write(const char* filename, int channel)
{
  FILE *fp;
  fp=fopen(filename,"w");
  if (!fp) return false;
  return write(fp,channel);
}

inline bool ImageBase::read(const char* filename)
{
  FILE *fp;
  fp=fopen(filename,"r");
  if (!fp) return false;
  return read(fp);
}
} // namespace Ga

#endif        // __GA_IMAGEBASE_H
