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
  std::fill(pChannel_, pChannel_ + GA_MAX_CHANNELS, (Array2DT<PixTyp>*) 0);
  initialize( x, y, noChannels );
}

template <class PixTyp>
ImageT<PixTyp>::ImageT (const ImageT<PixTyp>& rval) 
  : ImageBase(rval) {
  noChannels_=rval.noChannels_;
  for (int c=0; c<noChannels_; c++) {
    pChannel_[c]=rval.pChannel_[c]->shallowCopy();
  }
  setFileType(rval.fileType());
}

template <class PixTyp>
ImageT<PixTyp>::~ImageT()
{
  for (int c=0; c<noChannels(); c++)
    if (pChannel_[c]) pChannel_[c]=pChannel_[c]->unlink();
}

template <class PixTyp>
ImageT<PixTyp>& ImageT<PixTyp>::operator= (const ImageT<PixTyp>& rval) {
  if (this == &rval)
    return *this;
  ImageT<PixTyp>(rval).swap(*this);
  return *this;
}
	
template <class PixTyp>
ImageBase* ImageT<PixTyp>::copyObject() {
  return new ImageT(*this);
}

template <class PixTyp>
#if __GNUC__==2
const class type_info& ImageT<PixTyp>::typeId() const {
#else
const class std::type_info& ImageT<PixTyp>::typeId() const {
#endif
  return typeid(PixTyp);
}

template <class PixTyp>
int ImageT<PixTyp>::noChannels() const {
  return noChannels_;
}

template <class PixTyp>
PixTyp ImageT<PixTyp>::getPixel(int x, int y, int channel, PixTyp neutral) const {
  assert(channel>=0);
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  if ((x>=0) && (x<sizeX_) && (y>=0) && (y<sizeY_))
    return constBegin(y,channel)[x];
  else return neutral;
}

template <class PixTyp>
void ImageT<PixTyp>::partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height) {
  const ImageT& rval=(const ImageT&)rvalue;
  assert(width >= 0);
  assert(width < rval.sizeX_ - x0);
  assert(height >= 0);
  assert(height < rval.sizeY_ - y0);
  assert(x0 >= 0);
  assert(x0 < rval.sizeX_);
  assert(y0 >= 0);
  assert(y0 < rval.sizeY_);
  
  resize(width, height, rval.noChannels());
  setFileType(rval.fileType());
  for (int c=0; c<noChannels_; c++)
    for( int row = 0; row < sizeY_; row++ )
      memcpy( begin(row,c), rval.constBegin(row + y0,c) + x0, sizeX_ * sizeof(PixTyp) );
}









/** return pointer to the beginning of data
    usage: \code Iterator dp = M.begin(row); \endcode */
template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::begin(int row, int channel) {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return pChannel_[channel]->begin(pChannel_[channel],row);
}

template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constBegin(int row, int channel) const {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return (pChannel_[channel]->constBegin(row));
}

template <class PixTyp>
void* ImageT<PixTyp>::beginVoid(int row, int channel) {
  return begin(row,channel);
}

template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::end(int row, int channel) const {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return (pChannel_[channel]->end(row));
}
	
template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::end() const {
  assert(pChannel_[0]!=0);
  return (pChannel_[0]->end());
}
	
template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::endChannel(int channel) const {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return (pChannel_[channel]->end());
}

template <class PixTyp>
const void* ImageT<PixTyp>::endVoid(int row, int channel) const {
  return end(row,channel);
}
	
template <class PixTyp>
const void* ImageT<PixTyp>::endVoid() const {
  return end();
}
	
template <class PixTyp>
const void* ImageT<PixTyp>::constBeginVoid(int row, int channel) const {
  return constBegin(row,channel);
}
	
	
	

/** assign and retrieve matrix values
    usage: \code PixTyp x = A( 1, 5 ); A( 3, 7 ) = 5; \endcode */
template <class PixTyp>
PixTyp& ImageT<PixTyp>::operator () (int x, int y, int channel) {
  assert(channel>=0);
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  assert(x>=0);
  assert(x<sizeX_);
  assert(y>=0);
  assert(y<sizeY_);
  return begin(y,channel)[x];
}

/** Assign value to matrix element, check for legal row and column index
    usage: \code  A.SetElement( 3, 4, 4444 );       \endcode
    \param x x of element (column)
    \param y y of element (row)
    \param val value to set element to */
template <class PixTyp>
void ImageT<PixTyp>::setPixel(int x, int y, PixTyp val, int channel, bool clip) {
  	assert(channel>=0);
  	assert(channel<noChannels_);
  	assert(pChannel_[channel]!=0);
  	if (clip)
  	{
  	  if (x >= 0 && x < sizeX_ && y >= 0 && y < sizeY_)
      	begin(y,channel)[x] = val;
      else
        return;
    }
    else
    {
    	assert(x>=0);
    	assert(x<sizeX_);
  	  assert(y>=0);
    	assert(y<sizeY_);
      begin(y,channel)[x] = val;
    }
}

template <class PixTyp>
void ImageT<PixTyp>::setFloat(void* it, double val) {
	*(PixTyp*)it=(PixTyp)val;
}

template <class PixTyp>
void ImageT<PixTyp>::fillRow(void *it, int startX, int endX, double val, int channel) {
  PixTyp* ptr=(PixTyp*)it;
  PixTyp* eptr=ptr+endX;
  ptr+=startX;
  PixTyp v=PixTyp(val);
  for (;ptr<=eptr;ptr++) {
    *ptr=v;
  }
}

template <class PixTyp>
void* ImageT<PixTyp>::nextCol(const void*& ptr, int offset) const {
  return ((PixTyp*)ptr)+offset;
}

template <class PixTyp>
void* ImageT<PixTyp>::nextCol(const void*& ptr) const {
  return ((PixTyp*)ptr)+1;
}


//------------------------------------ Tool functions -----------------------

/** Maximum element value of a ImageT
    usage: \code  PixTyp xmax = A.Max( channel=0 ); PixTyp xmax = A.Max( x, y, channel=0 ); \endcode */
template <class PixTyp>
double ImageT<PixTyp>::findMaxValue(int channel) {
  ConstIterator index = constBegin(0, channel);
  PixTyp max = *index;
  unsigned int e = 0;

  for(unsigned int i = 1; i < noPixels(); i++) {
    if (max < *index) {
      max = *index;
      e = i;
    }
    ++ index;
  }

  return max;
}

/** Minimum of a ImageT
    usage: \code PixTyp xmin = A.Min( channel=0 ); \endcode */
template <class PixTyp>
double ImageT<PixTyp>::findMinValue(int channel) {
#if 0
  if(nSize_ < 0)
    return *(begin());
#endif

  ConstIterator index = constBegin(0, channel);
  PixTyp min = *index;
  unsigned int e = 0;

  for(unsigned int i = 1; i < noPixels(); i++) {
    if (min > *index) {
      min = *index;
      e = i;
    }
    ++ index;
  }

  return min;
  }

/** initialization; common function for initialization; for internal use only
    usage: \code Initialize( x, y ) \endcode \endcode */
template <class PixTyp>
void ImageT<PixTyp>::initialize( int x, int y, int noChannels ) {
  for (int c=0; c<noChannels_; c++)
    if (pChannel_[c]) pChannel_[c]=pChannel_[c]->unlink();

  noChannels_=noChannels;
  for (int c=0; c<noChannels_; c++) {
    if( x < 1 || y < 1 ) {
      sizeY_ = sizeX_ = 0;
    }
    else {
      sizeX_ = x;
      sizeY_ = y;
    }
    pChannel_[c]=new Array2DT<PixTyp>(sizeX_, sizeY_);
  }
  setFileType(_UNKNOWN);
}

/** prepare assignments; Preparing an assignment by adjusting "this" */
template <class PixTyp>
void ImageT<PixTyp>::resize(int rx, int ry, int noChannels) {
  ImageT(rx, ry, noChannels).swap(*this);
}

template <class PixTyp>
void ImageT<PixTyp>::fill(double value) {
  for (int c=0; c<noChannels_; c++) {
    assert(pChannel_[c]!=0);
    Iterator w=begin(0,c);
    for(unsigned int i = noPixels() - 1; i; i--, w ++)
      *w = value;
    *w = value;
  }	
}

/** set a pixel  */
template <class PixTyp>
void ImageT<PixTyp>::setInt(void *ptr, int val) {
  PixTyp *p=(PixTyp*)ptr;
  *p=PixTyp(val);
}

/** set a pixel  */
template <class PixTyp>
int ImageT<PixTyp>::getInt(const void *ptr) const {
  PixTyp *p=(PixTyp*)ptr;
  return int(*p);
}

template <class PixTyp>
void ImageT<PixTyp>::getChannel(ImageBase& resultImg, int channel)
{
  ImageT<PixTyp>& pic=(ImageT<PixTyp>&)resultImg;
	pic.resize(sizeX(), sizeY());

  pic.pChannel_[0] = pChannel_[channel]->shallowCopy();
}

template <class PixTyp>
void ImageT<PixTyp>::maxValue(ImageBase &inImg, float value) {
  ImageT<PixTyp>& in=(ImageT<PixTyp>&)inImg;
  PixTyp val = (PixTyp)value;
  Iterator elem = in.begin(0);
  int size = in.sizeX()*in.sizeY();

  for (int i = 0; i < size; i++) {
      if(*elem > val) *elem = val;
      elem++;
    }
}

template <class PixTyp>
void ImageT<PixTyp>::minValue(ImageBase &inImg, float value) {
  ImageT<PixTyp>& in=(ImageT<PixTyp>&)inImg;
  PixTyp val = (PixTyp)value;
  Iterator elem = in.begin(0);
  int size = in.sizeX()*in.sizeY();

  for (int i = 0; i < size; i++) {
     if(*elem < val) *elem = val;
      elem++;
    }
}

template <class PixTyp>
bool ImageT<PixTyp>::read(FILE *fp) {
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
	else fprintf(stderr,"Warning: Unsupport data type\n");

  if (noChannels()==3 && storageType==PFM_BYTE) { // PPM
  	type=_PPM;
  }
	setFileType(type);
  switch (type) {
  case _PPM: {
  	int cols, rows;
  	float minval, maxval;
    PFM3Byte *ppmbuffer=(PFM3Byte*)pfm_readpfm_type(fp, &cols, &rows, &minval, &maxval,PFM_3BYTE,0);
    {
      double gW,gN,gE,gS;
      //if (pfm_geo_get(&gW,&gN,&gE,&gS)) setGeoCoordinates(gW,gN,gE,gS);
    }

		setData(cols,rows,3,ppmbuffer->r,ppmbuffer->g,ppmbuffer->b);
	  free(ppmbuffer);
  	break;
#if 0  		
 	  pixval maxval;
 	  int format;
 	  int cols, rows;
  	ppm_readppminit(fp, &cols, &rows, &maxval, &format);

   	resize(cols,rows,3);
   	
   	int sizeX_ = cols, sizeY_ = rows;
   	pixel *row = (pixel *)new pixel[sizeX_];
   		
    for (int y = 0; y < sizeY_; ++y) {
 		  Iterator pChR = begin(y,0);
 		  Iterator pChG = begin(y,1);
		  Iterator pChB = begin(y,2);
		  assert(pChR);
		  assert(pChG);
		  assert(pChB);
	   	pixel *elem_pix = row;
       		
   		ppm_readppmrow(fp, row, sizeX_, maxval, format);
       		
 		  for (int x = 0; x < sizeX_; ++x, ++elem_pix, ++pChR, ++pChG, ++pChB) {
 		  	*pChR=PPM_GETR(*elem_pix);
 		  	*pChG=PPM_GETG(*elem_pix);
 		  	*pChB=PPM_GETB(*elem_pix);
 		  }
 	  }
		delete row;				    	
 		break;
#endif
 		}
  case _PBM: {
 	  int format;
 	  int cols, rows;
  	pbm_readpbminit(fp, &cols, &rows, &format);

   	resize(cols,rows,3);
   	
   	int sizeX_ = cols, sizeY_ = rows;
   	bit *row = (bit*)new bit[sizeX_];
   		
    for (int y = 0; y < sizeY_; ++y) {
 		  Iterator pBit = begin(y,0);
		  assert(pBit);
	   	bit *elem_pix = row;
       		
   		pbm_readpbmrow(fp, row, sizeX_, format);
       		
 		  for (int x = 0; x < sizeX_; ++x, ++elem_pix, ++pBit) {
 		  	*pBit=(*elem_pix!=0);
 		  }
 	  }
		delete row;				    	
 		break;
 		}
 	case _UNKNOWN:
		fprintf(stderr,"##  (ERROR) unknown image type!");
 		break;  	
  default: {
  	int cols, rows;
  	float minval, maxval;
		void* data=pfm_readpfm_type(fp, &cols, &rows, &minval, &maxval,storageType,0);
    {
      double gW,gN,gE,gS;
      //if (pfm_geo_get(&gW,&gN,&gE,&gS)) setGeoCoordinates(gW,gN,gE,gS);
    }
		setData(cols,rows,data);
	  free(data);
  	break;
  	}
  }
  return true;
}

/* set data without copying. There must be noChannels data-pointer in the ellipse */
template <class PixTyp>
void ImageT<PixTyp>::setData(int x, int y, int noChannels, ...) {
  va_list argPtr;

  va_start(argPtr, noChannels);
  for (int c=0; c<noChannels_; c++)
    if (pChannel_[c]) pChannel_[c]=pChannel_[c]->unlink();
  noChannels_=noChannels;
  for (int c=0; c<noChannels_; c++) {
    if( x < 1 || y < 1 ) {
      sizeY_ = sizeX_ = 0;
    }
    else {
      sizeX_ = x;
      sizeY_ = y;
    }
    void* data=va_arg(argPtr,void*);
    pChannel_[c]=new Array2DT<PixTyp>(sizeX_, sizeY_,(PixTyp*)data);
  }

  va_end(argPtr);
}

/** create a matrix from an array
    usage: \code int w[]={1, 2, 3, 4, 5, 6}; ImageT A( 2, 3, w ); \endcode */
template <class PixTyp>
void ImageT<PixTyp>::setData(int x, int y, void* initvalues) {
  noChannels_=1;
  resize( x, y, 1 );

  PixTyp *w = begin();
  if( sizeY_ != 0 )
    memcpy( w, initvalues, ( noPixels() ) * sizeof( PixTyp ) );
}

template <class PixTyp>
bool ImageT<PixTyp>::write(FILE *fp, int channel, const char* comment) {
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
		  ppmbuffer.r=(unsigned char*)constBegin(0,0);
 	 		ppmbuffer.g=(unsigned char*)constBegin(0,1);
			ppmbuffer.b=(unsigned char*)constBegin(0,2);
  		pfm_writepfm_type(fp,&ppmbuffer,sizeX(),sizeY(),1,-1,PFM_3BYTE);
  		}
			break;
		case _PBM: {
 	    int sizeX_ = sizeX(), sizeY_ = sizeY();
 		  bit *row = (bit *)new bit[sizeX_];
   		
   		pbm_writepbminit(fp, sizeX_, sizeY_, 0);
     		
   		for (int y = 0; y < sizeY_; ++y) {
   			ConstIterator pBit=constBegin(y,channel);
     		bit* elem_pix = row;
       		
   		  for (int x = 0; x < sizeX_; ++x, ++elem_pix, ++pBit) {
           *elem_pix=(*pBit ? 1 : 0);
   		  }
   		  pbm_writepbmrow(fp, row, sizeX_, 0);
  	  }
   	  delete row;
  		}
			break;
		default:
  		pfm_writepfm_type(fp,constBegin(0,channel),sizeX(),sizeY(),1,-1,storageType);
	}
	return true;
}

} // namespace Ga
