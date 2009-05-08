/****************************************************************************
                          gaimageio_libtiff.h -
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
#include <map>
#include <stdexcept>

namespace Ga
{
	bool checkTIFF(const std::string &filename, int &sizeX, int &sizeY, int &channels, int &segSizeX, int &segSizeY, FileType &storageType)
	{
		TIFF *tiff;
		
		// Open file
		if (!(tiff = TIFFOpen(filename.c_str(), "r")))
			return false;
		
		// Determine image dimensions
		sizeX = 0;
		sizeY = 0;
		TIFFGetFieldDefaulted(tiff, TIFFTAG_IMAGEWIDTH, &sizeX);
		TIFFGetFieldDefaulted(tiff, TIFFTAG_IMAGELENGTH, &sizeY);
		
		// Determine the sample-type
		unsigned short sampleFormat = 0;
		unsigned short sampleBits = 0;
		channels = 0;
		
		TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
		TIFFGetFieldDefaulted(tiff, TIFFTAG_BITSPERSAMPLE, &sampleBits);
		TIFFGetFieldDefaulted(tiff, TIFFTAG_SAMPLESPERPIXEL, &channels);
		
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
		
		// Determine segment sizes
		segSizeX = 0;
		segSizeY = 0;
					
		if (TIFFIsTiled(tiff))	// Tiles
		{
			TIFFGetFieldDefaulted(tiff, TIFFTAG_TILEWIDTH, &segSizeX);
			TIFFGetFieldDefaulted(tiff, TIFFTAG_TILELENGTH, &segSizeY);
		}
		else					// Strips
		{
			TIFFGetFieldDefaulted(tiff, TIFFTAG_IMAGEWIDTH, &segSizeX);
			TIFFGetFieldDefaulted(tiff, TIFFTAG_ROWSPERSTRIP, &segSizeY);
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
		int segSizeX_, segSizeY_;
		int channels_;
		
		public:
			LibTIFFImpl(const std::string &filename, FileType fileType, int sizeX, int sizeY, int channels = 1, int segSizeX = 64, int segSizeY = 64, bool writeMode = false)
			: type(fileType), sizeX_(sizeX), sizeY_(sizeY), channels_(channels), segSizeX_(segSizeX), segSizeY_(segSizeY), writeMode_(writeMode)
			{
				unsigned short usValue = 0;
				
				if (!writeMode)
				{
					// Open tiff-image for reading
					tiff = TIFFOpen(filename.c_str(), "rM");
					
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
					
					// Always tiled
					tiffmode = TiffModeTiles;
					
					// Set important tags
					TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
					TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, (((fileType == _TIFF_UINT8) && ((channels == 3) || (channels == 4))) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK));
					TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, sizeX);
					TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, sizeY);
					TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, channels);
					
					// Set BitsPerSample, MinMaxValues
					short bpp = 0;
					short sampleformat = 0;
					
					switch (fileType)
					{
						case _TIFF_UINT8:
						{
							sampleformat = 1;
							bpp = 8;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							unsigned char *min = new unsigned char[channels];
							unsigned char *max = new unsigned char[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = std::numeric_limits<unsigned char>::min();
								max[i] = std::numeric_limits<unsigned char>::max();
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						case _TIFF_SINT8:
						{
							sampleformat = 2;
							bpp = 8;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							signed char *min = new signed char[channels];
							signed char *max = new signed char[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = std::numeric_limits<signed char>::min();
								max[i] = std::numeric_limits<signed char>::max();
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						case _TIFF_UINT16:
						{
							sampleformat = 1;
							bpp = 16;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							unsigned short *min = new unsigned short[channels];
							unsigned short *max = new unsigned short[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = std::numeric_limits<unsigned short>::min();
								max[i] = std::numeric_limits<unsigned short>::max();
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						case _TIFF_SINT16:
						{
							sampleformat = 2;
							bpp = 16;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							signed short *min = new signed short[channels];
							signed short *max = new signed short[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = std::numeric_limits<signed short>::min();
								max[i] = std::numeric_limits<signed short>::max();
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						case _TIFF_UINT32:
						{
							sampleformat = 1;
							bpp = 32;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							unsigned int *min = new unsigned int[channels];
							unsigned int *max = new unsigned int[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = std::numeric_limits<unsigned int>::min();
								max[i] = std::numeric_limits<unsigned int>::max();
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						case _TIFF_SINT32:
						{
							sampleformat = 2;
							bpp = 32;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							signed int *min = new signed int[channels];
							signed int *max = new signed int[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = std::numeric_limits<signed int>::min();
								max[i] = std::numeric_limits<signed int>::max();
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						case _TIFF_FLOAT:
						{
							sampleformat = 3;
							bpp = 32;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							float *min = new float[channels];
							float *max = new float[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = 0.0f;
								max[i] = 1.0f;
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						case _TIFF_DOUBLE:
						{
							sampleformat = 3;
							bpp = 64;
							
							TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bpp);
							TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleformat);
							
							double *min = new double[channels];
							double *max = new double[channels];
							
							for (int i=0; i<channels; i++)
							{
								min[i] = 0.0;
								max[i] = 1.0;
							}
							
							TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, min);
							TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, max);
							
							delete[] min;
							delete[] max;
							
							break;
						}
						
						default:
							throw std::logic_error("Unsupported Tiff storage type");
					}
					
					// Set segment sizes
					assert((segSizeX & 15) == 0);	// Must be a multiple of 16
					assert((segSizeY & 15) == 0);
					
					TIFFSetField(tiff, TIFFTAG_TILEWIDTH, segSizeX);
					TIFFSetField(tiff, TIFFTAG_TILELENGTH, segSizeY);
					
					// Set compression algorithm
					TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
					//TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
				}
			}
			
			virtual ~LibTIFFImpl()
			{
				TIFFClose(tiff);
			}
			
			template<typename Dest>
					void readRect(int channel, int x, int y, int width, int height, Dest* buffer)
			{
				assert(writeMode_ == false);
				assert(width <= segmentSizeX() && height <= segmentSizeY());
				assert(x % segmentSizeX() == 0 && y % segmentSizeY() == 0);
				assert(channel < channels_);
				
				static long dataBufferSize = 0;
				static tdata_t dataBuffer = _TIFFmalloc(0);
				
				switch (tiffmode)
				{
					case TiffModeStrips:
					{
						// Reserve more memory if necessary
						tsize_t stripSize = TIFFStripSize(tiff);
						if (stripSize > dataBufferSize)
						{
							dataBuffer = _TIFFrealloc(dataBuffer, stripSize);
							dataBufferSize = stripSize;
						}
						
						// Read strip
						tstrip_t stripID = TIFFComputeStrip(tiff, y, 0);
						tsize_t sizeRead = TIFFReadEncodedStrip(tiff, stripID, dataBuffer, stripSize);
						assert(sizeRead == (width * height * channels_ * sizeof(Dest)));
						
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
							dataBuffer = _TIFFrealloc(dataBuffer, tileSize);
							dataBufferSize = tileSize;
						}
						
						// Read tile
						ttile_t tileID = TIFFComputeTile(tiff, x, y, 0, 0);
						tsize_t sizeRead = TIFFReadEncodedTile(tiff, tileID, dataBuffer, tileSize);
						assert(sizeRead == tileSize);
						
						// Extract channel and write to destination buffer
						int lineCount = height;
						int lineLength = width;
						int lineSkip = segmentSizeX() - lineLength;
						Dest *destBufferPixel = buffer;
						Dest *tileBufferPixel = (Dest *)dataBuffer + channel;
						
						for (int y=0; y<lineCount; y++)
						{
							for (int x=0; x<lineLength; x++)
							{
								*destBufferPixel = *tileBufferPixel;
								destBufferPixel++;
								tileBufferPixel += channels_;
							}
							tileBufferPixel += lineSkip * channels_;
						}
					}
					break;
				}
			}
			
			template<typename Src>
			struct TileCacheEntry
			{
				int channels;
				bool *channelMarker;
				tdata_t dataBuffer;
				
				TileCacheEntry()
					: channels(1), channelMarker(0), dataBuffer(0)
				{
				}
				
				bool isFinished()
				{
					for (int i=0; i<channels; i++)
						if (!channelMarker[i])
							return false;
					
					return true;
				}
			};
			
			template<typename Src>
					void replaceRect(int channel, int x, int y, int width, int height, const Src* buffer)
			{
				assert(writeMode_ == true);
				assert(width <= segmentSizeX() && height <= segmentSizeY());
				assert(x % segmentSizeX() == 0 && y % segmentSizeY() == 0);
				assert(channel < channels_);
				
				// Initialize the static tile cache
				static std::map<ttile_t, TileCacheEntry<Src> > tileCache;
				
				// Compute the tile-id and create new cache entry (if it's a new tile)
				ttile_t tileid = TIFFComputeTile(tiff, x, y, 0, 0);
				
				typename std::map<ttile_t, TileCacheEntry<Src> >::iterator entryIter = tileCache.find(tileid);
				if (entryIter == tileCache.end())
				{
					TileCacheEntry<Src> entry;
					entry.dataBuffer = _TIFFmalloc(TIFFTileSize(tiff));
					entry.channels = channels_;
					entry.channelMarker = new bool[channels_];
					for (int i=0; i<channels_; i++)
						entry.channelMarker[i] = false;
					
					tileCache[tileid] = entry;
					entryIter = tileCache.find(tileid);
				}
				
				// Write tile-data into the cache
				int lineCount = height;
				int lineLength = width;
				int lineSkip = segmentSizeX() - lineLength;
				const Src *srcBufferPixel = buffer;
				Src *tileBufferPixel = (Src *)entryIter->second.dataBuffer + channel;
				
				for (int y=0; y<lineCount; y++)
				{
					for (int x=0; x<lineLength; x++)
					{
						*tileBufferPixel = *srcBufferPixel;
						srcBufferPixel++;
						tileBufferPixel += channels_;
					}
					tileBufferPixel += lineSkip * channels_;
				}
				
				entryIter->second.channelMarker[channel] = true;
				
				// If the tile was finished, write it into the tiff and remove the cache entry
				if (entryIter->second.isFinished())
				{
					// Write tile into tiff file
					tsize_t sizeExpected = TIFFTileSize(tiff);
					tsize_t sizeWritten = TIFFWriteEncodedTile(tiff, tileid, entryIter->second.dataBuffer, sizeExpected);
					assert(sizeWritten == sizeExpected);
					
					// Release cache entry
					delete[] entryIter->second.channelMarker;
					_TIFFfree(entryIter->second.dataBuffer);
					tileCache.erase(entryIter);
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
			
			int segmentSizeX() const
			{
				return segSizeX_;
			}
			
			int segmentSizeY() const
			{
				return segSizeY_;
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
