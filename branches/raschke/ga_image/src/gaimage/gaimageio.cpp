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

#include "gaimageio.h"
#include "gaimageio_libnetpbm.h"
#include "gaimageio_libpfm.h"
#include <stdexcept>

namespace Ga
{
  template<typename Impl>
  class ImageIOAdapter : public ImageIO
  {
    std::auto_ptr<Impl> impl;
    
    FileType fileType() const { return impl->fileType(); }
    int sizeX() const { return impl->sizeX(); }
    int sizeY() const { return impl->sizeY(); }
    int channels() const { return impl->channels(); }
    const std::type_info& pixType() const { return impl->pixType(); }
    
    #define IMPLEMENT_IO_METHODS(Type) \
    void readRect(int channel, int x, int y, int width, int height, \
      Type* buffer) { impl->readRect(channel, x, y, width, height, buffer); } \
    void replaceRect(int channel, int x, int y, int width, int height, \
      const Type* buffer) { impl->replaceRect(channel, x, y, width, height, buffer); }
    
    IMPLEMENT_IO_METHODS(bool)
    IMPLEMENT_IO_METHODS(char)
    IMPLEMENT_IO_METHODS(signed char)
    IMPLEMENT_IO_METHODS(unsigned char)
    IMPLEMENT_IO_METHODS(signed short)
    IMPLEMENT_IO_METHODS(unsigned short)
    IMPLEMENT_IO_METHODS(signed int)
    IMPLEMENT_IO_METHODS(unsigned int)
    IMPLEMENT_IO_METHODS(float)
    IMPLEMENT_IO_METHODS(double)
    #undef IMPLEMENT_IO_METHODS
    
  public:
    ImageIOAdapter(std::auto_ptr<Impl>& impl) : impl(impl) {}
  };
}

std::auto_ptr<Ga::ImageIO> Ga::ImageIO::create(const std::string& filename,
  FileType fileType, int sizeX, int sizeY, int channels)
{
  printf("ImageIO: creating %s\n", filename.c_str());
  fflush(0);

  // Future: For tiled images, create a directory representation.
  
  switch (fileType)
  {
  case _PFM_FLOAT:
  case _PFM_SINT:
  case _PFM_UINT:
  case _PFM_SINT16:
  case _PFM_UINT16:
  case _PGM:
  case _PPM:
  {
    std::auto_ptr<LibPFMImpl> impl(new LibPFMImpl(fopen(filename.c_str(), "wb"), fileType, sizeX, sizeY));
    return std::auto_ptr<ImageIO>(new ImageIOAdapter<LibPFMImpl>(impl));
  }
  
	case _PBM:
	{
    //std::auto_ptr<LibNetPBMImpl> impl(new LibNetPBMImpl(filename, filetype, sizeX, sizeY, channels));
    //return std::auto_ptr<ImageIO>(new ImageIOAdapter(impl));
	}
  
  default:
    throw std::logic_error("Invalid file type in ImageIO::create");
  }
}

std::auto_ptr<Ga::ImageIO> Ga::ImageIO::reopen(const std::string& filename)
{
  printf("ImageIO: reopening %s\n", filename.c_str());
  fflush(0);
  
  // Future: For directories, use tiled image implementation.
  
  FILE* fp = fopen(filename.c_str(), "rb+");
  
  // TODO: Decide by first two bytes?

  // Try libpfm.
  int sizeX, sizeY;
  float min, max;
  int pfmStorageType;
  if (pfm_readpfm_header(fp, &sizeX, &sizeY, &min, &max, &pfmStorageType))
  {
    std::auto_ptr<LibPFMImpl> impl(new LibPFMImpl(fp, static_cast<FileType>(pfmStorageType), sizeX, sizeY));
    return std::auto_ptr<ImageIO>(new ImageIOAdapter<LibPFMImpl>(impl));
  }
  fseek(fp, 0, SEEK_SET);
  
  // Try libnetpbm.
  xelval max_x;
  int pnmType;
  pnm_readpnminit(fp, &sizeX, &sizeY, &max_x, &pnmType);
  fseek(fp, 0, SEEK_SET);  
  if (pnmType == PBM_FORMAT || pnmType == RPBM_FORMAT ||
      pnmType == PPM_FORMAT || pnmType == RPBM_FORMAT)
  {
    //std::auto_ptr<LibNetPBMImpl> impl(new LibNetPBMImpl(fp));
    //return std::auto_ptr<ImageIO>(new ImageIOAdapter(impl));
  }
  
  throw std::runtime_error("Unknown image type in " + filename);
}

Ga::ImageIO::~ImageIO()
{
}
