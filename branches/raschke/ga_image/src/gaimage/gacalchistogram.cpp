/****************************************************************************
                          gacalchistogram.cpp  -  GeoAIDA algorithm implementation
                             -------------------
    begin                : 2007-06-21
    copyright            : (C) 2007 TNT, Uni Hannover
    authors              : moved around by Julian Raschke
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
#include "gaimaget.h"

namespace Ga
{
  template <class PixType>
  inline void calcHistogramT(const ImageBase& in, ImageT<int>& result, double min, double max, int n, int channel) 
  {
    const ImageT<PixType>& img = dynamic_cast<const ImageT<PixType>&>(in);
  	typename ImageT<PixType>::ConstIterator elem = img.constBegin(0,channel);
  	ImageT<int>::Iterator pResult=result.begin();
  	double f=1/(max-min);

  	for (int i = 0; i < img.sizeImage(); ++i, ++elem) {
  	 	int index=int((*elem-min)*f*n);
    	if (index<0 || index>=n) 
    	  continue;
      pResult[index] ++;
    }
  }

  Image calcHistogram(const Image& img, double min, double max, int n, int channel)
  {
    Image result(typeid(int), n, 1);
    result.pImage()->typeImage(_PFM_SINT);
    ForEachTypeDo(img.typeId(), calcHistogramT, (*img.pImage(), *static_cast<ImageT<int>*>(result.pImage()),
                                                 min, max, n, channel));
    return result;
  }
}
