/****************************************************************************
                          gaimageio.h - Image I/O implementation
                             -------------------
    begin                : 2007-10-29
    copyright            : (C) 2007 TNT, Uni Hannover
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

extern "C" {
#include <pnm.h>
}

#include "gaimageio.h"
#include "gaimage.h"
#include <algorithm>
#include <vector>

// This degenerated implementation only supports PBM files anymore.

namespace Ga
{
  class LibNetPBMImpl
  {
    LibNetPBMImpl(const LibNetPBMImpl&);
    LibNetPBMImpl& operator=(const LibNetPBMImpl&);

    FILE* fp;
    int sizeX_, sizeY_;

  public:
    // Read contents of file.
    LibNetPBMImpl(FILE* fp, FileType fileType, int sizeX, int sizeY)
    : fp(fp), sizeX_(sizeX), sizeY_(sizeY)
    {
      assert(fileType == _PBM);
    }
    
    template<typename Dest>
    void readRect(int channel, int x, int y, int width, int height,
      Dest* buffer)
    {
      assert(x == 0 && y == 0 && width == sizeX() && height == sizeY());

      fseek(fp, 0, SEEK_SET);

   	  int format;
   	  int cols, rows;
    	pbm_readpbminit(fp, &cols, &rows, &format);

      assert(cols == width && rows == height && "Should be right from readpnminit");
      
      std::vector<bit> row(width);
      for (int y = 0; y < height; ++y)
      {
        pbm_readpbmrow(fp, &row[0], width, format);
        std::copy(row.begin(), row.end(), buffer + y * width);
      }
    }
    
    template<typename Src>
    void replaceRect(int channel, int x, int y, int width, int height,
      const Src* buffer)
    {
      assert(buffer != 0);
      assert(x == 0 && y == 0 && width == sizeX() && height == sizeY());
      
      assert(channel == 0);
      fseek(fp, 0, SEEK_SET);
      ftruncate(fileno(fp), 0);
      
      pbm_writepbminit(fp, width, height, 0);

      std::vector<bit> row(width);
      for (int y = 0; y < height; ++y)
      {
        std::copy(buffer + y * width, buffer + (y + 1) * width, row.begin());
        pbm_writepbmrow(fp, &row[0], width, 0);
      }
    }
    
    FileType fileType() const
    {
      return _PBM;
    }
    
    int sizeX() const
    {
      return sizeX_;
    }
    
    int sizeY() const
    {
      return sizeY_;
    }
    
    int channels() const
    {
      return 1;
    }
    
    const std::type_info& pixType() const
    {
      return typeid(bool);
    }
    
    std::string comment() const
    {
      return std::string();
    }
    
    void setComment(const std::string& comment) const
    {
      // TODO: Rather throw an exception here?
    }
  };
}
