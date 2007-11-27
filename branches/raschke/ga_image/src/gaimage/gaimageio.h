/****************************************************************************
                          gaimageio.h - Image I/O interface
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

#ifndef __GAIMAGEIO_H
#define __GAIMAGEIO_H

#include <string>
#include <typeinfo>

namespace Ga
{
  enum FileType
  {
    _PFM_FLOAT,
    _PFM_SINT,
    _PFM_UINT,
    _PFM_SINT16,
    _PFM_UINT16,
		_PGM,
		_PBM,
		_PPM,	
		_UNKNOWN 
  };
  
  class ImageIO
  { 
  private:
    ImageIO(const ImageIO& other);
    ImageIO& operator=(const ImageIO& other);

  protected:
    ImageIO() {}

  public:
    static std::auto_ptr<ImageIO> create(const std::string& filename,
      FileType fileType, int sizeX, int sizeY, int channels);
    static std::auto_ptr<ImageIO> reopen(const std::string& filename);

    virtual ~ImageIO();
    
    virtual FileType fileType() const = 0;
    virtual int sizeX() const = 0;
    virtual int sizeY() const = 0;
    virtual int channels() const = 0;
    virtual const std::type_info& pixType() const = 0;

    #define DECLARE_IO_METHODS(Type) \
    virtual void readRect(int channel, int x, int y, int width, int height, \
      Type* buffer) = 0; \
    virtual void replaceRect(int channel, int x, int y, int width, int height, \
      const Type* buffer) = 0;
    
    DECLARE_IO_METHODS(bool)
    DECLARE_IO_METHODS(char)
    DECLARE_IO_METHODS(signed char)
    DECLARE_IO_METHODS(unsigned char)
    DECLARE_IO_METHODS(signed short)
    DECLARE_IO_METHODS(unsigned short)
    DECLARE_IO_METHODS(signed int)
    DECLARE_IO_METHODS(unsigned int)
    DECLARE_IO_METHODS(float)
    DECLARE_IO_METHODS(double)
    #undef DECLARE_IO_METHODS
  };
}

#endif
