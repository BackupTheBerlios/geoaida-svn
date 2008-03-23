/****************************************************************************
                          gaimageio_splitpfm.h - Image I/O implementation
                             -------------------
    begin                : 2008-03-23
    copyright            : (C) 2008 TNT, Uni Hannover
    authors              : Julian Raschke
    email                : raschke@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gaimage.h"

// Proof of concept implementation for segmented images. Built on top of the
// libpfm implementation with artificial splitting of the image.

namespace Ga
{
  class SplitImpl
  {
    SplitImpl(const LibNetPBMImpl&);
    SplitImpl& operator=(const LibNetPBMImpl&);
    
    mutable Image img;
    
  public:
    // Read contents of file.
    SplitImpl(const std::string& filename)
    : img(filename)
    {
    }
    
    template<typename Dest>
    void readRect(int channel, int x, int y, int width, int height,
      Dest* buffer)
    {
      for (int row = y; row < y + height; ++row)
      {
        Image::ConstIterator it = img.constBegin(row, channel);
        for (int col = x; col < x + width; ++col)
          *buffer++ = *it++;
      }
    }
    
    template<typename Src>
    void replaceRect(int channel, int x, int y, int width, int height,
      const Src* buffer)
    {
      for (int row = y; row < y + height; ++row)
      {
        Image::Iterator it = img.begin(row, channel);
        for (int col = x; col < x + width; ++col)
          *it++ = *buffer++;
      }
    }
    
    FileType fileType() const
    {
      return _SPLIT;
    }
    
    int sizeX() const
    {
      return img.sizeX();
    }
    
    int sizeY() const
    {
      return img.sizeY();
    }
    
    int segmentSizeX() const
    {
      return 13;
    }
    
    int segmentSizeY() const
    {
      return 11;
    }
    
    int channels() const
    {
      return img.noChannels();
    }
    
    const std::type_info& pixType() const
    {
      return typeid(bool);
    }
    
    std::string comment() const
    {
      return img.comment();
    }
    
    void setComment(const std::string& comment) const
    {
      img.setComment(comment);
    }
    
    double fileMin() const
    {
      return img.pImage()->fileMin();
    }
    
    double fileMax() const
    {
      return img.pImage()->fileMax();
    }
    
    void setFileMin(double min)
    {
      img.pImage()->setFileMin(min);
    }
    
    void setFileMax(double max)
    {
      img.pImage()->setFileMax(max);
    }
  };
}
