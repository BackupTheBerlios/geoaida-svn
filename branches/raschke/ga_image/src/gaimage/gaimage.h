/****************************************************************************
                          gaimage.h  -  GeoAIDA template image class
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

#ifndef __GAIMAGE_H
#define __GAIMAGE_H

#include <typeinfo>
#include "gadefines.h"
#include "gaimagebase.h"

namespace Ga {

class ImageBase;

/** \class Image
    \brief Class for handling arbitrarily large image files.
    
    The Image class provides a type-agnostic wrapper around any ImageT<> instance.
    */

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
  
  template<typename OtherImg>
  bool operator==(IteratorT<OtherImg, Pix> rhs) {
    return img == rhs.img && ch == rhs.ch && elem == rhs.elem;
  }

  template<typename OtherImg>
  bool operator!=(IteratorT<OtherImg, Pix> rhs) {
    return img != rhs.img || ch != rhs.ch || elem != rhs.elem;
  }
};

class Image 
{
  ImageBase* pImage_;

public:
  typedef IteratorT<Image, double> Iterator;
  typedef IteratorT<const Image, double> ConstIterator;

  // Create wih ImageT representation of the given type, and given metrics.
  explicit Image(const class std::type_info& t, int x = 0, int y = 0, int noChannels=1);
  // Load from file.
  explicit Image(const std::string& filename);

  Image(const Image& rhs);
  Image& operator=(const Image& rhs);
  ~Image();
  void swap(Image& other);
  
  // Access to the wrapped object.
  ImageBase* pImage() const;
  
  // Geometry
  const class std::type_info& typeId() const;
  int sizeX() const;
  int sizeY() const;
  int noPixels() const; 
  int noChannels() const;
  Image getChannel(int channel=0);

  // I/O
  IMGTYPE fileType() const;
  void setFileType(IMGTYPE t);
  bool read(const char* filename);
  void write(const char* filename, int channel=0);
	
  // Drawing primitives
  double getPixel(int x, int y, int channel = 0) const;
  void setPixel(int x, int y, double val, int channel = 0, bool clip = false);
  void fillRow(int row, int startX, int endX, double val, int channel=0, bool clip=false);
  void fill(double value);
  
  // Auslagern usw.
  /*o*/ double findMinValue(int channel=0) const;
  /*o*/ double findMaxValue(int channel=0) const;

  // Iterators
  Iterator begin(int row=0, int channel=0);
  ConstIterator constBegin(int row=0, int channel=0) const;
  ConstIterator end(int row, int channel=0) const;
  ConstIterator end() const;
};

#define ForTypeDo(PixTyp,function,args) \
  if (gaMatrixType==typeid(PixTyp)) {   \
    function<PixTyp>args;       \
  }
#define ForEachTypeDo(type_id,function,args) \
{ \
 const class std::type_info& gaMatrixType=type_id; \
 \
  ForTypeDo(bool, function, args)  \
  ForTypeDo(char, function, args)  \
  ForTypeDo(signed char, function, args)  \
  ForTypeDo(unsigned char, function, args)  \
  ForTypeDo(short, function, args)   \
  ForTypeDo(signed short, function, args)   \
  ForTypeDo(unsigned short, function, args) \
  ForTypeDo(int, function, args)     \
  ForTypeDo(signed int, function, args)     \
  ForTypeDo(unsigned int, function, args)   \
  ForTypeDo(float, function, args)          \
}

}

#endif
