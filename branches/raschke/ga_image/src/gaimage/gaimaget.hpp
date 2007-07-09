/***************************************************************************
                          GaImageT.hpp  -  geoAIDA template image class
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : Jürgen Bückner, Oliver Stahlhut, Martin Pahl
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

/*
 * $Source: /data/cvs/ga_image/ga_image/gaimage/gaimaget.hpp,v $
 * $Revision: 1.25 $
 * $Date: 2003/11/20 10:17:43 $
 * $Author: pahl $
 * $Locker:  $
 */

#include <stdarg.h>
namespace Ga {

  /** default constructor, defines a picture for later assignment */
  template <class PixTyp>
  inline ImageT<PixTyp>::ImageT() : ImageBase() {
    initialize( 0, 0, 1 );
  } 
  
  /** copy constructor, definition and initialization from a picture;
      usage: \code ImageT A = B; ImageT A = B + 2 * C; return A; \endcode */
  template <class PixTyp>
  inline ImageT<PixTyp>::ImageT (const ImageT<PixTyp>& rval) 
    : ImageBase(rval) {
    noChannels_=rval.noChannels_;
    for (int c=0; c<noChannels_; c++) {
      pChannel_[c]=rval.pChannel_[c]->shallowCopy();
    }
    typeImage(rval.typeImage());
  }
  
template <class PixTyp>
inline ImageBase* ImageT<PixTyp>::newObject(int x_size, int y_size, int channels) {
  ImageT<PixTyp>* p=new ImageT<PixTyp>(x_size, y_size, channels);
  p->typeImage(typeImage());
  return p;
}
	
template <class PixTyp>
inline ImageBase* ImageT<PixTyp>::copyObject() {
  return new ImageT(*this);
}

template <class PixTyp>
inline ImageT<PixTyp>& ImageT<PixTyp>::operator= (const ImageT<PixTyp>& rval) {
  if (this==&rval) return *this;
  for (int c=0; c<noChannels_; c++) {
    assert(pChannel_[c]!=0);
    pChannel_[c]=pChannel_[c]->unlink();
  }
  (ImageBase&)*this=(ImageBase&)rval;
  noChannels_=rval.noChannels();
  for (int c=0; c<noChannels_; c++) {
    sizeX_=rval.sizeX_;
    sizeY_=rval.sizeY_;
    assert(rval.pChannel_[c]!=0);
    pChannel_[c]=rval.pChannel_[c]->shallowCopy();
  }	  	
  return *this;
}

/** create picture from matrix
    usage: \code ga_pix A( y ); \endcode */
template <class PixTyp>
inline ImageT<PixTyp>::ImageT (const ImageBase& imgIn) : ImageBase(imgIn) {
  ImageT<PixTyp>& src=(ImageT<PixTyp>&)imgIn;
  noChannels_=src.noChannels_;
  for (int c=0; c<noChannels_; c++) {
    pChannel_[c]=src.pChannel_[c]->shallowCopy();
  }
  typeImage(src.typeImage());
}

/** size a picture and initialize it to zero; must then be assigned other values (for a picture of known dimension)
   usage: \code ImageT A( 4, 5 ); A( 1, 1 ) = 3; A( 1, 2 ) = 7; \endcode
   \param x x-size, number of columns
   \param y y-size, number of rows */
template <class PixTyp>
inline ImageT<PixTyp>::ImageT(int x, int y, int noChannels) : ImageBase() {
  initialize( x, y, noChannels );
}


/** create a picture from an array
    usage: \code int w[]={1, 2, 3, 4, 5, 6}; ImageT A( 2, 3, w ); \endcode
    \param x x-size, number of columns
    \param y y-size, number of rows */
template <class PixTyp>
inline ImageT<PixTyp>::ImageT (int x, int y, PixTyp *vec) : ImageBase()  {
  noChannels_=1;
  alloc( x, y, 1 );
  setData(x,y,vec);
}

/** create a picture from formatted File (ASCII)
    usage: \code ImageT A("MAT.DAT"); \endcode
    \param file filename */
template <class PixTyp>
inline ImageT<PixTyp>::ImageT (char *file) : ImageBase(file) {
}

/** create a subpicture with identical beginning (at [1,1])
    usage: \code ImageT B( 4, 5, A ); \endcode
    \param x new x-size, number of columns
    \param y new y-size, number of rows
    \param imgIn input picture */
template <class PixTyp>
inline ImageT<PixTyp>::ImageT (int x, int y, const ImageT& imgIn) : ImageBase() {
  assert(x >= 0);
  assert(x < imgIn.sizeX_);
  assert(y >= 0);
  assert(y < imgIn.sizeY_);

  typeImage(imgIn.typeImage());
  noChannels_=imgIn.noChannels_;
  initialize( x, y );

  for (int c=0; c<noChannels_; c++)
    for( int row = 0; row < sizeY_; row++ )
      memcpy( begin(row,c), imgIn.constBegin(row,c),sizeX_*sizeof(PixTyp) );
}

/** create subpicture starting at arg3, arg4 with size arg1 x arg2
    usage: \code ImageT B( m, n, i, j, A ); \endcode
    \param x new x-size, number of columns
    \param y new y-size, number of rows
    \param x0 starting offset x
    \param y0 starting offset y
    \param imgIn input picture */
template <class PixTyp>
inline ImageT<PixTyp>::ImageT (int x, int y, int x0, int y0, const ImageT& imgIn)
  : ImageBase() {
  	partCopy(imgIn,x,y,x0,y0);
}

/** destructor */
template <class PixTyp>
inline ImageT<PixTyp>::~ImageT(void)
{
  for (int c=0; c<noChannels_; c++)
    if (pChannel_[c]) pChannel_[c]=pChannel_[c]->unlink();
}

/* set data without copying. There must be noChannels data-pointer in the ellipse */
template <class PixTyp>
inline void ImageT<PixTyp>::setData(int x, int y, int noChannels, ...) {
  va_list argPtr;

  va_start(argPtr, noChannels);
  for (int c=0; c<noChannels_; c++)
    if (pChannel_[c]) pChannel_[c]=pChannel_[c]->unlink();
  noChannels_=noChannels;
  for (int c=0; c<noChannels_; c++) {
    if( x < 1 || y < 1 ) {
      sizeY_ = sizeX_ = nSize_ = 0;
    }
    else {
      sizeX_ = x;
      sizeY_ = y;
      nSize_ = (size_t) sizeX_ * sizeY_;
    }
    void* data=va_arg(argPtr,void*);
    pChannel_[c]=new Array2DT<PixTyp>(sizeX_, sizeY_,(PixTyp*)data);
  }

  va_end(argPtr);
}

/** create a matrix from an array
    usage: \code int w[]={1, 2, 3, 4, 5, 6}; ImageT A( 2, 3, w ); \endcode */
template <class PixTyp>
inline void ImageT<PixTyp>::setData(int x, int y, void* initvalues) {
  noChannels_=1;
  resize( x, y, 1 );

  PixTyp *w = begin();
  if( sizeY_ != 0 )
    memcpy( w, initvalues, ( nSize_ ) * sizeof( PixTyp ) );
}

template <class PixTyp>
inline void ImageT<PixTyp>::partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height) {
  const ImageT& rval=(const ImageT&)rvalue;
  assert(width >= 0);
  assert(width < rval.sizeX_ - x0);
  assert(height >= 0);
  assert(height < rval.sizeY_ - y0);
  assert(x0 >= 0);
  assert(x0 < rval.sizeX_);
  assert(y0 >= 0);
  assert(y0 < rval.sizeY_);

  initialize( width, height, rval.noChannels_ );
  typeImage(rval.typeImage());
  for (int c=0; c<noChannels_; c++)
    for( int row = 0; row < sizeY_; row++ )
      memcpy( begin(row,c), rval.constBegin(row + y0,c) + x0, sizeX_ * sizeof(PixTyp) );
}

template <class PixTyp>
#if __GNUC__==2
inline const class type_info& ImageT<PixTyp>::typeId() const {
#else
inline const class std::type_info& ImageT<PixTyp>::typeId() const {
#endif
  return typeid(PixTyp);
}

template <class PixTyp>
inline int ImageT<PixTyp>::noChannels() const {
  return noChannels_;
}

template <class PixTyp>
inline void ImageT<PixTyp>::rmChannel() {
  if (noChannels()>0) {
    noChannels_--;
    assert(pChannel_[noChannels_]!=0);
    pChannel_[noChannels_]=pChannel_[noChannels_]->unlink();
  }
}

template <class PixTyp>
inline void ImageT<PixTyp>::addChannel(const ImageBase& rvalue, int channel) {
	const ImageT& rval=(const ImageT&)rvalue;
  assert(rval.sizeX()==sizeX());
  assert(rval.sizeY()==sizeY());
  pChannel_[noChannels_]=rval.pChannel_[channel]->shallowCopy();
  noChannels_++;
}

//------------------------------ Access of matrix data ----------------------
/** clear matrix */
template <class PixTyp>
inline void ImageT<PixTyp>::clear() {
  for (int c=0; c<noChannels_; c++) {
    assert(pChannel_[c]!=0);
    if( (nSize_ != 0) && (begin(0,c) != 0) )
      memset( begin(0,c), 0, nSize_*sizeof(PixTyp) );
  }	    	
}

/** treats pointer this as if were a long integer */
template <class PixTyp>
inline unsigned long ImageT<PixTyp>::hash(void) const {
  return (((unsigned long)this)/3 );
}

/** return pointer to the beginning of data
    usage: \code Iterator dp = M.begin(row); \endcode */
template <class PixTyp>
inline typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::begin(int row, int channel) {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return pChannel_[channel]->begin(pChannel_[channel],row);
}

/** return pointer to the beginning of data
    usage: \code PixTyp * dp = M.data(row); \endcode */
template <class PixTyp>
inline typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::data(int row, int channel) {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return pChannel_[channel]->begin(pChannel_[channel],row);
}

/** return pointer to the beginning of data
    usage: \code Iterator dp = M.dataArrayT(channel); \endcode */
template <class PixTyp>
inline PixTyp** ImageT<PixTyp>::dataArrayT(int channel) {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return pChannel_[channel]->dataArray(pChannel_[channel]);
}

/** return pointer to the beginning of data
    usage: \code Iterator dp = M.dataArray(channel); \endcode */
template <class PixTyp>
inline void* ImageT<PixTyp>::dataArray(int channel) {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return dataArrayT(channel);
}

template <class PixTyp>
inline typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constBegin(int row, int channel) const {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return (pChannel_[channel]->constBegin(row));
}

template <class PixTyp>
inline void* ImageT<PixTyp>::beginVoid(int row, int channel) {
  return begin(row,channel);
}

template <class PixTyp>
inline typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::end(int row, int channel) const {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return (pChannel_[channel]->end(row));
}
	
template <class PixTyp>
inline typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::end() const {
  assert(pChannel_[0]!=0);
  return (pChannel_[0]->end());
}
	
template <class PixTyp>
inline typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::endChannel(int channel) const {
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  return (pChannel_[channel]->end());
}

template <class PixTyp>
inline const void* ImageT<PixTyp>::endVoid(int row, int channel) const {
  return end(row,channel);
}
	
template <class PixTyp>
inline const void* ImageT<PixTyp>::endVoid() const {
  return end();
}
	
template <class PixTyp>
inline const void* ImageT<PixTyp>::constBeginVoid(int row, int channel) const {
  return constBegin(row,channel);
}
	
/** retrieve the value of an matrix element
    usage: \code PixTyp x = A.getFloat( 3, 5 ); \endcode */
template <class PixTyp>
inline PixTyp ImageT<PixTyp>::get(int x, int y, int channel, PixTyp neutral) const {
  assert(channel>=0);
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  if ((x>=0) && (x<sizeX_) && (y>=0) && (y<sizeY_))
    return constBegin(y,channel)[x];
  else return neutral;
}

template <class PixTyp>
inline double ImageT<PixTyp>::getFloat(int x, int y, int channel, double neutral) const {
  return get(x,y,channel,PixTyp(neutral));
}
	

/** assign and retrieve matrix values
    usage: \code PixTyp x = A( 1, 5 ); A( 3, 7 ) = 5; \endcode */
template <class PixTyp>
inline PixTyp& ImageT<PixTyp>::operator () (int x, int y, int channel) {
  assert(channel>=0);
  assert(channel<noChannels_);
  assert(pChannel_[channel]!=0);
  assert(x>=0);
  assert(x<sizeX_);
  assert(y>=0);
  assert(y<sizeY_);
  return begin(y,channel)[x];
}

/** assign and retrieve matrix values
    usage: \code PixTyp x = A( 5 ); A( 3 ) = 5; \endcode
    \param nr number of element
    \return reference to an element */
template <class PixTyp>
inline PixTyp& ImageT<PixTyp>::operator () (unsigned int n) {
  assert(pChannel_[0]!=0);
  assert(n<nSize_);
  return begin()[n];
}

/** pointer to row y
    \param y row number */
template <class PixTyp>
inline typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::operator [] (int y) {
  assert(pChannel_[0]!=0);
  assert(y>=0);
  assert(y<sizeY_);
  return begin(y);
}

/** Assign value to matrix element, check for legal row and column index
    usage: \code  A.SetElement( 3, 4, 4444 );       \endcode
    \param x x of element (column)
    \param y y of element (row)
    \param val value to set element to */
template <class PixTyp>
inline void ImageT<PixTyp>::set(int x, int y, PixTyp val, int channel, bool clip) {
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
inline void ImageT<PixTyp>::setFloat(int x, int y, double val, int channel, bool clip) {
  set(x,y,PixTyp(val),channel,clip);
}

template <class PixTyp>
inline double ImageT<PixTyp>::getFloat(const void *it) const {
	return (double)(*(PixTyp*)it);
}

template <class PixTyp>
inline void ImageT<PixTyp>::setFloat(void* it, double val) {
	*(PixTyp*)it=(PixTyp)val;
}

template <class PixTyp>
inline void ImageT<PixTyp>::fillRow(void *it, int startX, int endX, double val, int channel) {
  PixTyp* ptr=(PixTyp*)it;
  PixTyp* eptr=ptr+endX;
  ptr+=startX;
  PixTyp v=PixTyp(val);
  for (;ptr<=eptr;ptr++) {
    *ptr=v;
  }
}

template <class PixTyp>
inline void* ImageT<PixTyp>::nextCol(const void*& ptr, int offset) const {
  return ((PixTyp*)ptr)+offset;
}

template <class PixTyp>
inline void* ImageT<PixTyp>::nextCol(const void*& ptr) const {
  return ((PixTyp*)ptr)+1;
}

template <class PixTyp>
inline void* ImageT<PixTyp>::prevCol(const void*& ptr) const {
	return ((PixTyp*)ptr)-1;
}

template <class PixTyp>
inline void* ImageT<PixTyp>::nextRow(const void*& ptr) const {
	return ((PixTyp*)ptr)+sizeX();
}

template <class PixTyp>
inline void* ImageT<PixTyp>::prevRow(const void*& ptr) const {
	return ((PixTyp*)ptr)-sizeX();
}



/** assignment of matrix values (initialization) */
template <class PixTyp>
inline ImageT<PixTyp>& ImageT<PixTyp>::operator = (const PixTyp rval) {
  for (int c=0; c<noChannels_; c++) {
    assert(pChannel_[c]!=0);
    Iterator w=begin(0,c);
    for(unsigned int i = nSize_ - 1; i; i--, w ++)
      *w = rval;
    *w = rval;
  }	
  return *this;
}


//------------------------------------ Tool functions -----------------------

/** Maximum element value of a ImageT
    usage: \code  PixTyp xmax = A.Max( channel=0 ); PixTyp xmax = A.Max( x, y, channel=0 ); \endcode */
template <class PixTyp>
inline double ImageT<PixTyp>::matrixMax(int& x, int& y, int channel) {
#if 0
  if(nSize_ < 0)
    return *pMatData[0];
#endif

  ConstIterator index = constBegin(0, channel);
  PixTyp max = *index;
  unsigned int e = 0;

  for(unsigned int i = 1; i < nSize_; i++) {
    if (max < *index) {
      max = *index;
      e = i;
    }
    ++ index;
  }

  y = e / sizeX_;
  x = e % sizeX_;

  return max;
}

/** Minimum of a ImageT
    usage: \code PixTyp xmin = A.Min( channel=0 ); \endcode */
template <class PixTyp>
inline double ImageT<PixTyp>::matrixMin(int& x, int& y, int channel) {
#if 0
  if(nSize_ < 0)
    return *(begin());
#endif

  ConstIterator index = constBegin(0, channel);
  PixTyp min = *index;
  unsigned int e = 0;

  for(unsigned int i = 1; i < nSize_; i++) {
    if (min > *index) {
      min = *index;
      e = i;
    }
    ++ index;
  }

  y = e / sizeX_;
  x = e % sizeX_;

  return min;
  }

/** allocation of memory and initialization of pointers, internal use only */
template <class PixTyp>
inline void ImageT<PixTyp>::alloc( int x, int y, int noChannels ) {
  for (int c=0; c<noChannels_; c++)
    if (pChannel_[c]) pChannel_[c]=pChannel_[c]->unlink();
  noChannels_=noChannels;
  for (int c=0; c<noChannels_; c++) {
    if( x < 1 || y < 1 ) {
      sizeY_ = sizeX_ = nSize_ = 0;
    }
    else {
      sizeX_ = x;
      sizeY_ = y;
      nSize_ = (size_t) sizeX_ * sizeY_;
    }
    pChannel_[c]=new Array2DT<PixTyp>(sizeX_, sizeY_);
  }
}

/** initialization; common function for initialization; for internal use only
    usage: \code Initialize( x, y ) \endcode \endcode */
template <class PixTyp>
inline void ImageT<PixTyp>::initialize( int x, int y, int noChannels ) {
  noChannels_=noChannels;
  for (int c=0; c<noChannels_; c++) pChannel_[c]=0;
  alloc( x, y, noChannels_ );
  imageType_ = _UNKNOWN;
}

/** deinitialization; common function for deinitialization (of pointer only !!) */
template <class PixTyp>
inline void ImageT<PixTyp>::deinitialize(void) {
  if (noChannels_==0) return;
  for (int c=0; c<noChannels_; c++) {
    if( pChannel_[c] == 0 ) continue;
    pChannel_[c]=pChannel_[c]->unlink();
  }
}

/** prepare assignments; Preparing an assignment by adjusting "this" */
template <class PixTyp>
inline void ImageT<PixTyp>::resize(int rx, int ry, int noChannels) {
  if( sizeY_ != ry || sizeX_ != rx || noChannels_ != noChannels ) {
    deinitialize();
    alloc( rx, ry, noChannels );
  }
}

template <class PixTyp>
inline void ImageT<PixTyp>::fill(double value) {
	*this=(PixTyp)value;
}

/** Returns the size of one element */
template <class PixTyp>
inline int ImageT<PixTyp>::elemSize()
{
  return sizeof(PixTyp);
}

/** set a pixel  */
template <class PixTyp>
inline void ImageT<PixTyp>::setInt(void *ptr, int val) {
  PixTyp *p=(PixTyp*)ptr;
  *p=PixTyp(val);
}

/** set a pixel  */
template <class PixTyp>
inline void ImageT<PixTyp>::setInt(int x, int y, int val, int channel) {
  Iterator p=begin(y,channel)+x;
  setInt(p,val);
}

/** set a pixel  */
template <class PixTyp>
inline int ImageT<PixTyp>::getInt(const void *ptr) const {
  PixTyp *p=(PixTyp*)ptr;
  return int(*p);
}

/** set a pixel  */
template <class PixTyp>
inline int ImageT<PixTyp>::getInt(int x, int y, int channel) const {
  ConstIterator p=constBegin(y,channel)+x;
  return getInt(p);
}

template <class PixTyp>
inline int ImageT<PixTyp>::getInt(int x, int y, int channel, int neutral) const {
  return int(get(x,y,channel,neutral));
}
	
/** merge 'this' image with 'img'. The labels 'img_label' of 'img'
  * are insert (overlayed) in 'this' with the value 'new_label' */
template <class PixTyp>
inline void ImageT<PixTyp>::merge(ImageBase& image, double img_label, double new_label) {
	ImageT<PixTyp>& img=(ImageT<PixTyp>&)image;
	PixTyp label=(PixTyp)img_label;
	PixTyp newLabel=(PixTyp)new_label;
  PixTyp *p_img = img.begin();
  PixTyp *p_data = this->begin();
  int size = this->sizeImage();
  if (size == img.sizeImage())
    for (int i=0; i<=size; i++) {
      if (*p_img == label) *p_data = newLabel;
      p_img++;
      p_data++;
    }
}

/** extracts one band from a multi-band image to a new single-band image
     \param band 0=red 1=green 2=blue, enum type can be used (e.g. ImageT<GA_NOTYPE>::red) */

template <class PixTyp>
inline void ImageT<PixTyp>::getChannel(ImageBase& resultImg, int channel)
{
  ImageT<PixTyp>& pic=(ImageT<PixTyp>&)resultImg;
	pic.resize(sizeX(), sizeY());

  pic.pChannel_[0] = pChannel_[channel]->shallowCopy();
}



/**
*/
template <class PixTyp>
inline void ImageT<PixTyp>::maxValue(ImageBase &inImg, float value) {
  ImageT<PixTyp>& in=(ImageT<PixTyp>&)inImg;
  PixTyp val = (PixTyp)value;
  Iterator elem = in.begin(0);
  int size = in.sizeX()*in.sizeY();

  for (int i = 0; i < size; i++) {
      if(*elem > val) *elem = val;
      elem++;
    }
}

/**
*/
template <class PixTyp>
inline void ImageT<PixTyp>::minValue(ImageBase &inImg, float value) {
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
inline bool ImageT<PixTyp>::read(const char *filename) {
	FILE *fp=fopen(filename,"r");
  if (!fp) return(false);
  read(fp);
  fclose(fp);
	return(true);
}

template <class PixTyp>
inline bool ImageT<PixTyp>::read(FILE *fp) {
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
	typeImage(type);
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

template <class PixTyp>
inline bool ImageT<PixTyp>::write(const char *filename, int channel, const char* comment) {
	FILE *fp=fopen(filename,"w");
  if (!fp) return(false);	
  write(fp, channel, comment);
  fclose(fp);
	return(true);
}

template <class PixTyp>
inline bool ImageT<PixTyp>::write(FILE *fp, int channel, const char* comment) {
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
	
	switch (typeImage()) {
		case _PPM: {
		  PFM3Byte ppmbuffer;
		  ppmbuffer.r=(unsigned char*)constBegin(0,0);
 	 		ppmbuffer.g=(unsigned char*)constBegin(0,1);
			ppmbuffer.b=(unsigned char*)constBegin(0,2);
#if 0		
      if (geoNorth()!=geoSouth() && geoWest()!=geoEast())
        pfm_geo_set(geoWest(),geoNorth(),geoEast(),geoSouth());
      if (comment)
        pfm_comment_set(comment);
  		pfm_writepfm_type(fp,&ppmbuffer,sizeX(),sizeY(),1,-1,PFM_3BYTE);

vv war schon vor mir, jlnr vv
		
 	    int sizeX_ = sizeX(), sizeY_ = sizeY();
 		  pixel *row = (pixel *)new pixel[sizeX_], *elem_pix;
   		
   		ppm_writeppminit(fp, sizeX_, sizeY_, PPM_MAXMAXVAL, 0);
     		
   		for (int y = 0; y < sizeY_; ++y) {
   			ConstIterator pChR=constBegin(y,0);
 	 			ConstIterator pChG=constBegin(y,1);
				ConstIterator pChB=constBegin(y,2);
     		elem_pix = row;
       		
   		  for (int x = 0; x < sizeX_; ++x, ++elem_pix, ++pChR, ++pChG, ++pChB) {
           PPM_ASSIGN(*elem_pix, (pixval)*pChR, (pixval)*pChG, (pixval)*pChB);
   		  }
   		  ppm_writeppmrow(fp, row, sizeX_, PPM_MAXMAXVAL, 0);
  	  }
   	  delete row;
#endif
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
//  		pfm_writepfm_type(fp,constBegin(),sizeX(),sizeY(),minValue(),maxValue(),storageType);
  		// Let the pfm library calculate min and max
#if 0
      if (geoNorth()!=geoSouth() && geoWest()!=geoEast())
        pfm_geo_set(geoWest(),geoNorth(),geoEast(),geoSouth());
      if (comment)
        pfm_comment_set(comment);
#endif
  		pfm_writepfm_type(fp,constBegin(0,channel),sizeX(),sizeY(),1,-1,storageType);
	}
	return true;
}

} // namespace Ga
