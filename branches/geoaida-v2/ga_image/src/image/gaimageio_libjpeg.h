/****************************************************************************
                          gaimageio_libjpeg.h -
                             -------------------
    begin                : 2008-08-11
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

#include "gaimageio.h"
#include "gaimage.h"
#include <algorithm>
#include <map>
#include <stdexcept>
#include <jpeglib.h>

namespace Ga
{
	bool checkJPEG(const std::string &filename, int &sizeX, int &sizeY, int &channels, FileType &storageType)
	{
		// Open jpeg file handle and check for magic number
		unsigned short magicnumber;
		FILE *file = fopen(filename.c_str(), "rb");
		fread(&magicnumber, sizeof(magicnumber), 1, file);
		fseek(file, 0, SEEK_SET);
		
		if (magicnumber != 0xD8FF)
		{
			fclose(file);
			return false;
		}
		
		// Initialize JPEG info structs
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;
		
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, file);
		
		// Read header
		if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
		{
			fclose(file);
			jpeg_destroy_decompress(&cinfo);
			return false;
		}
		
		// Set parameters
		storageType = _JPEG_UINT8;
		sizeX = cinfo.image_width;
		sizeY = cinfo.image_height;
		channels = cinfo.num_components;
		
		// Close file
		fclose(file);
		jpeg_destroy_decompress(&cinfo);
		
		return true;
	}

	class LibJPEGImpl
	{
		LibJPEGImpl(const LibJPEGImpl&);
		LibJPEGImpl& operator=(const LibJPEGImpl&);
		
		static const std::type_info& typeIdFromFileType(FileType fileType)
		{
			switch (fileType)
			{
				case _JPEG_UINT8:	return typeid(unsigned char);
				
				default:
					throw std::logic_error("Unsupported Jpeg storage type");
			}
		}
		
		FILE *file;
		FileType type;
		
		std::string comment_;
		float fileMin_, fileMax_;
		int sizeX_, sizeY_;
		int channels_;
		
		public:
			LibJPEGImpl(const std::string &filename, FileType fileType, int sizeX, int sizeY, int channels = 1)
			: file(0), type(fileType), sizeX_(sizeX), sizeY_(sizeY), channels_(channels)
			{
				file = fopen(filename.c_str(), "rb");
				
				fileMin_ = 0;
				fileMax_ = 255;
			}
			
			~LibJPEGImpl()
			{
				if (file != 0)
					fclose(file);
			}
			
			template<typename Dest>
					void readRect(int channel, int x, int y, int width, int height, Dest* buffer)
			{
				if (!file)
					return;
				
				// Initialize JPEG info structs
				struct jpeg_decompress_struct cinfo;
				struct jpeg_error_mgr jerr;
				
				cinfo.err = jpeg_std_error(&jerr);
				jpeg_create_decompress(&cinfo);
				
				// Reset jpeg file handle
				fseek(file, 0, SEEK_SET);
				jpeg_stdio_src(&cinfo, file);
				
				// Start decompression
				jpeg_read_header(&cinfo, TRUE);
				jpeg_start_decompress(&cinfo);
				
				// Initalize data buffer
				assert((x == 0) && (y == 0));
				assert((width == segmentSizeX()) && (height == segmentSizeY()));
				
				JSAMPLE *dataBuffer[1] = { new JSAMPLE[width * channels()] };
				
				// Read scanlines
				Dest *destBufferPixel = buffer;
				for (int y = 0; y < height; y++)
				{
					// Read complete scanline to input buffer
					jpeg_read_scanlines(&cinfo, dataBuffer, 1);
					
					// Copy channel data to output buffer
					int pixelcount = width;
					Dest *srcBufferPixel = (Dest *)dataBuffer[0] + channel;
					
					while (pixelcount--)
					{
						*destBufferPixel = *srcBufferPixel;
						destBufferPixel++;
						srcBufferPixel += channels();
					}
				}
				
				// Finish decompression
				jpeg_finish_decompress(&cinfo);
				jpeg_destroy_decompress(&cinfo);
				
				// Release data buffer
				delete[] dataBuffer[0];
			}
			
			template<typename Src>
					void replaceRect(int channel, int x, int y, int width, int height, const Src* buffer)
			{
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
				return sizeX_;
			}
			
			int segmentSizeY() const
			{
				return sizeY_;
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
