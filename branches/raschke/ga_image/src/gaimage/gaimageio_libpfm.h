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
#include <pfm.h>
}

#include "gaimageio.h"
#include "gaimage.h"
#include <algorithm>
#include <vector>

namespace Ga
{
  class LibPFMImpl
  {
    LibPFMImpl(const LibPFMImpl&);    
    LibPFMImpl& operator=(const LibPFMImpl&);

    static const std::type_info& typeIdFromFileType(FileType fileType)
    {
      switch (fileType)
      {
      case _PFM_FLOAT: return typeid(float);
      case _PFM_SINT: return typeid(signed int);
      case _PFM_UINT: return typeid(unsigned int);
      case _PFM_SINT16: return typeid(signed short);
      case _PFM_UINT16: return typeid(unsigned short);
      case _PGM: return typeid(unsigned char);
      case _PBM: return typeid(bool);
      case _PPM: return typeid(unsigned char);
      default:
        throw std::logic_error("Unsupported PFM storage type");
      }
    }
    
    static int channelsFromFileType(FileType fileType)
    {
      switch (fileType)
      {
      case _PFM_FLOAT:
      case _PFM_SINT: 
      case _PFM_UINT: 
      case _PFM_SINT16:
      case _PFM_UINT16:
      case _PGM:
      case _PBM: return 1;
      case _PPM: return 3;
      default:
        throw std::logic_error("Unsupported PFM storage type");
      }
    }

    FILE* fp;
    FileType type;
    Image fileContent;
    
  public:
    // Read contents of file.
    LibPFMImpl(FILE* fp, FileType fileType, int sizeX, int sizeY)
    : fp(fp), type(fileType),
      fileContent(typeIdFromFileType(fileType), sizeX, sizeY,
                  channelsFromFileType(fileType))
    {
      if (ftell(fp) == 0)
        // Empty file; just created; leave as is
        return;
        
      // Otherwise, header has been read, read rest of file
      
      switch (fileType)
      {
        case _PPM:
        {
        	float min, max;
          PFM3Byte *ppmbuffer =
            static_cast<PFM3Byte*>(pfm_readpfm_type(fp, &sizeX, &sizeY, &min, &max, PFM_3BYTE, 0));
          std::copy(ppmbuffer->r, ppmbuffer->r + sizeX * sizeY, fileContent.begin(0, 0));
          std::copy(ppmbuffer->g, ppmbuffer->g + sizeX * sizeY, fileContent.begin(0, 1));
          std::copy(ppmbuffer->b, ppmbuffer->b + sizeX * sizeY, fileContent.begin(0, 2));
          
          printf("Got PPM file (%dx%d)\n", sizeX, sizeY);
      	}
      	
      	default:
          throw std::runtime_error("Unsupported file type for reading");
      }

    }
    
    template<typename Dest>
    void readRect(int channel, int x, int y, int width, int height,
      Dest* buffer)
    {
      assert(x == 0 && y == 0 && width == sizeX() && height == sizeY());
      
      std::copy(fileContent.constBegin(0, channel), fileContent.constEnd(0, channel), buffer);
    }
      
    template<typename Src>
    void replaceRect(int channel, int x, int y, int width, int height,
      const Src* buffer)
    {
      assert(x == 0 && y == 0 && width == sizeX() && height == sizeY());
      
      std::copy(buffer, buffer + width * height, fileContent.begin(0, channel));
      fseek(fp, 0, SEEK_SET);
      ftruncate(fileno(fp), 0);
      
      switch (fileType())
      {
    		case _PPM:
    		{
          std::vector<unsigned char> r(width*height), g(width*height), b(width*height);
          std::copy(fileContent.begin(0, 0), fileContent.end(0, 0), r.begin());
          std::copy(fileContent.begin(0, 1), fileContent.end(0, 1), g.begin());
          std::copy(fileContent.begin(0, 2), fileContent.end(0, 2), b.begin());
    		  PFM3Byte ppmbuffer;
          ppmbuffer.r=&r[0];
     	 		ppmbuffer.g=&g[0];
    			ppmbuffer.b=&b[0];
      		pfm_writepfm_type(fp, &ppmbuffer, sizeX(),sizeY(),1,-1,PFM_3BYTE);
          break;
      	}
      	default:
          char buf[100];
          sprintf(buf, "Unsupported file type for writing: %d", fileType());
          throw std::runtime_error(buf);
      }
    }
    
    FileType fileType() const
    {
      return type;
    }
    
    int sizeX() const
    {
      return fileContent.sizeX();
    }
    
    int sizeY() const
    {
      return fileContent.sizeY();
    }
    
    int channels() const
    {
      return fileContent.noChannels();
    }
    
    const std::type_info& pixType() const
    {
      return fileContent.typeId();
    }
  };
}
