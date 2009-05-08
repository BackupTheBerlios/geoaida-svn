/***************************************************************************
                          gaimaget.h  -  GeoAIDA template image impl.
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : J¸rgen B¸ckner, Oliver Stahlhut, Martin Pahl,
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

#include <assert.h>
#include "gaimagebase.h"
#include <algorithm>
#include <vector>
#include <deque>
#include <tr1/functional>

namespace Ga
{
  template<typename Pix, bool Mutable>
  class Iterator
  {
  public:
    typedef std::tr1::function<Iterator<Pix, Mutable>(unsigned LargeSize)> Relocator;
    
  private:
    BlockHandle* handle;

    LargeSize elem;
    // Offset into handle so that elem=0 <=> iterator at rangeBegin'th element on image.
    LargeSize offset;
    
    // Used for relocation to a different segment.
    LargeSize rangeBegin, rangeEnd;
    Relocator relocator;
    
  public:
    Iterator(BlockHandle* handle, LargeSize elem, LargeSize offset,
      LargeSize rangeBegin, LargeSize rangeEnd, const Relocator& relocator)
    : handle(handle), elem(elem), offset(offset),
        rangeBegin(rangeBegin), rangeEnd(rangeEnd), relocator(relocator)
    {
      // No valid handle given: This iterator can only be used for relocating.
      if (!handle)
        return;
      
      if (Mutable)
        handle->lockRW();
      else
        handle->lockR();
    }
    
    Iterator(const Iterator& other)
    {
      handle = other.handle;
      elem = other.elem;
      offset = other.offset;
      rangeBegin = other.rangeBegin;
      rangeEnd = other.rangeEnd;
      relocator = other.relocator;

      // No valid handle given: This iterator can only be used for relocating.
      if (!handle)
        return;
      
      if (Mutable)
        handle->lockRW();
      else
        handle->lockR();
    }
    
    Iterator& operator=(const Iterator& other)
    {
      Iterator(other).swap(*this);
      return *this;
    }
    
    ~Iterator()
    {
      if (handle)
        handle->unlock();
    }
    
    void swap(Iterator& other)
    {
      using std::swap;
      swap(handle, other.handle);
      swap(elem, other.elem);
      swap(offset, other.offset);
      swap(rangeBegin, other.rangeBegin);
      swap(rangeEnd, other.rangeEnd);
      swap(relocator, other.relocator);
    }

	
    Pix& operator*()
    {
      if (elem < rangeBegin || elem >= rangeEnd)
      {
        *this = relocator(elem);
        assert(elem >= rangeBegin);
        assert(elem < rangeEnd);
      }

      Pix* data = static_cast<Pix*>(handle->getData());
	  return data[elem - rangeBegin + offset];
    }
    
    Pix& operator[](LargeSize index) const
    {
      return *(*this + index);
    }
    
    Iterator& operator+=(LargeDiff offset)
    {
      elem += offset;
      return *this;
    }
    
    Iterator operator+(LargeDiff offset) const
    {
      Iterator result(*this);
      result.elem += offset;
      return result;
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
  
    Iterator& operator-=(LargeDiff offset)
    {
      elem -= offset;
      return *this;
    }
    
    Iterator operator-(LargeDiff offset) const
    {
      Iterator result(*this);
      result.elem -= offset;
      return result;
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
      return !(*this == other);
    }

	bool operator<(Iterator other) const
	{
	  return elem < other.elem;
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
  const int SEGMENT_SIZE = 256;
  const int TILE_CACHE_SIZE = 16;

  template <class PixTyp>
  class ImageT : public ImageBase
  {
  private:
    // Called by Image on ImageBase. private to show that this is not supposed to
    // be used when using ImageT directly.
    double getPixelAsDouble(int x, int y, int channel, double neutral) const;
    void setPixelToDouble(int x, int y, double val, int channel, bool clip);
	double getPixelAsDoubleFast(int x, int y, int channel, double neutral) const;

	BlockHandle *getBlockHandle(int segX, int segY, int channel) const;

    double fileMin_, fileMax_;
    int segSizeX_, segSizeY_;
    struct Channel
    {
      std::vector<BlockHandle> segments;
    };
    // mutable because ConstIterator needs a non-const BlockHandle to lock/unlock it.
    mutable std::vector<Channel> channels;

	mutable std::vector<std::deque<std::pair<unsigned int, PixTyp *> > > tileCache_;

    int segmentsX() const;
    int segmentsY() const;

    // Size of segment row/column 'seg'; -1 meaning that the usual size (the one from ImageIO or
    // the constructor) is used.
    // These functions can return less than the usual size for the last row or column!
    LargeSize segmentSizeX(int row = -1) const;
    LargeSize segmentSizeY(int col = -1) const;

    template<typename It> It iteratorForElem(unsigned channel, LargeSize elem) const;
    
  public:
    typedef Ga::Iterator<PixTyp, true> Iterator;
    typedef Ga::Iterator<const PixTyp, false> ConstIterator;

    ImageT(int sizeX, int sizeY, int noChannels=1);
    virtual ImageBase* copyObject();

    // Metrics.
    virtual const class std::type_info& typeId() const;
    int noChannels() const;
    double fileMin() const { return fileMin_; }
    double fileMax() const { return fileMax_; }
    void setFileMin(double fileMin) { fileMin_ = fileMin; }
    void setFileMax(double fileMax) { fileMax_ = fileMax; }
    
    // I/O.
	void preload();
	void read(ImageIOPtr io);
	void write(ImageIOPtr io);
	
    // Using declarations to avoid that our overrides hide the inherited overloads.
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

    double findMaxValue(int channel=0);
    double findMinValue(int channel=0);
    void resize(int rx, int ry, int noChannels=1);

    void setInt(void *ptr, int val);

    int getInt(const void *ptr) const;

    void getChannel(ImageBase& pic, int channel=0);
    void addChannels(ImageBase& pic);

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
