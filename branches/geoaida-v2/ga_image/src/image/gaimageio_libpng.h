/****************************************************************************
                          gaimageio_libpng.h -
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
#include <stdexcept>
#include <png.h>

namespace Ga
{
	bool checkPNG(const std::string &filename, int &sizeX, int &sizeY, int &channels, FileType &storageType)
	{
		FILE *file = fopen(filename.c_str(), "rb");
		if (!file)
        	return false;
		
		png_byte signature[8];
		fread(signature, sizeof(png_byte), 8, file);
		if (png_sig_cmp(signature, 0, 8))
		{
			fclose(file);
			return false;
		}
		
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
		{
			fclose(file);
			return false;
		}
		
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			fclose(file);
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return false;
		}
		
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fclose(file);
			return false;
		}
		
		png_init_io(png_ptr, file);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);
		
		sizeX = info_ptr->width;
		sizeY = info_ptr->height;
		png_byte color_type = info_ptr->color_type;
		png_byte bit_depth = info_ptr->bit_depth;
		
		switch (color_type)
		{
			case PNG_COLOR_TYPE_GRAY:
				channels = 1;
				switch (bit_depth)
				{
					case 8:
						storageType = _PNG_UINT8;
						break;
					
					case 16:
						storageType = _PNG_UINT16;
						break;
					
					default:
						fclose(file);
						png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
						return false;
				}
				break;
			
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				channels = 2;
				switch (bit_depth)
				{
					case 8:
						storageType = _PNG_UINT8;
						break;
					
					case 16:
						storageType = _PNG_UINT16;
						break;
					
					default:
						fclose(file);
						png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
						return false;
				}
				break;
			
			case PNG_COLOR_TYPE_PALETTE:
				channels = 3;
				storageType = _PNG_UINT8;
				break;
			
			case PNG_COLOR_TYPE_RGB:
				channels = 3;
				switch (bit_depth)
				{
					case 8:
						storageType = _PNG_UINT8;
						break;
					
					case 16:
						storageType = _PNG_UINT16;
						break;
					
					default:
						fclose(file);
						png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
						return false;
				}
				break;
			
			case PNG_COLOR_TYPE_RGB_ALPHA:
				channels = 4;
				switch (bit_depth)
				{
					case 8:
						storageType = _PNG_UINT8;
						break;
					
					case 16:
						storageType = _PNG_UINT16;
						break;
					
					default:
						fclose(file);
						png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
						return false;
				}
				break;
			
			default:
				fclose(file);
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				return false;
		}
		
		// Destroy png structs
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		
		// Close file
		fclose(file);
		return true;
	}

	class LibPNGImpl
	{
		LibPNGImpl(const LibPNGImpl&);
		LibPNGImpl& operator=(const LibPNGImpl&);
		
		static const std::type_info& typeIdFromFileType(FileType fileType)
		{
			switch (fileType)
			{
				case _PNG_UINT8:	return typeid(unsigned char);
				case _PNG_UINT16:	return typeid(unsigned short);
				
				default:
					throw std::logic_error("Unsupported Png storage type");
			}
		}
		
		FILE *file;
		FileType type;
		
		std::string comment_;
		float fileMin_, fileMax_;
		int sizeX_, sizeY_;
		int channels_;
		
		public:
			LibPNGImpl(const std::string &filename, FileType fileType, int sizeX, int sizeY, int channels = 1)
			: file(0), type(fileType), sizeX_(sizeX), sizeY_(sizeY), channels_(channels)
			{
				file = fopen(filename.c_str(), "rb");
				
				switch (type)
				{
					case _PNG_UINT8:
						fileMin_ = 0;
						fileMax_ = (1 << 8) - 1;
						break;
					
					case _PNG_UINT16:
						fileMin_ = 0;
						fileMax_ = (1 << 16) - 1;
						break;
					
					default:
						fileMin_ = fileMax_ = 0;
						break;
				}
			}
			
			~LibPNGImpl()
			{
				if (file != 0)
					fclose(file);
			}
			
			template<typename Dest>
					void readRect(int channel, int x, int y, int width, int height, Dest* buffer)
			{
				if (!file)
					return;
				
				// Jump to start of file and init libpng reading
				fseek(file, 0, SEEK_SET);
				
				png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if (!png_ptr)
					return;
				
				png_infop info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
				{
					png_destroy_read_struct(&png_ptr, NULL, NULL);
					return;
				}
				
				//if (setjmp(png_jmpbuf(png_ptr)))
				//	return;
				
				png_init_io(png_ptr, file);
				png_set_sig_bytes(png_ptr, 0);
				
				// Read image data
				png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);
				png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
				
				Dest *destBufferPixel = buffer;
				for (int y = 0; y < height; y++)
				{
					// Copy channel data to output buffer
					int pixelcount = width;
					Dest *srcBufferPixel = (Dest *)row_pointers[y] + channel;
					
					while (pixelcount--)
					{
						*destBufferPixel = *srcBufferPixel;
						destBufferPixel++;
						srcBufferPixel += channels();
					}
				}
				
				// Destroy png structs
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
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
