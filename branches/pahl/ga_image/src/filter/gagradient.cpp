/* -*- mode:c++ -*- */
//*******************************************************************
//  Filename:   gradient.h
//  Datum:      1997
//  Autoren:    J"urgen B"uckner
//  Ort:        TNT, Uni. - Hannover, Germany
//  Projekt:    Bilddatenstruktur - TROPIC
//
//  Info:	calculate the gradient of the image [3x3] Sobel
//
//           --------      
//          /     ^ /      
//         /  ^^   /--     
//        / ^ � ^^/  /
//        --------  /--
//         /  �  * /+ /
//         --------  /
//          /   ~~  /
//          --------
//
//*******************************************************************

#ifndef __GA_GRADIENT_H
#define __GA_GRADIENT_H

#include "gagradient.h"
#include "gagradientt.h"
#include "gaimaget.h"
#include "gaimage.h"

namespace Ga {
template <class PixTyp> void gradient(const ImageBase& result, ImageBase& imgIn)
{
  ImageT<PixTyp>& im1=(ImageT<PixTyp>&)result;
  ImageT<PixTyp>& im2=(ImageT<PixTyp>&)imgIn;
  gradient(im1,im2);
}

Image gradient(const Image& imgIn)
{
  Image result(imgIn.typeId());
  ForEachTypeDo(imgIn.typeId(),gradient,(*(result.pImage()),*(imgIn.pImage())));
  return result;
}

Image gradient2D(const Image &pic, int channel)
{
  int sizex=pic.sizeX();
  int sizey=pic.sizeY();
  Image result(typeid(float),sizex,sizey,2);

  for (int y=0; y<sizey-1; y++)
    for (int x=0; x<sizex-1; x++) {
      float p11=pic.getPixel(x,y);
      float p21=pic.getPixel(x+1,y);
      float p12=pic.getPixel(x,y+1);
      float p22=pic.getPixel(x+1,y+1);
      if (isnan(p11) || isnan(p21) || isnan(p12) || isnan(p22)) {
        result.setPixel(x,y,NAN);
        continue;
      }
      double l1=p22-p11;
      double l2=p21-p12;
      result.setPixel(x,y,hypot(l1,l2),0);
      double angle=atan2(l1,l2);
      angle-=M_PI_4;
      if (angle<-M_PI) angle+=2*M_PI;
      result.setPixel(x,y,angle,1);
    }
  for (int x=0; x<sizex; x++) {
    result.setPixel(x,sizey-1,NAN,0);
    result.setPixel(x,sizey-1,NAN,1);
  }
  for (int y=0; y<sizey; y++) {
    result.setPixel(sizex-1,y,NAN,0);
    result.setPixel(sizex-1,y,NAN,1);
  }
  return result;
}

Image gradient2D3Channels(const Image &pic)
{
#define sqr(x) ((x)*(x))
  Image result(typeid(float),pic.sizeX(),pic.sizeY(),2);
  int sizex=pic.sizeX();
  int sizey=pic.sizeY();

  for (int x=0; x<sizex-1; x++)
    for (int y=0; y<sizey; y++) {
      float p11r=pic.getPixel(x  ,y  ,0);
      float p12r=pic.getPixel(x  ,y+1,0);
      float p21r=pic.getPixel(x+1,y  ,0);
      float p22r=pic.getPixel(x+1,y+1,0);
      float p11g=pic.getPixel(x  ,y  ,1);
      float p12g=pic.getPixel(x  ,y+1,1);
      float p21g=pic.getPixel(x+1,y  ,1);
      float p22g=pic.getPixel(x+1,y+1,1);
      float p11b=pic.getPixel(x  ,y  ,2);
      float p12b=pic.getPixel(x  ,y+1,2);
      float p21b=pic.getPixel(x+1,y  ,2);
      float p22b=pic.getPixel(x+1,y+1,2);

      float l1=sqrt(sqr(p22r-p11r)+sqr(p22g-p11g)+sqr(p22b-p11b));
      float l2=sqrt(sqr(p21r-p12r)+sqr(p21g-p12g)+sqr(p21b-p12b));
      result.setPixel(x,y,hypot(l1,l2),0);
      float angle=atan2(l1,l2);
      angle-=M_PI_4;
      if (angle<-M_PI) angle+=2*M_PI;
      result.setPixel(x,y,angle,1);

    }
  for (int x=0; x<sizex; x++) {
    result.setPixel(x,sizey-1,NAN,0);
    result.setPixel(x,sizey-1,NAN,1);
  }
  for (int y=0; y<sizey; y++) {
    result.setPixel(sizex-1,y,NAN,0);
    result.setPixel(sizex-1,y,NAN,1);
  }
  return result;
#undef sqr
}

Image gradient2DhsvMax(const Image &pic)
{
#define sqr(x) ((x)*(x))
  Image result(typeid(float),pic.sizeX(),pic.sizeY(),2);
  int sizex=pic.sizeX();
  int sizey=pic.sizeY();

  for (int x=0; x<sizex-1; x++)
    for (int y=0; y<sizey; y++) {
      float p11h=pic.getPixel(x  ,y  ,0);
      float p12h=pic.getPixel(x  ,y+1,0);
      float p21h=pic.getPixel(x+1,y  ,0);
      float p22h=pic.getPixel(x+1,y+1,0);
      float p11s=pic.getPixel(x  ,y  ,1);
      float p12s=pic.getPixel(x  ,y+1,1);
      float p21s=pic.getPixel(x+1,y  ,1);
      float p22s=pic.getPixel(x+1,y+1,1);
      float p11v=pic.getPixel(x  ,y  ,2);
      float p12v=pic.getPixel(x  ,y+1,2);
      float p21v=pic.getPixel(x+1,y  ,2);
      float p22v=pic.getPixel(x+1,y+1,2);

      double l1=p22h-p11h;
      double l2=p21h-p12h;
      if (p11h==-1 && p22h!=-1) l1=180;
      if (p22h==-1 && p11h!=-1) l1=-180;
      if (p12h==-1 && p21h!=-1) l2=180;
      if (p21h==-1 && p12h!=-1) l2=-180;
      if (l1>180) l1-=180;
      if (l1<-180) l1+=180;
      if (l2>180) l2-=180;
      if (l2<-180) l2+=180;
      double habs=hypot(l1,l2);
      double hangle=atan2(l1,l2);

      l1=p22s-p11s;
      l2=p21s-p12s;
      double sabs=hypot(l1,l2);
      double sangle=atan2(l1,l2);

      l1=p22v-p11v;
      l2=p21v-p12v;
      double vabs=hypot(l1,l2);
      double vangle=atan2(l1,l2);

      if (habs>vabs) {
        vabs=habs;
        vangle=hangle;
      }

      if (sabs>vabs) {
        vabs=sabs;
        vangle=sangle;
      }
      result.setPixel(x,y,vabs,0);
      vangle-=M_PI_4;
      if (vangle<-M_PI) vangle+=2*M_PI;
      result.setPixel(x,y,vangle,1);

    }
  for (int x=0; x<sizex; x++) {
    result.setPixel(x,sizey-1,NAN,0);
    result.setPixel(x,sizey-1,NAN,1);
  }
  for (int y=0; y<sizey; y++) {
    result.setPixel(sizex-1,y,NAN,0);
    result.setPixel(sizex-1,y,NAN,1);
  }
  return result;
#undef sqr
}


} // namespace Ga

#endif
