/****************************************************************************
                          gaimageio.h - Image I/O interface
                             -------------------
    begin                : 2007-10-29
    copyright            : (C) 2007 TNT, Uni Hannover
    authors              : Julian Raschke, Karsten Vogt
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
#include <memory>

namespace Ga
{
  // FORMAT: Supported file types
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

    _TIFF_FLOAT,
    _TIFF_DOUBLE,
    _TIFF_SINT32,
    _TIFF_UINT32,
    _TIFF_SINT16,
    _TIFF_UINT16,
    _TIFF_SINT8,
    _TIFF_UINT8,

    _JPEG_UINT8,

    _UNKNOWN
  };
  
  // Abstract base class that provides an interface for initiating the read/write
  // process and reading and writing portions of an image.
  class ImageIO
  { 
  private:
    ImageIO(const ImageIO& other);            //< Copying forbidden
    ImageIO& operator=(const ImageIO& other); //< Assignment forbidden
    
  protected:
    ImageIO() {}

  public:
    /// Create an empty image file of the given dimensions.
    static std::auto_ptr<ImageIO> create(const std::string& filename,
      FileType fileType, int sizeX, int sizeY, int channels);
    /// Reopen a given file for reading and potentially replacing stuff.
    static std::auto_ptr<ImageIO> reopen(const std::string& filename);
    
    virtual std::string filename() const = 0;
    
    virtual ~ImageIO();
    
    virtual FileType fileType() const = 0;
    virtual int sizeX() const = 0;
    virtual int sizeY() const = 0;
    virtual int channels() const = 0;
    virtual const std::type_info& pixType() const = 0;
    
    virtual int segmentSizeX() const = 0;
    virtual int segmentSizeY() const = 0;

    // The comment is only available after reading the image, and should be
    // set before starting to write the image. This is an implication of
    // libpfm's design, but could be changed with some effort.
    // This is usually called only by ImageT<>::read/write, which handle this
    // correctly.
    // The same applies to fileMin/fileMax.
    
    virtual std::string comment() const = 0;
    virtual void setComment(const std::string& comment) = 0;
    
    /// Stored min value of the file, if supported by the format.
    virtual double fileMin() const = 0;
    /// Stored max value of the file, if supported by the format.
    virtual double fileMax() const = 0;
    /// Store min value of the file, if supported by the format.
    virtual void setFileMin(double fileMin) = 0;
    /// Store max value of the file, if supported by the format.
    virtual void setFileMax(double fileMax) = 0;
    
    // These methods must be virtual; since virtual templates aren't allowed, a
    // macro is used to generate them.
    
    #define DECLARE_IO_METHODS(Type) \
    virtual void readRect(int channel, int x, int y, int width, int height, \
      Type* buffer) = 0; \
    virtual void replaceRect(int channel, int x, int y, int width, int height, \
      const Type* buffer) = 0;
    
    // FORMAT: Supported pixel types
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
