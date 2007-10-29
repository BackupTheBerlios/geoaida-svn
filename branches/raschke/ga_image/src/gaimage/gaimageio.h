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

#include <string>
#include <typeinfo>
#include "gafileformat.h"

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
  public:
    static std::auto_ptr<ImageIO> create(const std::string& filename,
      FileType filetype, int sizeX, int sizeY, int channels);
    static std::auto_ptr<ImageIO> reopen(const std::string& filename);

    virtual ~ImageIO();
    
    virtual FileType fileType() const = 0;
    virtual int sizeX() const = 0;
    virtual int sizeY() const = 0;
    virtual int channels() const = 0;
    virtual const std::type_info& pixType() const = 0;
    
  private:
    ImageIO(const ImageIO& other);
    ImageIO& operator=(const ImageIO& other);

  protected:
    virtual void readRect(int channel, int x, int y, int width, int height,
      bool* buffer) = 0;                                                   
    virtual void readRect(int channel, int x, int y, int width, int height,
      char* buffer) = 0;                                                   
    virtual void readRect(int channel, int x, int y, int width, int height,
      signed char* buffer) = 0;                                            
    virtual void readRect(int channel, int x, int y, int width, int height,
      unsigned char* buffer) = 0;                                          
    virtual void readRect(int channel, int x, int y, int width, int height,
      signed int* buffer) = 0;                                             
    virtual void readRect(int channel, int x, int y, int width, int height,
      unsigned int* buffer) = 0;                                           
    virtual void readRect(int channel, int x, int y, int width, int height,
      float* buffer) = 0;                                                  
    virtual void readRect(int channel, int x, int y, int width, int height,
      double* buffer) = 0;

    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const bool* buffer) = 0;                                                
    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const char* buffer) = 0;                                                
    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const signed char* buffer) = 0;                                         
    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const unsigned char* buffer) = 0;                                       
    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const signed int* buffer) = 0;                                          
    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const unsigned int* buffer) = 0;                                        
    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const float* buffer) = 0;                                               
    virtual void replaceRect(int channel, int x, int y, int width, int height,
      const double* buffer) = 0;
  };
  
  template<typename Impl>
  class ImageIOAdapter
  {
    Impl* impl;
    
  protected:
    void readRect(int channel, int x, int y, int width, int height,
      bool* buffer) { impl->readRect(channel, x, y, width, height, buffer); }
    void readRect(int channel, int x, int y, int width, int height,
      char* buffer) { impl->readRect(channel, x, y, width, height, buffer); }
    // ...
    
  public:
    ImageIOAdapter(FILE* fp)
    : impl(new Impl(fp))
    {
    }
    
    ~ImageIOAdapter()
    {
      delete impl;
    }
  };
}
