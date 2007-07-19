/***************************************************************************
                          gaimaget.h  -  GeoAIDA template image impl.
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : Jürgen Bückner, Oliver Stahlhut, Martin Pahl,
                           Julian Raschke
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

#ifndef __GA_IMAGET_H
#define __GA_IMAGET_H

// <cmath> and <climits> would be nicer; however, all the code only uses
// things in the global namespace. Assuming the new-style headers place
// their contents in the global namespace is not portable.
#include <math.h>
#include <limits.h>

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
    \brief TODO
    */

  template <class PixTyp>
  class ImageT : public ImageBase
  {
  private:
    // To be called from Image. Private to show that this is not supposed to be
    // used when using ImageT directly.
    double getPixelAsDouble(int x, int y, int channel, double neutral) const;
    void setPixelToDouble(int x, int y, double val, int channel, bool clip);
    
  protected:
    /** initialization; common function for initialization; for internal use only
        usage: \code Initialize( x, y ) \endcode \endcode */
    void initialize( int x, int y, int noChannels );
    /** prepare assignments; Preparing an assignment by adjusting "this" */
    Array2DT<PixTyp> *pChannel_[GA_MAX_CHANNELS];
    int noChannels_;

  public:
    typedef PixTyp* Iterator;
    typedef const PixTyp* ConstIterator;

    ImageT(int x, int y, int noChannels=1);
    ImageT(const ImageT<PixTyp>& rval);
    virtual ~ImageT(void);
    ImageT<PixTyp>& operator= (const ImageT<PixTyp>& rval);
    virtual ImageBase* copyObject();

    // Metrics.
    virtual const class std::type_info& typeId() const;
    int noChannels() const;
    
    // I/O.
    bool read(FILE *fp);
    bool write(FILE *fp, int channel=0, const char* comment=0);
    using ImageBase::read;
    using ImageBase::write;

    // Drawing primitives.
    PixTyp getPixel(int x, int y, int channel=0, PixTyp neutral=0) const;
    void setPixel(int x, int y, PixTyp val, int channel=0, bool clip=false);
    void fill(double value);
    virtual void fillRow(void *it, int startX, int endX, double val, int channel=0);
    virtual void partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height);
    
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
    virtual void setFloat(void* it, double val);
    virtual void* nextCol(const void*& ptr, int offset) const;
    virtual void* nextCol(const void*& ptr) const;

    PixTyp& operator () (int x, int y, int channel=0);

    /* set data without copying. Ther must be noChannels data-pointer in the ellipse */
    void setData(int x, int y, int noChannels, ...);
    void setData(int x, int y, void* initvalues);

  //------------------------------------ Tool functions -----------------------

  double findMaxValue(int channel=0);
  double findMinValue(int channel=0);
  void resize(int rx, int ry, int noChannels=1);

  void setInt(void *ptr, int val);

  int getInt(const void *ptr) const;

  void getChannel(ImageBase& pic, int channel=0);

  void minValue(ImageBase &in, float value);
  void maxValue(ImageBase &in, float value);

  void swap(ImageT& other) {
    using std::swap;
    swap(fileType_, other.fileType_);
    swap(sizeX_, other.sizeX_);
    swap(sizeY_, other.sizeY_);
    swap(sizeY_, other.sizeY_);
    for (unsigned i = 0; i < GA_MAX_CHANNELS; ++i)
      swap(pChannel_[i], other.pChannel_[i]);
    swap(noChannels_, other.noChannels_);
  }
};

} // namespace Ga

#include "gaimaget.hpp"

#endif        // __GA_IMAGET_H
