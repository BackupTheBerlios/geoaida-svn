////////////////////////////////////////////////////////////////////////////////
///
/// \file		gageoimage.h
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

#ifndef __GA_GEOIMAGE_H
#define __GA_GEOIMAGE_H

#include <typeinfo>
#include <vips/vips>

namespace GaGeoImage
{

  struct GeoCoordinates {

    double north;
    double south;
    double west;
    double east;

    GeoCoordinates(){
      north=0;
      south=0;
      west=0;
      east=0;
    }
    void clear(){
      north=0;
      south=0;
      west=0;
      east=0;
    }
  };

  class GeoImage
  {
  public:
    //--- constructor/destructor -----------------------------------------//
      GeoImage(vips::VImage& img);

      GeoImage(vips::VImage& img, const double gW, const double gN, const double gE, const double gS);
    
    //--- methods --------------------------------------------------------//


    void setGeoCoordinates(const double gW, const double gN, const double gE, const double gS);

    /** get west/left geocoordinate */
    inline double geoWest() const{
      return coordinates_.west;
    }
    
    /** get north/top geocoordinate */
    inline double geoNorth() const{
      return coordinates_.north;
    }

    /** get east/right geocoordinate */
    inline double geoEast() const{
      return coordinates_.east;
    }

    /** get south/bottom geocoordinate */
    inline double geoSouth() const{
       return coordinates_.south;
    }

    /** set the resolution of this image unit [m/pixel] */
    inline void setGeoResolution(const double res){
      resolution_=res;
    }
    
    /** get the resolution of this image [m/pixel] */
    inline double geoResolution() const{
      return resolution_;
    }

    /** returns the x-image coordinate of a x-geocoordinate */
    int geo2ImageX(const double geo) const;

    /** returns the x-image coordinate of a x-geocoordinate, caution: no range check */
    int geo2ImageX_f(const double geo) const;

    /** returns the x-image coordinate of a x-geocoordinate, caution: no range check */
    double geo2ImageX_double(const double geo) const;

    /** returns the y-image coordinate of a y-geocoordinate */
    int geo2ImageY(const double geo) const;
    
    /** returns the y-image coordinate of a y-geocoordinate, caution: no range check */
    int geo2ImageY_f(const double geo) const;

    /** returns the y-image coordinate of a y-geocoordinate, caution: no range check */
    double geo2ImageY_double(const double geo) const;

    /** returns the geocoordinate of a x-image coordinate */
    double image2GeoX(const double x) const;

    /** returns the geocoordinate of a y-image coordinate */
    double image2GeoY(const double y) const;

    /** Calculate the bounding box in pixeln from given bounding box in geocoordinates */
    void imageBBox(const double gW, const double gN, const double gE, const double gS, int &llx,
		   int &lly, int &urx, int &ury) const;

    /** Calculate the bounding box in geocoordinates from given bounding box in pixeln */
    void geoBBox(const int llx, const int lly, const int urx, const int ury, double &gW, double &gN,
                    double &gE, double &gS) const;

    /** returns the number of image pixels covered by a geo-distance (only exact for 0 and 90 degree) */
    double geodist2pixel(const double dist) const;

    /** returns the geo-distance for a number of pix image pixel (only exact for 0 and 90 degree) */
    double pixel2geodist(const int pix) const;

    /** set a pixel using geocoordinates */
    void setGeoPixel(const  double gx,  const double gy, const double val, const int band=0);

    /** get a pixel using geocoordinates */
    double getGeoPixel(const double gx, const double gy, const int band=0) const;
    
    /** resets geo coordiantes to 0 */
    void clearGeo();


    #if 0
    /** copies label from source image. The label is selected with source_labelValue and inserted as dest_labelValue
     * */
    void geoCopyLabelFromImage(const GeoImage& sourceImage, const int source_labelValue, const int dest_labelValue,
			       const int sllx, const int slly, const int surx, const int sury,
			       int& nllx, int& nlly, int& nurx, int& nury, int changeVec[]);
#endif
  protected:
    vips::VImage& image_;
    GeoCoordinates coordinates_;
    double resolution_;

  private:

    void copyLabelFromImage(vips::VImage& source, 
			    const int source_labelValue, 
			    const int dest_labelValue, 
			    const int offsetX = 0,
			    const int offsetY = 0);
  }; 







/** set geocoordinates for this image */
  inline void GeoImage::setGeoCoordinates(const double gW, const double gN, const double gE, const double gS) {
    coordinates_.south=gS;
    coordinates_.north=gN;
    coordinates_.east=gE;
    coordinates_.west=gW;
  }

/** returns the x-image coordinate of a x-geocoordinate */
  inline int GeoImage::geo2ImageX(double geo) const {
    double f1 = coordinates_.east - coordinates_.west;
    double f2 = geo - coordinates_.west;
    
    return (int)(f2 / f1 * image_.Xsize());
  }

 
/** returns the x-image coordinate of a x-geocoordinate, caution: no range check */
 inline int GeoImage::geo2ImageX_f(double geo) const {
   return (int)((geo - coordinates_.west) / (coordinates_.east - coordinates_.west) * (double)image_.Xsize());
 }

/** returns the x-image coordinate of a x-geocoordinate, caution: no range check */
 inline double GeoImage::geo2ImageX_double(double geo) const {
   //!MP10.07.01  return ((geo - coordinates_.west) / (coordinates_.east - coordinates_.west) * (double)image.sizeX());
   return ((geo - coordinates_.west) / (coordinates_.east - coordinates_.west) * (double)image_.Xsize());
 }

 /** returns the y-image coordinate of a y-geocoordinate */
 inline int GeoImage::geo2ImageY(double geo) const {
   return (int)(image_.Ysize()/(coordinates_.south-coordinates_.north)*(geo-coordinates_.north));
 }

/** returns the y-image coordinate of a y-geocoordinate, caution: no range check */
 inline int GeoImage::geo2ImageY_f(double geo) const {
  
   return (int)(image_.Ysize()/(coordinates_.south-coordinates_.north)*(geo-coordinates_.north));
 }

/** returns the y-image coordinate of a y-geocoordinate, caution: no range check */
 inline double GeoImage::geo2ImageY_double(double geo) const {
   
   return image_.Ysize()/(coordinates_.south-coordinates_.north)*(geo-coordinates_.north);
 }
 
/** returns the geocoordinate of a x-image coordinate */
 inline double GeoImage::image2GeoX(double x) const {
   
   return (x+0.5)*(coordinates_.east-coordinates_.west)/image_.Xsize()+coordinates_.west;
 }
 
 /** returns the geocoordinate of a y-image coordinate */
 inline double GeoImage::image2GeoY(double y) const {
   
   return (coordinates_.south-coordinates_.north)/image_.Ysize()*(y+0.5)+coordinates_.north;
 }
 
 /** returns the geo-distance for a number of pix image pixel (only exact for 0 and 90 degree) */
 
 inline double GeoImage::pixel2geodist(const int pix) const{
   return (pix * resolution_);
 }
 
 /** returns the number of image pixels covered by a geo-distance (only exact for 0 and 90 degree) */
 
 inline double GeoImage::geodist2pixel(const double dist) const{
   return (dist / resolution_);
 }

 inline void GeoImage::clearGeo() {
   coordinates_.clear();
   //  cursor_x_ = cursor_y_ = 0;
 }




} // namespace GaGeoImage



#endif
