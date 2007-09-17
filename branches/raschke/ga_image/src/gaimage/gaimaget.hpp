/***************************************************************************
                          gaimaget.hpp  -  GeoAIDA template impl.
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : Jürgen Bückner, Oliver Stahlhut, Martin Pahl,
                           Julian Raschke
    email                : bueckner@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <stdexcept>
#include <stdarg.h>

namespace Ga {
template <class PixTyp>
double ImageT<PixTyp>::getPixelAsDouble(int x, int y, int channel, double neutral) const {
  return getPixel(x, y, channel, neutral);
}

template <class PixTyp>
void ImageT<PixTyp>::setPixelToDouble(int x, int y, double val, int channel, bool clip) {
  setPixel(x, y, val, channel, clip);
}

template <class PixTyp>
ImageT<PixTyp>::ImageT(int x, int y, int noChannels) : ImageBase() {
  // Unsigned values would make more sense, but can be dangerous as well.
  // Let's put our fate into good, old assert's hand.
  assert(x >= 0);
  assert(y >= 0);
  
  sizeX_ = x;
  sizeY_ = y;
  channels.resize(noChannels);
  
  // Protection against integer overflows.
  LargeSize size = x;
  size *= y;
  size *= sizeof(PixTyp);
  // Actual allocation.
  for (int i = 0; i < noChannels; ++i)
    channels[i] = Cache::get().alloc(size);
  
  setFileType(_UNKNOWN);
}

template <class PixTyp>
ImageBase* ImageT<PixTyp>::copyObject() {
  return new ImageT(*this);
}

template <class PixTyp>
const class std::type_info& ImageT<PixTyp>::typeId() const {
  return typeid(PixTyp);
}

template <class PixTyp>
int ImageT<PixTyp>::noChannels() const {
  return channels.size();
}

template <class PixTyp>
PixTyp ImageT<PixTyp>::getPixel(int x, int y, int channel, PixTyp neutral) const {
  assert(channel>=0);
  assert(channel<noChannels());
  if (x>=0 && x<sizeX() && y>=0 && y<sizeY())
    return constBegin(y,channel)[x];
  else
    return neutral;
}

template <class PixTyp>
void ImageT<PixTyp>::partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height) {
  const ImageT& rval = dynamic_cast<const ImageT&>(rvalue);

  assert(width >= 0);
  assert(width < rval.sizeX() - x0);
  assert(height >= 0);
  assert(height < rval.sizeY() - y0);
  assert(x0 >= 0);
  assert(x0 < rval.sizeX());
  assert(y0 >= 0);
  assert(y0 < rval.sizeY());
  
  resize(width, height, rval.noChannels());
  setFileType(rval.fileType());
  for (int c = 0; c < noChannels(); ++c)
    for (int row = 0; row < sizeY(); ++row)
      std::copy(rval.constBegin(row + y0, c) + x0, rval.constEnd(row, c), begin(row, c));
}

template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::begin(int row, int channel) {
  return Iterator(channels.at(channel), row * sizeX());
}

template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constBegin(int row, int channel) const {
  return ConstIterator(channels.at(channel), row * sizeX());
}

template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::end(int row, int channel) {
  return Iterator(channels.at(channel), (row + 1) * sizeX());
}
	
template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::end() { // TODO: WTF?
  return end(sizeY(), noChannels());
}
	
template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::endChannel(int channel) {
  return end(sizeY(), channel);
}

template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constEnd(int row, int channel) const {
  return ConstIterator(channels.at(channel), (row + 1) * sizeX());
}
	
template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constEnd() const { // TODO: WTF?
  return constEnd(sizeY(), noChannels());
}
	
template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constEndChannel(int channel) const {
  return constEnd(sizeY(), channel);
}

template <class PixTyp>
PixTyp& ImageT<PixTyp>::operator() (int x, int y, int channel) {
  return begin(y,channel)[x];
}

template <class PixTyp>
void ImageT<PixTyp>::setPixel(int x, int y, PixTyp val, int channel, bool clip) {
	assert(channel>=0);
	assert(channel<noChannels());
  if (!clip || x >= 0 && x < sizeX() && y >= 0 && y < sizeY())
    begin(y,channel)[x] = val;
}

template <class PixTyp>
void ImageT<PixTyp>::fillRow(int row, int startX, int endX, double val, int channel) {
  Iterator it = begin(row, channel);
  std::fill(it + startX, it + endX, static_cast<PixTyp>(val));
}

template <class PixTyp>
double ImageT<PixTyp>::findMaxValue(int channel) {
  return *std::max_element(begin(0, channel), endChannel(channel));
}

template <class PixTyp>
double ImageT<PixTyp>::findMinValue(int channel) {
  return *std::min_element(begin(0, channel), endChannel(channel));
}

template <class PixTyp>
void ImageT<PixTyp>::resize(int rx, int ry, int noChannels) {
  ImageT(rx, ry, noChannels).swap(*this);
}

template <class PixTyp>
void ImageT<PixTyp>::fill(double value) {
  for (int c = 0; c < noChannels(); ++c)
    std::fill(begin(0, c), end(sizeY(), c), static_cast<PixTyp>(value));
}

template <class PixTyp>
void ImageT<PixTyp>::getChannel(ImageBase& resultImg, int channel)
{
  ImageT<PixTyp>& pic = dynamic_cast<ImageT<PixTyp>&>(resultImg);
  ImageT<PixTyp> channelCopy(sizeX(), sizeY(), 1);
  channelCopy.channels[0] = channels.at(channel);
  pic.swap(channelCopy);
}

template <class PixTyp>
void ImageT<PixTyp>::read(ImageIO& io) {
	int storageType=PFM_LAST;
	IMGTYPE type = _UNKNOWN;
 	if (typeid(PixTyp)==typeid(float)) {
		storageType=PFM_FLOAT;
		type=_PFM_FLOAT;
	}
	else if (typeid(PixTyp)==typeid(char)) {
		storageType=PFM_BYTE;
		type=_PGM;
	}
	else if (typeid(PixTyp)==typeid(unsigned char)) {
		storageType=PFM_BYTE;
		type=_PGM;
	}
	else if (typeid(PixTyp)==typeid(signed char)) {
		storageType=PFM_BYTE;
		type=_PGM;
	}
	else if (typeid(PixTyp)==typeid(short int)) {
		storageType=PFM_UINT16;
		type=_PFM_UINT16;
	}
	else if (typeid(PixTyp)==typeid(unsigned short int)) {
		storageType=PFM_UINT16;
		type=_PFM_UINT16;
	}
	else if (typeid(PixTyp)==typeid(signed short int)) {
		storageType=PFM_SINT16;
		type=_PFM_SINT16;
	}
	else if (typeid(PixTyp)==typeid(int)) {
		storageType=PFM_UINT;
		type=_PFM_UINT;
	}
	else if (typeid(PixTyp)==typeid(unsigned int)) {
		storageType=PFM_UINT;
		type=_PFM_UINT;
	}
	else if (typeid(PixTyp)==typeid(signed int)) {
		storageType=PFM_SINT;
		type=_PFM_SINT;
	}
	else if (typeid(PixTyp)==typeid(bool)) {
		storageType=PFM_BIT;
		type=_PBM;
	}
	else throw std::runtime_error("Unsupported data type");

  if (noChannels()==3 && storageType==PFM_BYTE) { // PPM
  	type=_PPM;
  }
	setFileType(type);
	
  switch (type) {
  case _PPM: {
    // Special case - this is read via PFM library, but uses 3 channels.
    
    // Get metrics and data via PFM library
  	int cols, rows;
  	float min, max;
    PFM3Byte *ppmbuffer =
      static_cast<PFM3Byte*>(pfm_readpfm_type(io.__fp__(), &cols, &rows, &min, &max, PFM_3BYTE, 0));
      
    // Copy values into this object - ought to be proper size already
    assert(cols == sizeX());
    assert(rows == sizeY());
    assert(noChannels() == 3);
    std::copy(ppmbuffer->r, ppmbuffer->r + cols * rows, begin(0, 0));
    std::copy(ppmbuffer->g, ppmbuffer->g + cols * rows, begin(0, 1));
    std::copy(ppmbuffer->b, ppmbuffer->b + cols * rows, begin(0, 2));

	  free(ppmbuffer);
  	break;
 		}
 		
  case _PBM: {
    // Special case - this is read via libpbm.
    
    // Start reading via PBM library.
 	  int cols, rows;
 	  int format;
  	pbm_readpbminit(io.__fp__(), &cols, &rows, &format);

    assert(cols == sizeX());
    assert(rows == sizeY());
    assert(noChannels() == 1); // <- This was 3 in earlier version; why?!
   	
    std::vector<bit> row(sizeX());
   		
    for (int y = 0; y < sizeY(); ++y) {
   		pbm_readpbmrow(io.__fp__(), &row[0], sizeX(), format);

      Iterator it = begin(y, 0);
      for (int x = 0; x < sizeX(); ++x, ++it)
        *it = row[x];
 	  }
 		break;
 		}
 		
 	case _UNKNOWN:
    throw std::runtime_error("Unknown image type");
    
  default: {
    // Usual case - one channel that is read via PFM library.
    
  	int cols, rows;
  	float min, max;
  	
    // HERE it gets ugly. This assumes this image has the right typeId()!
    // typeId() and fileType() were supposed to be independent.
    // TODO!
		PixTyp* data =
		  static_cast<PixTyp*>(pfm_readpfm_type(io.__fp__(), &cols, &rows, &min, &max, storageType, 0));
    //if (pfm_geo_get(&gW,&gN,&gE,&gS)) setGeoCoordinates(gW,gN,gE,gS);
    
    std::copy(data, data + cols*rows, begin());
	  free(data);
  	break;
  	}
  }
}

template <class PixTyp>
void ImageT<PixTyp>::write(ImageIO& io, int channel, const char* comment) {
	int storageType=PFM_LAST;
 	if (typeid(PixTyp)==typeid(float)) {
		storageType=PFM_FLOAT;
	}
	else if (typeid(PixTyp)==typeid(unsigned char)) {
		storageType=PFM_BYTE;
	}
	else if (typeid(PixTyp)==typeid(signed char)) {
		storageType=PFM_BYTE;
	}
	else if (typeid(PixTyp)==typeid(unsigned short int)) {
		storageType=PFM_UINT16;
	}
	else if (typeid(PixTyp)==typeid(signed short int)) {
		storageType=PFM_SINT16;
	}
	else if (typeid(PixTyp)==typeid(unsigned int)) {
		storageType=PFM_UINT;
	}
	else if (typeid(PixTyp)==typeid(signed int)) {
		storageType=PFM_SINT;
	}
	else if (typeid(PixTyp)==typeid(bool)) {
		storageType=PFM_BIT;
	}
	else fprintf(stderr,"Warning: Unsupport data type\n");
	
	switch (fileType()) {
		case _PPM: {
		  PFM3Byte ppmbuffer;
		  // TODO: This sucks.
		  ppmbuffer.r=(unsigned char*)&*constBegin(0,0);
 	 		ppmbuffer.g=(unsigned char*)&*constBegin(0,1);
			ppmbuffer.b=(unsigned char*)&*constBegin(0,2);
  		pfm_writepfm_type(io.__fp__(),&ppmbuffer,sizeX(),sizeY(),1,-1,PFM_3BYTE);
  		}
			break;
		case _PBM: {
 	    int sizeX_ = sizeX(), sizeY_ = sizeY();
      std::vector<bit> row(sizeX_);
   		
   		pbm_writepbminit(io.__fp__(), sizeX_, sizeY_, 0);
     		
   		for (int y = 0; y < sizeY_; ++y) {
   			ConstIterator pBit=constBegin(y,channel);
     		bit* elem_pix = &row[0];
       	
   		  for (int x = 0; x < sizeX_; ++x, ++elem_pix, ++pBit) {
           *elem_pix=(*pBit ? 1 : 0);
   		  }
   		  pbm_writepbmrow(io.__fp__(), &row[0], sizeX_, 0);
  	  }
  		}
			break;
		default:
  		pfm_writepfm_type(io.__fp__(),&*constBegin(0,channel),sizeX(),sizeY(),1,-1,storageType);
	}
}

} // namespace Ga
