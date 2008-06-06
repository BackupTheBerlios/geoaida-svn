

////////////////////////////////////////////////////////////////////////////////
///
/// \file		gageoimage.cpp
/// \brief		wrapper for realising images with geo data
///
/// \date		June 2008
/// \author		Christian Becker (becker@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "gageoimage.h"
#include <ImageT>

//using Ga::Image;

namespace GaGeoImage {
//--- constructor/destructor -----------------------------------------//
  GeoImage::GeoImage(Ga::Image& img):
    image_(img)
  {

  }
  GeoImage::GeoImage(Ga::Image& img, double gW, double gN, double gE, double gS):
    image_(img){
    setGeoCoordinates(gW, gN, gE, gS);
  }



//--- methods --------------------------------------------------------//
  

  
  /** Calculate the bounding box in pixeln from given bounding box in geocoordinates */
  void GeoImage::imageBBox(const double gW, const double gN, const double gE, const double gS, int &llx,
			 int &lly, int &urx, int &ury) const{
    llx = int (floor(geo2ImageX_double(gW) + 0.5));
    urx = int (floor(geo2ImageX_double(gE) - 0.5));
    lly = int (floor(geo2ImageY_double(gS) - 0.5));
    ury = int (floor(geo2ImageY_double(gN) + 0.5));    
  }
  
  /** Calculate the bounding box in geocoordinates from given bounding box in pixeln */
  void GeoImage::geoBBox(const int llx, const int lly, const int urx, const int ury, double &gW, double &gN,
			 double &gE, double &gS) const{
    gW = image2GeoX(llx - 0.5);
    gE = image2GeoX(urx + 0.5);
    gS = image2GeoY(lly + 0.5);
    gN = image2GeoY(ury - 0.5);

  }
    
  /** set a pixel using geocoordinates */
  void GeoImage::setGeoPixel(const double gx, const double gy, const double val, const int channel) {
    int x = geo2ImageX(gx);
    int y = geo2ImageY(gy);
    image_.setPixel(x, y, val, channel);
  }
  
  /** get a pixel using geocoordinates */
  double GeoImage::getGeoPixel(const double gx, const double gy, const int channel) const{
    int x = geo2ImageX(gx);
    int y = geo2ImageY(gy);
    return image_.getPixel(x, y, channel);
  }


  /** copies label from source image. The label is selected with source_labelValue and inserted as dest_labelValue
   *  
   * */    
  void GeoImage::geoCopyLabelFromImage(const GeoImage& sourceImage, const int source_labelValue, const int dest_labelValue,
			  const int sllx, const int slly, const int surx, const int sury,
			  int& nllx, int& nlly, int& nurx, int& nury, int changeVec[])
  {

    int dllx=sllx, dlly=slly, durx=surx, dury=sury;
    if (sllx==-1) {
      dllx=0;
      durx=sourceImage.image_.sizeX()-1;
      dlly=sourceImage.image_.sizeY()-1;
      dury=0;
    }
    else{
      dllx=sllx;
      dlly=slly; 
      durx=surx;
      dury=sury;
    }

    if ((sourceImage.image_.sizeX()==image_.sizeX()) && (sourceImage.image_.sizeY()==image_.sizeY())){
      // This is normally the case.
      copyLabelFromImage(sourceImage.image_,source_labelValue, dest_labelValue);
    }
    else {            
      // This is just rarely (never?) the case, thus it is implemented "quick and dirty".
      double gW,gN,gE,gS;
      sourceImage.geoBBox(sllx,slly,surx,sury,gW,gN,gE,gS);
      imageBBox(gW,gN,gE,gS,dllx,dlly,durx,dury);

      // Range Check:
      if (dllx<0) dllx=0;
      if (dlly>image_.sizeY()) dlly=image_.sizeY();
      if (durx>image_.sizeX()) durx=image_.sizeX();
      if (dury<0) dury=0;

      float dx = float(surx-sllx+1)/float(durx-dllx+1);
      float dy = float(slly-sury+1)/float(dlly-dury+1);

      
      for (int y=dury; y<=dlly; y++) {
	int py = int((y-dury)*dy)+sury;
	for (int x=dllx; x<=durx; x++) {
	  int px = int((x-dllx)*dx)+sllx;
	  
          if (sourceImage.image_.getPixel(px,py) == source_labelValue) {
            if (changeVec) { 
	      int val = image_.getPixel(x, y);
              if(val) changeVec[val]++; //
            }
            image_.setPixel(x,y,dest_labelValue);
          }
	}
      }
    }
    
    if(nllx) nllx=dllx;
    if(nlly) nlly=dlly;
    if(nurx) nurx=durx;
    if(nury) nury=dury;
  }
    
  // ++++++++++++++ Intern Template Functions ++++++++++++++++



  
  template<typename PixType>
  void GeoImage::copyLabelFromImageT(const Ga::ImageBase* source, const double& source_labelValue, const double& dest_labelValue, const int& channel)
  {
    Ga::ImageBase* destImageB=image_.pImage();

    const Ga::ImageT<PixType>* sourceImage = dynamic_cast<const Ga::ImageT<PixType>*> (source);
    Ga::ImageT<PixType>* destImage = dynamic_cast<Ga::ImageT<PixType>*> (destImageB);
    
    if ((channel + 1 > destImage->noChannels()) || (channel + 1 > sourceImage->noChannels()))
      return;
    if (destImage->typeId() != sourceImage->typeId())
      return;
    if (destImage->noPixels() != sourceImage->noPixels())
      return;
    
     

    
  
    PixType label=(PixType)source_labelValue;
    PixType newLabel=(PixType)dest_labelValue;
  
    typename Ga::ImageT<PixType>::ConstIterator pSourceImage=sourceImage->constBegin(0,0);
    typename Ga::ImageT<PixType>::Iterator pDestImage=destImage->begin(0,0);

    for (int i = 0; i < sourceImage->noPixels(); ++i, ++pSourceImage, ++pDestImage) {
      PixType sourceValue = *pSourceImage;
      if (sourceValue == label){
	*pDestImage = newLabel;
      }
      
    }
  }

  void GeoImage::copyLabelFromImage(const Ga::Image& source, const double source_labelValue, const double dest_labelValue, const int channel)
  {
    ForEachTypeDo(image_.typeId(), copyLabelFromImageT, (source.pImage(), source_labelValue, dest_labelValue, channel));
  }

} //namespace GaGeoImage



