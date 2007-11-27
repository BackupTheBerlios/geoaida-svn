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

namespace Ga
{
  class LibNetPBMImpl
  {
    LibNetPBMImpl(const LibNetPBMImpl&);
    LibNetPBMImpl& operator=(const LibNetPBMImpl&);

    static const std::type_info& typeIdForFileType(FileType fileType)
    {
      switch (fileType)
      {
    	case _PBM:
        return typeid(bool);
      case _PGM:
    	case _PPM:	
        return typeid(unsigned char);
      default:
        throw std::logic_error("Unsupported file type");
      }
    }

    static int channelsForFileType(FileType fileType)
    {
      switch (fileType)
      {
    	case _PBM:
    	case _PGM:
        return 1;
    	case _PPM:	
        return 3;
      default:
        throw std::logic_error("Unsupported file type");
      }
    }

    Image fileContent;

  public:
    LibNetPBMImpl(FileType fileType, int sizeX, int sizeY)
    : fileContent(typeIdForFileType(fileType), sizeX, sizeY, channelsForFileType(fileType))
    {
      
    }
    
    template<typename Dest>
    void readRect(int channel, int x, int y, int width, int height,
      Dest* buffer)
    {
        
    }
      
    template<typename Src>
    void replaceRect(int channel, int x, int y, int width, int height,
      const Src* buffer)
    {
        
    }
  };
}
