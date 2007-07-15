/***************************************************************************
                          GaImageT.h  -  geoAIDA template image class
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : Jürgen Bückner, Oliver Stahlhut, Martin Pahl
    email                : bueckner@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * $Source: /data/cvs/ga_image/ga_image/gaimage/gaimaget.h,v $
 * $Revision: 1.14 $
 * $Date: 2003/05/28 06:35:02 $
 * $Author: pahl $
 * $Locker:  $
 */

#ifndef __GA_IMAGET_H
#define __GA_IMAGET_H

// <cmath> and <climits> would be nicer; however, all the code only uses
// things in the global namespace. Assuming the new-style headers place
// their contents in the global namespace is not portable.
#include <math.h>
#include <limits.h>

#ifdef GCC29
#include <float.h>
#endif

extern "C" {
#include <pnm.h>
#include <pfm.h>
}
#define GA_MAX_CHANNELS 10
#include <assert.h>
#include "gadefines.h"
#include "gaimagebase.h"
#include "gaarray2dt.h"
#include <algorithm>

namespace Ga {

/** \class ImageT
    \brief defines an template class for operations between matrices and vectors
    and for handling geo-related images
    */

  template <class PixTyp>
  class ImageT : public ImageBase
  {
  public:
    typedef PixTyp* Iterator;
    typedef const PixTyp* ConstIterator;

    // Constructors + crap.
    ImageT(int x = 0, int y = 0, int noChannels=1);
    ImageT(const ImageT<PixTyp>& rval);
    virtual ~ImageT(void);
    ImageT<PixTyp>& operator= (const ImageT<PixTyp>& rval);
    virtual ImageBase* copyObject();
    
    // Drawing primitives.
    void fill(double value);
    virtual void fillRow(void *it, int startX, int endX, double val, int channel=0);
    virtual void partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height);
    
    // Metrics.
    virtual const class std::type_info& typeId() const;
    int noChannels() const;
    
    /** return pointer to the beginning of data
	usage: \code Iterator dp = M.begin(row); \endcode */
    Iterator begin(int row=0, int channel=0);
    ConstIterator constBegin(int row=0, int channel=0) const;
    void* beginVoid(int row=0, int channel=0);
    ConstIterator end(int row, int channel=0) const;
    ConstIterator end() const;
    ConstIterator endChannel(int channel) const;
    const void* endVoid(int row, int channel=0) const;
    const void* endVoid() const;
    const void* constBeginVoid(int row=0, int channel=0) const;
    /** retrieve the value of an matrix element
	usage: \code PixTyp x = A.GetElement( 3, 5 ); \endcode */
    PixTyp get(int x, int y, int channel=0, PixTyp neutral=0) const;
    double getFloat(int x, int y, int channel, double neutral=0) const;
    virtual double getFloat(const void *it) const;
    virtual void setFloat(void* it, double val);
    virtual void* nextCol(const void*& ptr, int offset) const;
    virtual void* nextCol(const void*& ptr) const;

    PixTyp& operator () (int x, int y, int channel=0);

    /** Assign value to matrix element, check for legal row and column index
	usage: \code  A.SetElement( 3, 4, 4444 );       \endcode
	\param x x of element (column)
	\param y y of element (row)
	\param val value to set element to */
    void set(int x, int y, PixTyp val, int channel=0, bool clip=false);
    void setFloat(int x, int y, double val, int channel=0, bool clip=false);
    /* set data without copying. Ther must be noChannels data-pointer in the ellipse */
    void setData(int x, int y, int noChannels, ...);
    void setData(int x, int y, void* initvalues);

  //------------------------------------ Tool functions -----------------------

  double findMaxValue(int& x, int& y, int channel=0);
  double findMinValue(int& x, int& y, int channel=0);
  int elemSize();
  void resize(int rx, int ry, int noChannels=1);

  /** set a pixel */
  void setInt(void *ptr, int val);
  void setInt(int x, int y, int val, int channel=0);

  /** set a pixel */
  int getInt(const void *ptr) const;
  int getInt(int x, int y, int channel=0) const;
  int getInt(int x, int y, int channel, int neutral) const;

  void getChannel(ImageBase& pic, int channel=0);

  void minValue(ImageBase &in, float value);
  void maxValue(ImageBase &in, float value);

  bool read(FILE *fp);
	bool read(const char *filename);
  bool write(FILE *fp, int channel=0, const char* comment=0);
	bool write(const char *filename, int channel=0, const char* comment=0);

  void swap(ImageT& other) {
    using std::swap;
    swap(imageType_, other.imageType_);
    swap(sizeX_, other.sizeX_);
    swap(sizeY_, other.sizeY_);
    swap(sizeY_, other.sizeY_);
    for (unsigned i = 0; i < GA_MAX_CHANNELS; ++i)
      swap(pChannel_[i], other.pChannel_[i]);
    swap(noChannels_, other.noChannels_);
  }

protected:
  /** initialization; common function for initialization; for internal use only
      usage: \code Initialize( x, y ) \endcode \endcode */
  void initialize( int x, int y, int noChannels );
  /** prepare assignments; Preparing an assignment by adjusting "this" */
  Array2DT<PixTyp> *pChannel_[GA_MAX_CHANNELS];
  int noChannels_;
};

} // namespace Ga

#include "gaimaget.hpp"

#endif        // __GA_IMAGET_H
