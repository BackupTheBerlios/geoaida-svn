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

#include "gaimageio.h"

#include <typeinfo>
#include <cassert>
#include <iostream>
#include <string>
#include <climits>
#include <stdexcept>

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

template <class PixType> class ImageT;

class ImageBase
{
protected:
  FileType fileType_;
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

  FileType fileType() const { return fileType_; }
	void setFileType(FileType t) { fileType_ = t;}
  void read(const char* filename);
  void write(const char *filename, int channel=0);
  virtual void read(ImageIO&) = 0;
  virtual void write(ImageIO&, int channel=0, const char* comment=0) = 0;

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

inline void ImageBase::read(const char* filename)
{
  std::auto_ptr<ImageIO> io = ImageIO::reopen(filename);
  return read(*io);
}

inline void ImageBase::write(const char* filename, int channel)
{
  std::auto_ptr<ImageIO> io = ImageIO::create(filename, fileType(), sizeX(), sizeY(), noChannels());
  return write(*io, channel);
}

} // namespace Ga

#endif        // __GA_IMAGEBASE_H
