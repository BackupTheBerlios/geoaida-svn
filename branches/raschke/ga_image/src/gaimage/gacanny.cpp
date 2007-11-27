/****************************************************************************
                          gacanny.cpp  -  GeoAIDA algorithm implementation
                             -------------------
    begin                : 2007
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

#include "gaalgo.h"
#include <vector>
#include <cmath>
#include <algorithm>

// Local helpers.

namespace {
  using namespace Ga;

  Image normalizeImage(const Image& source) {
    // Uses Image::Iterator just because it's there.
    // Should be optimized.
    
    int sizeX = source.sizeX(), sizeY = source.sizeY();
    Image result(typeid(float), sizeX, sizeY);
    
  	double min = source.findMinValue();
  	double range = source.findMaxValue() - min;
  	
    unsigned num = 0;
  	
    Image::ConstIterator in = source.constBegin();
    Image::ConstIterator end = source.constEnd();
    for (Image::Iterator out = result.begin(); in != end; ++in, ++out) {
      double pix = *in;
      pix -= min;
      pix /= range;
      *out = pix;
      ++num;
    }
    
  	return result;
  }

  const int SOBEL_X[9]   = {  1,  0, -1,    2,  0, -2,    1,  0, -1 };
  const int SOBEL_Y[9]   = {  1,  2,  1,    0,  0,  0,   -1, -2, -1 };

  // Helper for convolution:
  // Adjust x/y when going over edge
  float getPixel(const Image &source, int x, int y) {
    if (x < 0)
      x = 0;
    else if (x >= source.sizeX())
      x = source.sizeX() - 1;
    if (y < 0)
      y = 0;
    else if (y >= source.sizeY())
      y = source.sizeY() - 1;
    return source.getPixel(x, y, 0);
  }

  // Helper for convolution:
  // Adjust val when outside 0..1
  void setPixel(Image &dest, int x, int y, float val) {
    if (val < 0)
      val = 0;
    else if (val > 1)
      val = 1;
    dest.setPixel(x, y, val, 0);
  }

  Image convolve(const Image &source, const int (&coefficients)[9], double factor, double base) {
    int sizeX = source.sizeX(), sizeY = source.sizeY();
    Image result(typeid(float), sizeX, sizeY);

    for (int x = 0; x < sizeX; ++x)
    for (int y = 0; y < sizeY; ++y) {
      float accum = base;
      for (int i = 0; i < 9; ++i) {
        int relX = (i % 3) - 1;
        int relY = (i / 3) - 1;
        accum += getPixel(source, x + relX, y + relY) * coefficients[i] * factor;
      }

      setPixel(result, x, y, accum);
    }

    return result;
  }

  Image combineIntensities(const Image &sobelX, const Image &sobelY) {
    int sizeX = sobelX.sizeX(), sizeY = sobelX.sizeY();
    Image result(typeid(float), sizeX, sizeY);

    for (int x = 0; x < sizeX; ++x)
    for (int y = 0; y < sizeY; ++y) {
      float gx = sobelX.getPixel(x, y, 0);
      float gy = sobelY.getPixel(x, y, 0);
      result.setPixel(x, y, fabs(gx - 0.5) + fabs(gy - 0.5), 0);
    }

    return result;
  }

  const float VERTICAL = 1.0/2;
  const float HORIZONTAL = 1.0/4;
  const float SLASH = 1.0/8;
  const float BACKSLASH = 1.0/16;

  Image calcAngles(const Image &sobelX, const Image &sobelY) {
    int sizeX = sobelX.sizeX(), sizeY = sobelX.sizeY();
    Image result(typeid(float), sizeX, sizeY);

    for (int x = 0; x < sizeX; ++x)
    for (int y = 0; y < sizeY; ++y) {
      double gx = sobelX.getPixel(x, y, 0) - 0.5;
      double gy = sobelY.getPixel(x, y, 0) - 0.5;
      double res;
      if (gx == 0)
        if (gy == 0)
        res = 0;
      else
        res = M_PI / 2;
      else
        res = atan(gy / gx);

      if (res < M_PI / 8 && res > -M_PI / 8)
        result.setPixel(x, y, VERTICAL, 0);
      else if (res > 3 * M_PI / 8 || res < -3 * M_PI / 8)
        result.setPixel(x, y, HORIZONTAL, 0);
      else if (res < 0)
        result.setPixel(x, y, BACKSLASH, 0);
      else
        result.setPixel(x, y, SLASH, 0);
    }

    return result;
  }

  Image findLocalMaxs(const Image &intensities, const Image &angles) {
    int sizeX = intensities.sizeX(), sizeY = intensities.sizeY();
    Image result(typeid(float), sizeX, sizeY);

    for (int x = 0; x < sizeX; ++x)
    for (int y = 0; y < sizeY; ++y) {
      double localIntensity = intensities.getPixel(x, y, 0);

      if (intensities.getPixel(x-1, y-1) > localIntensity && angles.getPixel(x, y, 0) == SLASH ||
        intensities.getPixel(x+0, y-1) > localIntensity && angles.getPixel(x, y, 0) == HORIZONTAL ||
        intensities.getPixel(x+1, y-1) > localIntensity && angles.getPixel(x, y, 0) == BACKSLASH ||

        intensities.getPixel(x-1, y+0) > localIntensity && angles.getPixel(x, y, 0) == VERTICAL ||
        intensities.getPixel(x+1, y+0) > localIntensity && angles.getPixel(x, y, 0) == VERTICAL ||

        intensities.getPixel(x-1, y+1) > localIntensity && angles.getPixel(x, y, 0) == BACKSLASH ||
        intensities.getPixel(x+0, y+1) > localIntensity && angles.getPixel(x, y, 0) == HORIZONTAL ||
        intensities.getPixel(x+1, y+1) > localIntensity && angles.getPixel(x, y, 0) == SLASH)
        localIntensity = 0;

      result.setPixel(x, y, localIntensity, 0);
    }

    return result;
  }

  bool inImage(const Image &img, int x, int y) {
    return !(x < 0 || x >= img.sizeX() || y < 0 || y >= img.sizeY());
  }

  static const float REQUIRED_DIR[9] = {
    BACKSLASH, VERTICAL, SLASH,
    HORIZONTAL, -1, HORIZONTAL,
    SLASH, VERTICAL, BACKSLASH
  };

  bool trace(Image &result, const Image &intensities, const Image &angles, int x, int y, double threshold) {
    result.setPixel(x, y, 255, 0);

    for (int i = 0; i < 9; ++i) {
      if (i == 4)
        continue;
      int scanX = x + i % 3 - 1;
      int scanY = y + i / 3 - 1;
      if (inImage(result, scanX, scanY) && 
        intensities.getPixel(scanX, scanY, 0) >= threshold &&
        result.getPixel(scanX, scanY) != 255 &&
        angles.getPixel(x, y, 0) == REQUIRED_DIR[i])
        trace(result, intensities, angles, scanX, scanY, threshold);
    }
  }

  Image applyHysteresis(const Image &intensities, const Image &angles,
      double low, double high) {
    int sizeX = intensities.sizeX(), sizeY = intensities.sizeY();
    Image result(typeid(unsigned char), sizeX, sizeY, 3); // <- TODO

    for (int x = 0; x < sizeX; ++x)
      for (int y = 0; y < sizeY; ++y)
        result.setPixel(x, y, 0, 0);

    for (int x = 0; x < sizeX; ++x)
      for (int y = 0; y < sizeY; ++y)
        if (intensities.getPixel(x, y, 0) >= high)
          trace(result, intensities, angles, x, y, low);

    return result;
  }  
}

Ga::Image Ga::canny(const Image& img, double lowThreshold, double highThreshold) {
  Image input = img;
    
	// The Canny algorithm is designed to work on grayscale images, so make sure we have one.
	if (input.noChannels() != 1) {
		input = toLuminance(input);
	}
	
	// Normalize the image so every pixel lies in 0..1.
	input = normalizeImage(input);
	
  Image sobelX = convolve(input, SOBEL_X, 1.0/4, 0.5);
  Image sobelY = convolve(input, SOBEL_Y, 1.0/4, 0.5);
  
  Image intensities = combineIntensities(sobelX, sobelY);
  Image angles = calcAngles(sobelX, sobelY);

  Image localMaxs = findLocalMaxs(intensities, angles);
  Image canny = applyHysteresis(localMaxs, angles, lowThreshold, highThreshold);
  // TODO
  canny.setFileType(_PPM);
  return canny;
}
