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
    
    static int storageTypeFromTypeId(const std::type_info& id)
    {
      if (id == typeid(float))
        return PFM_FLOAT;
      else if (id == typeid(bool))
        return PFM_BIT;
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
    
    FILE* fp;
    FileType type;
    std::string comment_;
    int sizeX_, sizeY_;
    
  public:
    LibPFMImpl(FILE* fp, FileType fileType, int sizeX, int sizeY)
    : fp(fp), type(fileType), sizeX_(sizeX), sizeY_(sizeY)
    {
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
        std::copy(storageBuffer, storageBuffer + width * height, buffer);
        free(storageBuffer);
      }
      else
      {
      	float min, max;
        PFM3Byte* ppmbuffer = static_cast<PFM3Byte*>(pfm_readpfm_type(fp, &sizeX_, &sizeY_,
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
      
      // Update comment from static libpfm buffer.
      comment_ = pfm_comment_get() ? pfm_comment_get() : "";
    }
    
    template<typename Src>
    void replaceRect(int channel, int x, int y, int width, int height,
      const Src* buffer)
    {
      assert(buffer != 0);
      assert(x == 0 && y == 0 && width == sizeX() && height == sizeY());

      // Store comment so libpfm will use it.
      pfm_comment_set(comment_.c_str());
      
      if (fileType() != _PPM)
      {
        assert(channel == 0);
        fseek(fp, 0, SEEK_SET);
        ftruncate(fileno(fp), 0);
        pfm_writepfm_type(fp, buffer, sizeX(), sizeY(),
          *std::min_element(buffer, buffer + width * height),
          *std::max_element(buffer, buffer + width * height),
          storageTypeFromTypeId(typeid(Src)));
      }
      else
  		{
  		  // Writing PPMs is a bit awkward. Each time a channel is written, the other channels are read
  		  // and written again, unless the image is empty. If it is empty, unwritten channels are filled
  		  // with fake data.
  		  
  		  // This could be worked around by merging all channels into a single one; however, this raises
  		  // new questions (how would findMax work etc.)
        
        PFM3Byte *ppmbuffer;
  		  
        fseek(fp, 0, SEEK_END);
  		  if (ftell(fp) == 0)
  		  {
          ppmbuffer = (PFM3Byte*)malloc(sizeof ppmbuffer);
          ppmbuffer->r = (unsigned char*)malloc(width * height);
          ppmbuffer->g = (unsigned char*)malloc(width * height);
          ppmbuffer->b = (unsigned char*)malloc(width * height);
  		  }
  		  else
  		  {
          int w, h;
          float min, max;
          fseek(fp, 0, SEEK_SET);
  		    ppmbuffer = static_cast<PFM3Byte*>(pfm_readpfm_type(fp, &w, &h, &min, &max, PFM_3BYTE, 0));
          assert(w == width && h == height && "Overwriting PPM files of different size not handled!");
        }
        
        switch(channel)
        {
          case 0:
            std::copy(buffer, buffer + width * height, ppmbuffer->r);
            break;
          case 1:
            std::copy(buffer, buffer + width * height, ppmbuffer->g);
            break;
          case 2:
            std::copy(buffer, buffer + width * height, ppmbuffer->b);
            break;
          default:
            assert(!"Invalid channel for LibPFMImpl::replaceRect");
        }

        fseek(fp, 0, SEEK_SET);
        ftruncate(fileno(fp), 0);
        pfm_writepfm_type(fp, ppmbuffer, width, height, 1, -1, PFM_3BYTE);
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

    std::string comment() const
    {
      return comment_;
    }
    
    void setComment(const std::string& comment)
    {
      comment_ = comment;
    }
  };
}
