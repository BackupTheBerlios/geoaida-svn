/****************************************************************************
                          gaimageio.h - Image I/O implementation
                             -------------------
    begin                : 2008-06-04
    copyright            : (C) 2008 TNT, Uni Hannover
    authors              : Karsten Vogt
    email                : vogt@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tiffio.h"
#include "gaimageio.h"
#include "gaimage.h"
#include <algorithm>
#include <vector>
#include <stdexcept>

namespace Ga
{
	bool checkTIFF(const std::string &filename, int &sizeX, int &sizeY, FileType &storageType)
	{
		TIFF *tiff;
		
		// Open file
		if (!(tiff = TIFFOpen(filename.c_str(), "r")))
			return false;
		
		// Determine image dimensions
		TIFFGetFieldDefaulted(tiff, TIFFTAG_IMAGEWIDTH, &sizeX);
		TIFFGetFieldDefaulted(tiff, TIFFTAG_IMAGELENGTH, &sizeY);
		
		// Determine the sample-type
		unsigned short sampleFormat;
		unsigned short sampleBits;
		TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
		TIFFGetFieldDefaulted(tiff, TIFFTAG_BITSPERSAMPLE, &sampleBits);
		
		switch (sampleFormat)
		{
			case 1: // Unsigned int
				if (sampleBits == 32)
					storageType = _TIFF_UINT32;
				else if (sampleBits == 16)
					storageType = _TIFF_UINT16;
				else if (sampleBits == 8)
					storageType = _TIFF_UINT8;
				else
					throw std::logic_error("Unsupported Tiff sample format");
				break;
					
			case 2: // Signed int
				if (sampleBits == 32)
					storageType = _TIFF_SINT32;
				else if (sampleBits == 16)
					storageType = _TIFF_SINT16;
				else if (sampleBits == 8)
					storageType = _TIFF_SINT8;
				else
					throw std::logic_error("Unsupported Tiff sample format");
				break;
				
			case 3: // Floating point
				if (sampleBits == 64)
					storageType = _TIFF_DOUBLE;
				else if (sampleBits == 32)
					storageType = _TIFF_FLOAT;
				else
					throw std::logic_error("Unsupported Tiff sample format");
				break;
					
			default:
				throw std::logic_error("Unsupported Tiff sample format");
		}
		
		// Close file
		TIFFClose(tiff);
		
		return true;
	}
	
	class LibTIFFImpl
	{
		LibTIFFImpl(const LibTIFFImpl&);
		LibTIFFImpl& operator=(const LibTIFFImpl&);
		
		// Tiff storage organisation (Tiles or Strips?)
		enum TiffMode
		{
			TiffModeStrips,
			TiffModeTiles
		} tiffmode;
		
		static const std::type_info& typeIdFromFileType(FileType fileType)
		{
			switch (fileType)
			{
				case _TIFF_FLOAT:	return typeid(float);
				case _TIFF_DOUBLE:	return typeid(double);
				case _TIFF_SINT32:	return typeid(signed int);
				case _TIFF_UINT32:	return typeid(unsigned int);
				case _TIFF_SINT16:	return typeid(signed short);
				case _TIFF_UINT16:	return typeid(unsigned short);
				case _TIFF_SINT8:	return typeid(signed char);
				case _TIFF_UINT8:	return typeid(unsigned char);
				
				default:
					throw std::logic_error("Unsupported Tiff storage type");
			}
		}
		
		TIFF *tiff;
		FileType type;
		bool writeMode_;
		
		std::string comment_;
		float fileMin_, fileMax_;
		int sizeX_, sizeY_;
		int channels_;
		
		public:
			LibTIFFImpl(const std::string &filename, FileType fileType, int sizeX, int sizeY, int channels, bool writeMode = false)
			: type(fileType), sizeX_(sizeX), sizeY_(sizeY), channels_(channels), _writeMode_(writeMode)
			{
				unsigned short usValue = 0;
				
				if (!writeMode)
				{
					// Open tiff-image for reading
					tiff = TIFFOpen(filename.c_str(), "r");
					
					// Determine pixel-organisation (stripes or tiles?)
					tiffmode = TIFFIsTiled(tiff) ? TiffModeTiles : TiffModeStrips;
					
					// Read channel count
					TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &usValue);
					channels_ = usValue;
					
					// Read min and max pixel value
					TIFFGetFieldDefaulted(tiff, TIFFTAG_MINSAMPLEVALUE, &usValue);
					fileMin_ = usValue;
					TIFFGetFieldDefaulted(tiff, TIFFTAG_MAXSAMPLEVALUE, &usValue);
					fileMax_ = usValue;
				}
				else
				{
					// Open tiff-image for writing
					tiff = TIFFOpen(filename.c_str(), "w");
					
					// Set important tags
					TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, sizeX);
					TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, sizeY);
					TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, channels);
				}
			}
			
			virtual ~LibTIFFImpl()
			{
				TIFFClose(tiff);
			}
			
			template<typename Dest>
					void readRect(int channel, int x, int y, int width, int height, Dest* buffer)
			{
				//std::cout << std::endl;
				//std::cout << "channel = " << channel << "\nx = " << x << "\ny = " << y << "\nwidth = " << width << "\nheight = " << height << std::endl;
				//std::cout << "SegmentWidth = " << segmentSizeX() << "\nSegmentHeight = " << segmentSizeY() << std::endl;
				
				assert(writeMode_ == false);
				assert(width <= segmentSizeX() && height <= segmentSizeY());
				assert(x % segmentSizeX() == 0 && y % segmentSizeY() == 0);
				assert(channel < channels_);
				
				static int dataBufferSize = 0;
				static tdata_t dataBuffer = _TIFFmalloc(dataBufferSize);
				
				switch (tiffmode)
				{
					case TiffModeStrips:
					{
						// Reserve more memory if necessary
						tsize_t stripSize = TIFFStripSize(tiff);
						if (stripSize > dataBufferSize)
						{
							dataBufferSize = stripSize;
							dataBuffer = _TIFFrealloc(dataBuffer, dataBufferSize);
						}
						
						// Read strip
						tsize_t sizeRead = TIFFReadEncodedStrip(tiff, TIFFComputeStrip(tiff, y, 0), dataBuffer, stripSize);
						assert(sizeRead > 0);
						
						// Extract channel and write to destination buffer
						int pixelcount = width * height;
						Dest *destBufferPixel = buffer;
						Dest *stripBufferPixel = (Dest *)dataBuffer + channel;
						
						while (pixelcount--)
						{
							*destBufferPixel = *stripBufferPixel;
							destBufferPixel++;
							stripBufferPixel += channels_;
						}
					}
					break;
					
					case TiffModeTiles:
					{
						// Reserve more memory if necessary
						tsize_t tileSize = TIFFTileSize(tiff);
						if (tileSize > dataBufferSize)
						{
							dataBufferSize = tileSize;
							dataBuffer = _TIFFrealloc(dataBuffer, dataBufferSize);
						}
						
						// Read tile
						tsize_t sizeRead = TIFFReadTile(tiff, dataBuffer, x, y, 0, 0);
						assert(sizeRead > 0);
						
						// Extract channel and write to destination buffer
						int pixelcount = width * height;
						Dest *destBufferPixel = buffer;
						Dest *tileBufferPixel = (Dest *)dataBuffer + channel;
						
						while (pixelcount--)
						{
							*destBufferPixel = *tileBufferPixel;
							destBufferPixel++;
							tileBufferPixel += channels_;
						}
					}
					break;
				}
			}
			
			template<typename Src>
					void replaceRect(int channel, int x, int y, int width, int height, const Src* buffer)
			{
				assert(writeMode_ == true);
				assert(width <= segmentSizeX() && height <= segmentSizeY());
				assert(x % segmentSizeX() == 0 && y % segmentSizeY() == 0);
				assert(channel < channels_);
				
				static int dataBufferSize = 0;
				static tdata_t dataBuffer = _TIFFmalloc(dataBufferSize);
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
			
			int segmentSizeX() const
			{
				switch (tiffmode)
				{
					case TiffModeStrips:
					{
						long imageWidth = 0;
						TIFFGetFieldDefaulted(tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
						
						return imageWidth;
					}
					
					case TiffModeTiles:
					{
						unsigned int tileWidth = 0;
						TIFFGetFieldDefaulted(tiff, TIFFTAG_TILEWIDTH, &tileWidth);
						
						return tileWidth;
					}
				}
			}
			
			int segmentSizeY() const
			{
				switch (tiffmode)
				{
					case TiffModeStrips:
					{
						long rowsPerStrip = 0;
						TIFFGetFieldDefaulted(tiff, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip);
						
						return rowsPerStrip;
					}
					
					case TiffModeTiles:
					{
						unsigned int tileLength = 0;
						TIFFGetFieldDefaulted(tiff, TIFFTAG_TILELENGTH, &tileLength);
						
						return tileLength;
					}
				}
			}
			
			int channels() const
			{
				return channels_;
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
			
			double fileMin() const
			{
				return fileMin_;
			}
			
			double fileMax() const
			{
				return fileMax_;
			}
			
			void setFileMin(double fileMin)
			{
				fileMin_ = fileMin;
			}
			
			void setFileMax(double fileMax)
			{
				fileMax_ = fileMax;
			}
	};
}
