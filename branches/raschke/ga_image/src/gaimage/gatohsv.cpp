/****************************************************************************
                          gahsv.cpp  -  GeoAIDA algorithm implementation
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
  void toHSVT(const ImageBase& in, ImageT<float>& pic)
  {
    assert(in.noChannels()==3);
  
    const ImageT<PixType>& img = dynamic_cast<const ImageT<PixType>&>(in);
  
    ImageT<float>::Iterator pChH = pic.begin(0,0);
    ImageT<float>::Iterator pChS = pic.begin(0,1);
    ImageT<float>::Iterator pChV = pic.begin(0,2);
    typename ImageT<PixType>::ConstIterator pChR=img.constBegin(0,0);
    typename ImageT<PixType>::ConstIterator pChG=img.constBegin(0,1);
    typename ImageT<PixType>::ConstIterator pChB=img.constBegin(0,2);
    for (int i = 0; i < img.sizeImage(); ++i, ++pChH, ++pChS, ++pChV, ++pChR, ++pChG, ++pChB) {
      double r = *pChR;
      double g = *pChG;
      double b = *pChB;
      double max = r;                               // maximum RGB component
      int whatmax = 0;                            // r=>0, g=>1, b=>2
      if (g>max) {
          max=g;
          whatmax=1;
      }
      if (b>max) {
          max=b;
          whatmax=2;
      }
      double min = r;                               // find minimum value
      if (g<min) min = g;
      if (b<min) min = b;
      double delta = max-min;
      float v = max;                                   // calc value
      float s = (max!=0) ? (510*delta+max)/(2*max) : 0;
      float h = 0;
      if ( s == 0 ) {
          h = -1;                                // undefined hue
      }
      else {
        switch ( whatmax ) {
          case 0:                             // red is max component
            if ( g >= b )
              h = (120*(g-b)+delta)/(2*delta);
            else
              h = (120*(g-b+delta)+delta)/(2*delta) + 300;
            break;
          case 1:                             // green is max component
            if ( b > r )
              h = 120 + (120*(b-r)+delta)/(2*delta);
            else
              h = 60 + (120*(b-r+delta)+delta)/(2*delta);
            break;
          case 2:                             // blue is max component
            if ( r > g )
              h = 240 + (120*(r-g)+delta)/(2*delta);
            else
              h = 180 + (120*(r-g+delta)+delta)/(2*delta);
            break;
          }
      }
      *pChH=h;
      *pChS=s;
      *pChV=v;
    }
  }

  Image toHSV(const Image& img)
  {
    Image result(typeid(float), img.sizeX(), img.sizeY());
    ForEachTypeDo(img.typeId(), toHSVT, (*img.pImage(), *static_cast<ImageT<float>*>(result.pImage())));
    return result;
  }
}
