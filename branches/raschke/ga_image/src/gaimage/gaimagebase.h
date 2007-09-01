/***************************************************************************
                          gaimagebase.h  -  GeoAIDA template image class
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : J�rgen B�ckner, Oliver Stahlhut, Martin Pahl,
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
#include <cassert>
#include <iostream>
#include <string>
#include <climits>

// TODO: Change to new-style includes; involves work.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#ifndef NAN
#define NAN sqrt(-1)
#endif

namespace Ga {

template<typename Img, typename Pix>
struct IteratorT
{
  Img* img;
  unsigned ch, elem;
  
  class Proxy {
    Img& img;
    unsigned ch, elem;
    
  public:
    Proxy(Img& img, unsigned ch, unsigned elem) : img(img), ch(ch), elem(elem) {}
    operator Pix() const { return img.getPixel(elem % img.sizeX(), elem / img.sizeY(), ch); }
    Proxy& operator=(Pix val) { img.setPixel(elem % img.sizeX(), elem / img.sizeY(), val, ch); return *this; }
  };
  
  IteratorT() : img(0) {}
  explicit IteratorT(Img& img, unsigned ch, unsigned elem) : img(&img), ch(ch), elem(elem) {}
  IteratorT& operator++() { ++elem; return *this; }
  IteratorT operator++(int) { IteratorT old = *this; ++*this; return old; }
  Proxy operator*() const { return Proxy(*img, ch, elem); }
  
  bool operator==(const IteratorT& rhs) const {
    return img == rhs.img && ch == rhs.ch && elem == rhs.elem;
  }

  bool operator!=(const IteratorT& rhs) const {
    return !(*this == rhs);
  }
};

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
protected:
  IMGTYPE fileType_;
  int sizeX_, sizeY_;

  ImageBase();

public:
  virtual ImageBase* copyObject() = 0;
  virtual ~ImageBase() {}

  virtual const class std::type_info& typeId() const = 0;
  int sizeX() const { return sizeX_; }
  int sizeY() const { return sizeY_; }
  int noPixels() const { return sizeX_*sizeY_; }
  virtual int noChannels() const=0;
  virtual void getChannel(ImageBase& pic, int channel=0) = 0;
  virtual void resize(int rx, int ry, int noChannels=1)=0; // TODO: KILL!

  IMGTYPE fileType() const { return fileType_; }
	void setFileType(IMGTYPE t) { fileType_ = t;}
  bool read(const char* filename);
  bool write(const char *filename, int channel=0);
  virtual bool read(FILE *fp) = 0;
  virtual bool write(FILE *fp, int channel=0, const char* comment=0) = 0;

  // Drawing primitives
  virtual double getPixelAsDouble(int x, int y, int channel=0, double neutral=0) const=0;
  virtual void setPixelToDouble(int x, int y, double val, int channel=0, bool clip=false)=0;
  virtual void fillRow(int row, int startX, int endX, double val, int channel = 0) =0;
  virtual void fill(double value)=0;

  // -> Auslagern
  virtual double findMaxValue(int ch=0) =0;
  virtual double findMinValue(int ch=0) =0;
};

inline ImageBase::ImageBase()
{
  sizeX_=0;
  sizeY_=0;
  fileType_=_UNKNOWN;
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
