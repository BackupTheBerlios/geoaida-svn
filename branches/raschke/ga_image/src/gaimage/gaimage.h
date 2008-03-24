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
#include "gaimagebase.h"

namespace Ga {

class ImageBase;

// Simple Forward-Iterator that uses getPixel and setPixel to access the contents of its Image.
// (Just as slow as calling getPixel/setPixel yourself, so you may want to avoid it.)
// (Maybe it should be removed. It's a lot of code for something that is used infrequently.)
template<typename Img>
struct SimpleIterator
{
  Img* img;
  unsigned ch, elem;
  
  class Proxy {
    Img& img;
    unsigned ch, elem;
    
  public:
    Proxy(Img& img, unsigned ch, unsigned elem) : img(img), ch(ch), elem(elem) {}
    operator double() const { return img.getPixel(elem % img.sizeX(), elem / img.sizeX(), ch); }
    Proxy& operator=(double val) { img.setPixel(elem % img.sizeX(), elem / img.sizeX(), val, ch); return *this; }
  };
  
  explicit SimpleIterator(Img& img, unsigned ch, unsigned elem) : img(&img), ch(ch), elem(elem) {}
  SimpleIterator& operator++() { ++elem; return *this; }
  SimpleIterator operator++(int) { SimpleIterator old = *this; ++*this; return old; }
  Proxy operator*() const { return Proxy(*img, ch, elem); }
  std::ptrdiff_t operator-(const SimpleIterator& other) const { return elem - other.elem; }
  
  bool operator==(const SimpleIterator& rhs) const {
    return img == rhs.img && ch == rhs.ch && elem == rhs.elem;
  }

  bool operator!=(const SimpleIterator& rhs) const {
    return !(*this == rhs);
  }
  
  typedef std::input_iterator_tag iterator_category;
  typedef double value_type;
  typedef std::ptrdiff_t difference_type;
  typedef Proxy* pointer;
  typedef Proxy reference;
};

class Image 
{
  ImageBase* pImage_;

public:
  typedef SimpleIterator<Image> Iterator;
  typedef SimpleIterator<const Image> ConstIterator;
  
  // Create wih ImageT representation of the given type, and given metrics.
  explicit Image(const class std::type_info& t, int x = 0, int y = 0,
    int noChannels=1, int segSizeX=0, int segSizeY=0);
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

  // Comments (most notable used to implement associated geoposition data)
  std::string comment() const;
  void setComment(const std::string& comment);

  // I/O
  FileType fileType() const;
  void setFileType(FileType t);
  bool read(const char* filename);
  void write(const char* filename, int channel=0);
	
  // Drawing primitives
  double getPixel(int x, int y, int channel = 0) const;
  void setPixel(int x, int y, double val, int channel = 0, bool clip = false);
  void fillRow(int row, int startX, int endX, double val, int channel=0);
  void fill(double value);
  
  // TODO: Move to algorithms; needn't be member functions
  double findMinValue(int channel=0) const;
  double findMaxValue(int channel=0) const;
  
  // Iterators
  // TODO: replace by channelBegin/rowBegin (+const*, +*End)
  // The current iterators make it easy to iterate over channel boundaries!
  ConstIterator constBegin(int row=0, int channel=0) const;
  ConstIterator constEnd(int row, int channel=0) const;
  ConstIterator constEnd() const;
  Iterator begin(int row=0, int channel=0);
  Iterator end(int row, int channel=0);
  Iterator end();
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
