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
    */

typedef void* Iterator;
typedef const void* ConstIterator;

class Image 
{
  ImageBase* pImage_;
  void init(const class std::type_info& t, int x, int y, int noChannels=1);

public:
  // Create without ImageT representation.
  // To be killed...
  Image();
  // Create with empty ImageT representation.
  explicit Image(const class std::type_info& t, int x = 0, int y = 0, int noChannels=1);
  // Clone other image's contents.
  Image(const Image& rhs);
  Image& operator=(const Image& rhs);
  ~Image();
  void swap(Image& other);
  ImageBase* pImage() const;
  
  bool isEmpty() const;

  // Geometry
  int sizeX() const;
  int sizeY() const;
  int sizeImage() const; 
  int noChannels() const;
  Image getChannel(int channel=0);

  const class std::type_info& typeId() const;
  IMGTYPE typeImage() const;
  void typeImage(IMGTYPE t);
  IMGTYPE readImageType(FILE *fp,int* cols, int* rows);
  bool read(const char* filename);
  bool read(FILE *fp);
  void write(const char* filename, int channel=0);
  void write(FILE *fp, int channnel=0);
	
  // Drawing primitives
  double getFloat(int x, int y, int channel = 0) const;
  void set(int x, int y, double val, int channel=0, bool clip=false);
  void fillRow(int row, int startX, int endX, double val, int channel=0, bool clip=false);
  void fill(double value);
  void set(int x, int y, int val, int channel=0);
  int getInt(int x, int y, int channel=0) const;
  int getInt(int x, int y, int channel, int neutral) const;
  
  // Auslagern usw.
  /*o*/ double findMinValue(int channel=0);
  /*o*/ double findMinValue(int& x, int& y, int channel=0);
  /*o*/ double findMaxValue(int channel=0);
  /*o*/ double findMaxValue(int& x, int& y, int channel=0);

  // -> Iterator
  void* begin(int row=0, int channel=0);
  const void* constBegin(int row=0, int channel=0) const;
  const void* end(int row, int channel=0) const;
  const void* end() const;
  double getFloat(const void *it) const;
  void set(void* it, double val);
  void nextCol(const void*& ptr) const;
  void nextCol(void*& ptr) const;
  void nextCol(const void*& ptr, int offset) const;
  void nextCol(void*& ptr, int offset) const;
  void set(void *ptr, int val);
  int getInt(const void *ptr) const;

protected:
  /** initialization; common function for initialization; for internal use only
      usage: \code Initialize( x, y ) \endcode \endcode */
  void initialize( int x, int y, int noChannels=1 );
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
