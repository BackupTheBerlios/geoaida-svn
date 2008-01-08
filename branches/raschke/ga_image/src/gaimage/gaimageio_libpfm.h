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

    // TODO: Superfluous?
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
    
    static int storageTypeFromTypeId(const std::type_info& id)
    {
      if (id == typeid(float))
        return PFM_FLOAT;
      else if (id == typeid(signed int))
        return PFM_SINT;
      else if (id == typeid(unsigned int))
        return PFM_UINT;
      else if (id == typeid(signed short))
        return PFM_SINT16;
      else if (id == typeid(unsigned short))
        return PFM_UINT16;
      else if (id == typeid(unsigned char))
        return PFM_BYTE;
      else if (id == typeid(unsigned char))
        return PFM_3BYTE;
      else if (id == typeid(double))
        return PFM_DOUBLE;
      else
        throw std::logic_error("No storage type for typeId");
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
    int sizeX_, sizeY_;
    
  public:
    // Read contents of file.
    LibPFMImpl(FILE* fp, FileType fileType, int sizeX, int sizeY)
    : fp(fp), type(fileType), sizeX_(sizeX), sizeY_(sizeY)
    {
      fseek(fp, 0, SEEK_END);
      if (ftell(fp) == 0)
        // Empty file, just created; leave as is
        return;
    }
    
    template<typename Dest>
    void readRect(int channel, int x, int y, int width, int height,
      Dest* buffer)
    {
      assert(x == 0 && y == 0 && width == sizeX() && height == sizeY());

      fseek(fp, 0, SEEK_SET);

      if (fileType() != _PPM)
      {
        float min, max;
        
        Dest* storageBuffer = static_cast<Dest*>(pfm_readpfm_type(fp, &sizeX_, &sizeY_,
          &min, &max, storageTypeFromTypeId(typeid(Dest)), 0));
        
        free(storageBuffer);
      }
      else
      {
      	float min, max;
        PFM3Byte *ppmbuffer = static_cast<PFM3Byte*>(pfm_readpfm_type(fp, &sizeX_, &sizeY_,
          &min, &max, PFM_3BYTE, 0));
        
        switch(channel)
        {
          case 0:
            std::copy(ppmbuffer->r, ppmbuffer->r + sizeX_ * sizeY_, buffer);
            break;
          case 1:
            std::copy(ppmbuffer->g, ppmbuffer->g + sizeX_ * sizeY_, buffer);
            break;
          case 2:
            std::copy(ppmbuffer->b, ppmbuffer->b + sizeX_ * sizeY_, buffer);
            break;
          default:
            assert(!"Invalid channel for LibPFMImpl::readRect");
        }
        
        free(ppmbuffer->r);
        free(ppmbuffer->g);
        free(ppmbuffer->b);
        free(ppmbuffer);
      }
    }
      
    template<typename Src>
    void replaceRect(int channel, int x, int y, int width, int height,
      const Src* buffer)
    {
      assert(x == 0 && y == 0 && width == sizeX() && height == sizeY());
      
      fseek(fp, 0, SEEK_SET);
      ftruncate(fileno(fp), 0);
      
      if (fileType() != _PPM)
      {
        pfm_writepfm_type(fp, buffer, sizeX(), sizeY(), -10000, 10000, storageTypeFromTypeId(typeid(Src)));
      }
      else
  		{
        assert(!"replaceRect will not work until 3bytes are joined into one struct");
        //         std::vector<unsigned char> r(width*height), g(width*height), b(width*height);
        //         std::copy(fileContent.begin(0, 0), fileContent.begin(height, 0), r.begin());
        //         std::copy(fileContent.begin(0, 1), fileContent.begin(height, 1), g.begin());
        //         std::copy(fileContent.begin(0, 2), fileContent.begin(height, 2), b.begin());
        //        PFM3Byte ppmbuffer;
        //         ppmbuffer.r=&r[0];
        //          ppmbuffer.g=&g[0];
        // ppmbuffer.b=&b[0];
        //        pfm_writepfm_type(fp, &ppmbuffer, sizeX(), sizeY(), 1, -1, PFM_3BYTE);
    	}
    }
    
    FileType fileType() const
    {
      return type;
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
      return fileType() == _PPM ? 3 : 1;
    }
    
    const std::type_info& pixType() const
    {
      return typeIdFromFileType(fileType());
    }
  };
}
