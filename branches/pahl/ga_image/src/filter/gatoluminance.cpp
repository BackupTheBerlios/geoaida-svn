/****************************************************************************
                          gatoluminance.cpp  -  GeoAIDA algorithm implementation
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
  template<typename PixType>
  void toLuminanceT(const ImageBase& in, ImageT<float>& out)
  {
    assert(in.noChannels()==3);
  
    const ImageT<PixType>& pic = dynamic_cast<const ImageT<PixType>&>(in);

    // TODO: How comes the result image's type is only set in this case...?!
    if (pic.fileType()==_PPM)
      out.setFileType(_PGM);

    ImageT<float>::Iterator elem = out.begin();
    typename ImageT<PixType>::ConstIterator pChR=pic.constBegin(0,0);
    typename ImageT<PixType>::ConstIterator pChG=pic.constBegin(0,1);
    typename ImageT<PixType>::ConstIterator pChB=pic.constBegin(0,2);
    for (int i = 0; i < pic.noPixels(); ++i, ++elem, ++pChR, ++pChG, ++pChB) {
      *elem = (*pChR * 0.3 + *pChG * 0.59 + *pChB * 0.11 + 0.5);
    }
  }

  Image toLuminance(const Image& img)
  {
    Image result(typeid(float), img.sizeX(), img.sizeY());
    ForEachTypeDo(img.typeId(), toLuminanceT, (*img.pImage(), *static_cast<ImageT<float>*>(result.pImage())));
    return result;
  }
}
