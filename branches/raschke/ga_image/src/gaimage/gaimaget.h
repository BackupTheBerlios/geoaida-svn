/***************************************************************************
                          GaImageT.h  -  geoAIDA template image class
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
 * $Source: /data/cvs/ga_image/ga_image/gaimage/gaimaget.h,v $
 * $Revision: 1.14 $
 * $Date: 2003/05/28 06:35:02 $
 * $Author: pahl $
 * $Locker:  $
 */

#ifndef __GA_IMAGET_H
#define __GA_IMAGET_H

// <cmath> and <climits> would be nicer; however, all the code only uses
// things in the global namespace. Assuming the new-style headers place
// their contents in the global namespace is not portable.
#include <math.h>
#include <limits.h>

#ifdef GCC29
#include <float.h>
#endif

extern "C" {
#include <pnm.h>
#include <pfm.h>
}
#define GA_MAX_CHANNELS 10
#include <assert.h>
#include "gadefines.h"
#include "gaimagebase.h"
#include "gaarray2dt.h"

namespace Ga {

/** \class ImageT
    \brief defines an template class for operations between matrices and vectors
    and for handling geo-related images
    */

  template <class PixTyp>
  class ImageT : public ImageBase
  {
  public:
    enum { red, green, blue };
    typedef PixTyp* Iterator;
    typedef const PixTyp* ConstIterator;
    
    //--------------------- constructors for images -----------------------------
    /** default constructor, defines a picture for later assignment */
    ImageT();
    /** copy constructor, definition and initialization from a picture;
	usage: \code ImageT A = B; ImageT A = B + 2 * C; return A; \endcode */
    ImageT(const ImageT<PixTyp>& rval);
    virtual ImageBase* newObject(int x_size, int y_size, int channels=1);
    virtual ImageBase* copyObject();
    
    /** create picture from matrix
	usage: \code ga_pix A( y ); \endcode */
    ImageT(const ImageBase& imgIn);
    
    /** size a picture and initialize it to zero; must then be assigned other values (for a picture of known dimension)
	usage: \code ImageT A( 4, 5 ); A( 1, 1 ) = 3; A( 1, 2 ) = 7; \endcode
	\param x x-size, number of columns
	\param y y-size, number of rows */
    ImageT(int x, int y, int noChannels=1);
    
    /** create a picture from an array
	usage: \code int w[]={1, 2, 3, 4, 5, 6}; ImageT A( 2, 3, w ); \endcode
	\param x x-size, number of columns
	\param y y-size, number of rows */
    ImageT (int x, int y, PixTyp *vec);
    
    /** create a picture from formatted File (ASCII)
	usage: \code ImageT A("MAT.DAT"); \endcode
	\param file filename */
    ImageT(char *file);
    
    /** create a subpicture with identical beginning (at [1,1])
	usage: \code ImageT B( 4, 5, A ); \endcode
	\param x new x-size, number of columns
	\param y new y-size, number of rows
	\param imgIn input picture */
    ImageT(int x, int y, const ImageT& imgIn);
    
    /** create subpicture starting at arg3, arg4 with size arg1 x arg2
	usage: \code ImageT B( m, n, i, j, A ); \endcode
	\param x new x-size, number of columns
	\param y new y-size, number of rows
	\param x0 starting offset x
	\param y0 starting offset y
	\param imgIn input picture */
    ImageT(int x, int y, int x0, int y0, const ImageT& imgIn);
    
    /** destructor */
    virtual ~ImageT(void);
    virtual void partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height);
    virtual const class std::type_info& typeId() const;
    int noChannels() const;
    void rmChannel();
    void addChannel(const ImageBase& rval, int channel=0);
    
    //------------------------------ Access of matrix data ----------------------
    /** clear matrix */
    void clear();
    /** treats pointer this as if were a long integer */
    unsigned long hash(void) const;
    /** return pointer to the beginning of data
	usage: \code Iterator dp = M.begin(row); \endcode */
    Iterator begin(int row=0, int channel=0);
    /** return pointer to the beginning of data
	usage: \code PixTyp * dp = M.data(row); \endcode */
    PixTyp* data(int row =0, int channel = 0);
    void* dataArray(int channel=0);
    PixTyp** dataArrayT(int channel=0);
    ConstIterator constBegin(int row=0, int channel=0) const;
    void* beginVoid(int row=0, int channel=0);
    ConstIterator end(int row, int channel=0) const;
    ConstIterator end() const;
    ConstIterator endChannel(int channel) const;
    const void* endVoid(int row, int channel=0) const;
    const void* endVoid() const;
    const void* constBeginVoid(int row=0, int channel=0) const;
    /** retrieve the value of an matrix element
	usage: \code PixTyp x = A.GetElement( 3, 5 ); \endcode */
    PixTyp get(int x, int y, int channel=0, PixTyp neutral=0) const;
    double getFloat(int x, int y, int channel, double neutral=0) const;
    virtual double getFloat(const void *it) const;
    virtual void setFloat(void* it, double val);
    virtual void fillRow(void *it, int startX, int endX, double val, int channel=0);
    virtual void* nextCol(const void*& ptr, int offset) const;
    virtual void* nextCol(const void*& ptr) const;
    virtual void* prevCol(const void*& ptr) const;
    virtual void* nextRow(const void*& ptr) const;
    virtual void* prevRow(const void*& ptr) const;
    /** assign and retrieve matrix values
	usage: \code PixTyp x = A( 1, 5 ); A( 3, 7 ) = 5; \endcode */
    PixTyp& operator () (int x, int y, int channel=0);
    /** assign and retrieve matrix values
	usage: \code PixTyp x = A( 5 ); A( 3 ) = 5; \endcode
	\param nr number of element
	\return reference to an element */
    PixTyp& operator () (unsigned int n);
    /** pointer to row y
	\param y row number */
    Iterator operator [] (int y);
    /** Assign value to matrix element, check for legal row and column index
	usage: \code  A.SetElement( 3, 4, 4444 );       \endcode
	\param x x of element (column)
	\param y y of element (row)
	\param val value to set element to */
    void set(int x, int y, PixTyp val, int channel=0, bool clip=false);
    void setFloat(int x, int y, double val, int channel=0, bool clip=false);
    /* set data without copying. Ther must be noChannels data-pointer in the ellipse */
    void setData(int x, int y, int noChannels, ...);
    void setData(int x, int y, void* initvalues);
    /** assignment of a matrix
	usage: \code  A=B; A=C+D; A=B=C+D; \endcode */
    ImageT<PixTyp>& operator = (const ImageT<PixTyp>& rval);
    /** assignment of matrix values (initialization) */
    ImageT& operator = (const PixTyp rval);

  //------------------------------------ Tool functions -----------------------

  double matrixMax(int& x, int& y, int channel=0);
  double matrixMin(int& x, int& y, int channel=0);
  int elemSize();
  void resize(int rx, int ry, int noChannels=1);

  /** set a pixel */
  void setInt(void *ptr, int val);
  void setInt(int x, int y, int val, int channel=0);

  /** set a pixel */
  int getInt(const void *ptr) const;
  int getInt(int x, int y, int channel=0) const;
  int getInt(int x, int y, int channel, int neutral) const;


  /** merge 'this' image with 'img'. The labels 'img_label' of 'img'
  	* are insert (overlayed) in 'this' with the value 'new_label' */
  void merge(ImageBase& image, double img_label, double new_label);

 /** extracts one band from a rgb-color image to a new single-band image
      \param band 0=red 1=green 2=blue, enum type can be used (e.g. ImageT<GA_NOTYPE>::red) */
  void getChannel(ImageBase& pic, int channel=0);

  void minValue(ImageBase &in, float value);
  void maxValue(ImageBase &in, float value);

  bool read(FILE *fp);
	bool read(const char *filename);
  bool write(FILE *fp, int channel=0, const char* comment=0);
	bool write(const char *filename, int channel=0, const char* comment=0);

protected:
  /** allocation of memory and initialization of pointers, internal use only */
  void alloc( int x, int y, int noChannels );
  /** initialization; common function for initialization; for internal use only
      usage: \code Initialize( x, y ) \endcode \endcode */
  void initialize( int x, int y, int noChannels );
  /** deinitialization; common function for deinitialization (of pointer only !!) */
  void deinitialize(void);
  /** prepare assignments; Preparing an assignment by adjusting "this" */
  void fill(double value);
  Array2DT<PixTyp> *pChannel_[GA_MAX_CHANNELS];
  int noChannels_;
};

} // namespace Ga

#include "gaimaget.hpp"

#endif        // __GA_IMAGET_H
