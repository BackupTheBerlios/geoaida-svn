/****************************************************************************
                          gaalgo.h  -  GeoAIDA algorithm declarations
                             -------------------
    begin                : 2007-06-21
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

#ifndef __GAALGO_H
#define __GAALGO_H

#include "gaimage.h"

namespace Ga
{
  /*
  Implementation of the Canny edge detection algorithm.
  
  Creates a same-sized image from the supplied one, where detected edges are
  drawn in white on a black background. The high threshold controls how
  strong (0..1) an edge must be to be detected; it will then be traced for as
  long as it is stronger than the low threshold.
  
  The implementation consists of separate phases, all of them O(pixels).
  
  Application note: Because the algorithm cannot trace directly adjacent,
  parallel lines, it might be of advantage to enlargen the image if the
  resolution is too low.
  
  Notes: This includes the sobel operator to approximate the gradient. This
  could probably be merged into gagradient.
  */
  Image canny(const Image& img, double lowThreshold, double highThreshold);
  
  // Convert RGB color image to luminance (single-channel double) image.
  Image toLuminance(const Image& img);
  
  // Convert RGB color image to hue/saturation/value image.
  Image toHSV(const Image& img);
  
  // Generate histogram as a narrow int image (width n, height 1).
  Image calcHistogram(const Image& img, double min, double max, int n, int channel = 0);
  
  // Converts a 3-band image (irrgb-color) to a new single band NDVI image.
  Image toNDVI(const Image& img);
  
#if 0
/** resample the current image to size (x, y) */
template <class PixTyp>
inline void ImageT<PixTyp>::resample(ImageBase& resImg, int nx, int ny)
{
  ImageT<PixTyp>& result=(ImageT<PixTyp>&)resImg;
  result.resize(nx, ny, noChannels());
  result.typeImage(typeImage());

  int nc, nr;
  int x1, x2, y1, y2;
  double x, y;

  double cfac=1.0*(sizeX_ - 1) / (nx - 1);
  double rfac=1.0*(sizeY_ - 1) / (ny - 1);

  double p11, p12, p21, p22;
  for (int c=0; c<noChannels(); c++) {
    Iterator elem = result.begin(0,c);

    for (nr = 0; nr < ny; ++nr)
      for (nc = 0; nc < nx; ++nc) {
	x=nc*cfac; y=nr*rfac;
	x1 = (int)x; y1 = (int)y;
	x2 = x1 + 1; y2 = y1 + 1;
	
	// range checking is done in getFloat() !
	     p11 = getFloat(x1, y1, c); p12 = getFloat(x1, y2, c);
	p21 = getFloat(x2, y1, c); p22 = getFloat(x2, y2, c);
	
	if ((typeid(PixTyp)==typeid(float)) || (typeid(PixTyp)==typeid(float))) {		
	  int nan=0;
	  if (isnan(p11)) nan++;
	  if (isnan(p21)) nan++;
	  if (isnan(p12)) nan++;
	  if (isnan(p22)) nan++;
	  if (nan>=2) {
	    *elem=(PixTyp)NAN;
	  }
	  else if (nan==0)
	    *elem=(PixTyp)((y2-y)*((x2-x)*p11+(x-x1)*p21)
			   +(y-y1)*((x2-x)*p12+(x-x1)*p22));
	  else {
	    if (isnan(p11)) p11=p22+(p21-p22)+(p12-p22);
	    if (isnan(p12)) p12=p21+(p11-p21)+(p22-p21);
	    if (isnan(p21)) p21=p12+(p22-p12)+(p11-p12);
	    if (isnan(p22)) p22=p11+(p12-p11)+(p21-p11);
	    *elem=(PixTyp)((y2-y)*((x2-x)*p11+(x-x1)*p21)
			   +(y-y1)*((x2-x)*p12+(x-x1)*p22));
	  }
	}
	else { // typeid != float != double
		    *elem = (PixTyp)((y2-y)*((x2-x)*p11+(x-x1)*p21)+(y-y1)*((x2-x)*p12+(x-x1)*p22) + 0.5);
	}
	++elem;
      }
  }

}

/** resample the current image to size (x, y) using a nearest neighbour check */
template <class PixTyp>
inline void ImageT<PixTyp>::resampleNN(ImageBase& resImg, int nx, int ny)
{
  ImageT<PixTyp>& result=(ImageT<PixTyp>&)resImg;
  result.resize(nx, ny, noChannels());
  result.typeImage(typeImage());

  int x, y;

  double xfac = (double)sizeX_ / (double)nx;
  double yfac = (double)sizeY_ / (double)ny;

  for (int c=0; c<noChannels(); c++) {
    Iterator elem = result.begin(0,c);

    for (y = 0; y < ny; ++y)
      for (x = 0; x < nx; ++x) {
	*elem = get((int)((x + 0.5) * xfac), (int)((y + 0.5) * yfac),c);
	++elem;
      }
  }
}
/**
blow the label 'label' in image 'in'. The number of blow steps are given
by 'iterations'.
*/
template <class PixTyp>
inline void ImageT<PixTyp>::blow(ImageBase &inImg, int la, int iterations) {

  ImageT<PixTyp>& in=(ImageT<PixTyp>&)inImg;
  ImageT<PixTyp> tmp = in;
  PixTyp label = (PixTyp) la;

  int xs = in.sizeX();
  int ys = in.sizeY();

  while (iterations --) {
    if (ys>3) {//Y - Rand bearbeiten
      for (int y = 1; y < ys-1; y++)
        if (in.get(1, y) == label) {
          tmp.set(1, y+1, label);
          tmp.set(1, y-1, label);
        }
      if(xs>1)
      for (int y = 1; y < ys-1; y++)
        if (in.get(xs, y) == label) {
          tmp.set(xs-1, y+1, label);
          tmp.set(xs-1, y-1, label);
        }
    }
    if (xs>3) {//X - Rand bearbeiten
      for (int x = 1; x < xs-1; x++)
        if (in.get(x, 1) == label) {
          tmp.set(x+1, 1, label);
          tmp.set(x-1, 1, label);
        }
      if(ys>1)
      for (int x = 1; x < xs-1; x++)
        if (in.get(x, ys) == label) {
          tmp.set(x+1, ys-1, label);
          tmp.set(x-1, ys-1, label);
        }
    }
    for (int y = 1; y < ys-1; y ++) //restbild
      for (int x = 1; x < xs-1; x ++)
        if (in.get(x, y) == label) {
          tmp.set(x-1, y, label);
          tmp.set(x+1, y, label);
          tmp.set(x, y+1, label);
          tmp.set(x, y-1, label);
       }
    in = tmp;
  }
}

/** resample the current image to size (x, y) using a nearest neighbour check;
    this version uses a sophisticated class determination algorithm, which is
    especially useful for subsampling/shrinking an image, thus for enlargement */
template <class PixTyp>
inline void ImageT<PixTyp>::resampleNNplus(ImageBase& resImg, int nx, int ny)
{
  ImageT<PixTyp>& result=(ImageT<PixTyp>&)resImg;
  // calculate the scaling factors
       float xfac = (float)sizeX_ / (float)nx;
  float yfac = (float)sizeY_ / (float)ny;

  if (xfac < 1.0 || yfac < 1.0) {
      resampleNN(result, nx, ny);
      return;
  }

  // allocate the resized image
  result.resize(nx, ny, noChannels());
  result.typeImage(typeImage());

  // allocate memory for xfac * yfac possible classes, 2.0 is for safety ;-)
  const int window_size=(int)(xfac * yfac * 2.0);
  PixTyp *window_val = (PixTyp *)new PixTyp[window_size];
  float *window_count = new float[window_size];

  for (int ch=0; ch<noChannels(); ch++) {
    // get a pointer to the the image data
	 Iterator elem = result.begin(0,ch);
    PixTyp ptmp;

    int classmax, c, bleft, bright, btop, bbottom;
    float max, xfrac, yfrac, fx, fy;

    // sweep over the new image ...
	 for (int y = 0; y < ny; ++y)
	   for (int x = 0; x < nx; ++x) {
	     // ... and calculate all it's pixel values

	          // clear the class determination arrays
	          memset(window_val, 0, window_size * sizeof(PixTyp));
	          memset(window_count, 0, window_size * sizeof(float));
	          classmax = 0; max = 0.0;

	          // get all pixels covered by the low-res window (only useful for downsampling ...)
	          for (float dy = 0.0; dy < yfac; dy += 1.0) {
	            // do we pick pixel at the y-upper/-lower border of the window ?
	            btop = (dy == 0.0 ? 1 : 0);
	            bbottom = ((dy + 1.0) >= yfac ? 1 : 0);
	
	            for (float dx = 0.0; dx < xfac; dx += 1.0) {
	              // do we pick pixel at the x-left/-right border of the window ?
	              bleft = (dx == 0.0 ? 1 : 0);
	              bright = ((dx + 1.0) >= xfac ? 1 : 0);

	              // this is the floating point representation of the current pixel within the window ...
	              fx = (x * xfac + dx);
	              fy = (y * yfac + dy);
	
	              // ... get its value from the original image!
	              ptmp = get((int)fx, (int)fy, ch);
	
	              // how much overlapping area does the current pixel contribute to the window ?
	              // default: 1.0 pixel
	              xfrac = yfrac = 1.0;
	              // if we travel along the border the area is smaller:
	              if (bleft)
	                xfrac -= (fx - floor(fx));
	              if (bright)
	                xfrac -= (ceil(x * xfac + xfac) - (x * xfac + xfac));
	              if (btop)
	                yfrac -= (fy - floor(fy));
	              if (bbottom)
	                yfrac -= (ceil(y * yfac + yfac) - (y * yfac + yfac));
	
	              // is the current pixel value(class) already registered ?
	              for (c = 0; c <= classmax; c ++) {
	                // if yes then
	                if (window_val[c] == ptmp) {
	                  // increase the class weight by the contributed pixel area
		                assert(c<window_size);
	                  window_count[c] += xfrac * yfrac;
	                  break;
	                }
	              }
	              // if the value(class) isn't yet registered
	     if (c == classmax + 1) {
	       // put it into the list
		    assert(classmax<window_size);
	       window_val[classmax] = ptmp;
	       window_count[classmax] += xfrac * yfrac;
	       classmax ++;
	     }
	   }
  }

  // after we have completed the whole window we have to decide to
       // which class this new pixel belongs: find the maximum class value
       for (c = 0, max = 0.0; c <= classmax; c ++) {
	 if (max < window_count[c]) {
	   max = window_count[c];
	   // set the pixel to the maximum value
		*elem = window_val[c];
	 }
       }
  // and move to the next pixel in the new image
       ++elem;
}
}

delete window_val;
delete window_count;
}

#endif
  
}

#endif
