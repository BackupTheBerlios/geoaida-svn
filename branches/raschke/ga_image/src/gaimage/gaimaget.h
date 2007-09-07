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

#include <assert.h>
#include "gadefines.h"
#include "gaimagebase.h"
#include "gaarray2dt.h"
#include "gacache.h"
#include <algorithm>
#include <vector>

namespace Ga {

/** \class ImageT
    \brief TODO
    */
  
  template<typename Pix, bool Mutable>
  class Iterator
  {
    BlockHandle* handle;
    unsigned elem;
    
  public:
    Iterator(BlockHandle& handle, unsigned elem)
    : handle(&handle), elem(elem)
    {
      if (Mutable)
        handle.lockRW();
      else
        handle.lockR();
    }
    
    Iterator(const Iterator& other)
    {
      handle = other.handle;
      if (Mutable)
        handle->lockRW();
      else
        handle->lockR();
      elem = other.elem;
    }
    
    Iterator& operator=(const Iterator& other)
    {
      Iterator(other).swap(*this);
      return *this;
    }
    
    ~Iterator()
    {
      handle->unlock();
    }
    
    void swap(Iterator& other)
    {
      std::swap(handle, other.handle);
      std::swap(elem, other.elem);
    }
    
    Pix& operator*() const
    {
      return (*this)[0];
    }
    
    Pix& operator[](std::size_t index) const
    {
      Pix* data = static_cast<Pix*>(handle->getData());
      return data[elem + index];
    }
    
    Iterator& operator+=(std::ptrdiff_t offset)
    {
      elem += offset;
      return *this;
    }
    
    Iterator operator+(std::ptrdiff_t offset) const
    {
      return Iterator(*this) += offset;
    }
    
    Iterator& operator++()
    {
      ++elem;
      return *this;
    }
    
    Iterator operator++(int)
    {
      Iterator temp(*this);
      ++*this;
      return temp;
    }
  
    Iterator& operator-=(std::ptrdiff_t offset)
    {
      elem -= offset;
      return *this;
    }
    
    Iterator operator-(std::ptrdiff_t offset) const
    {
      return Iterator(*this) -= offset;
    }
    
    Iterator& operator--()
    {
      --elem;
      return *this;
    }
    
    Iterator operator--(int)
    {
      Iterator temp(*this);
      --*this;
      return temp;
    }
    
    std::ptrdiff_t operator-(Iterator other) const
    {
      return elem - other.elem;
    }
    
    bool operator==(Iterator other) const
    {
      return elem == other.elem;
    }
    
    bool operator!=(Iterator other) const
    {
      return elem != other.elem;
    }
  };
}
 
namespace std
{ 
  template<typename Pix, bool Mutable>
  struct iterator_traits<Ga::Iterator<Pix, Mutable> > : std::iterator_traits<Pix*>
  {
  };
}

namespace Ga
{
  template <class PixTyp>
  class ImageT : public ImageBase
  {
  private:
    // To be called from Image. private to show that this is not supposed to be
    // used when using ImageT directly.
    double getPixelAsDouble(int x, int y, int channel, double neutral) const;
    void setPixelToDouble(int x, int y, double val, int channel, bool clip);
    // mutable: ConstIterator needs a non-const BlockHandle to lock/unlock it.
    mutable std::vector<BlockHandle> channels;
    
  public:
    //typedef IteratorT<ImageT<PixTyp>, PixTyp> Iterator;
    //typedef IteratorT<const ImageT<PixTyp>, PixTyp> ConstIterator;
    typedef Ga::Iterator<PixTyp, true> Iterator;
    typedef Ga::Iterator<const PixTyp, false> ConstIterator;

    ImageT(int x, int y, int noChannels=1);
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
    virtual void fillRow(int row, int startX, int endX, double val, int channel = 0);
    virtual void partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height);
    
    Iterator begin(int row=0, int channel=0);
    Iterator end(int row, int channel=0);
    Iterator end();
    Iterator endChannel(int channel);

    ConstIterator constBegin(int row=0, int channel=0) const;
    ConstIterator constEnd(int row, int channel=0) const;
    ConstIterator constEnd() const;
    ConstIterator constEndChannel(int channel) const;

    PixTyp& operator() (int x, int y, int channel=0);

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

  void swap(ImageT& other) {
    using std::swap;
    swap(fileType_, other.fileType_);
    swap(sizeX_, other.sizeX_);
    swap(sizeY_, other.sizeY_);
    swap(channels, other.channels);
  }
};

} // namespace Ga

#include "gaimaget.hpp"

#endif        // __GA_IMAGET_H
