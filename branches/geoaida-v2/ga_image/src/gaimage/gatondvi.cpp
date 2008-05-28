/****************************************************************************
                          gatondvi.cpp  -  GeoAIDA algorithm implementation
                             -------------------
    begin                : 2007-07-09
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
  template<typename PixType>
  void toNDVIT(const ImageBase& in, ImageT<float>& out)
  {
    assert(in.noChannels()==3);
  
    const ImageT<PixType>& pic = dynamic_cast<const ImageT<PixType>&>(in);
    
    // TODO: How comes the result image's type is only set in this case...?!
    if (pic.fileType()==_PPM)
      out.setFileType(_PGM);

    ImageT<float>::Iterator elem = out.begin();
    typename ImageT<PixType>::ConstIterator pChIR = pic.constBegin(0,0);
    typename ImageT<PixType>::ConstIterator pChR = pic.constBegin(0,1);
    for (int i = 0; i < in.noPixels(); ++i, ++elem, ++pChIR, ++pChR) {
      *elem = (float) ( (double)((double) *pChIR - (double) *pChR)/
			  ((double) *pChIR + (double) * pChR) );
    }
  }
  
  Image toNDVI(const Image& img)
  {
    Image result(typeid(float), img.sizeX(), img.sizeY());
    ForEachTypeDo(img.typeId(), toNDVIT, (*img.pImage(), *static_cast<ImageT<float>*>(result.pImage())));
    return result;
  }
}
