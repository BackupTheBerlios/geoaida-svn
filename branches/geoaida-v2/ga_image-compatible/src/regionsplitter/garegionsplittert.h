/* -*- mode:c++ -*- */
//*******************************************************************
//  Filename:   garegionsplittert.h
//  Ort:        TNT, Leibniz Uni. - Hannover, Germany
//
//  Info:	Region Finding and Exporting Interface (Template Version)
//
// Copyright (C) 2002 Martin Pahl
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//           --------      
//          /     ^ /      
//         /  ^^   /--     
//        / ^ ° ^^/  /
//        --------  /--
//         /  °  * /+ /
//         --------  /
//          /   ~~  /
//          --------
//
//*******************************************************************

#ifndef GaRegionSplitterT_h
#define GaRegionSplitterT_h

#ifdef linux
extern "C" {
int __isnanf(float);
	   }
#ifndef NAN
#define NAN sqrt(-1)
#endif
#endif

#include <QList>
#include <QString>
#include <QXmlStreamWriter>




template <class RegDescT, class RegionFinderClassT>
class RegionSplitterT{
public:
  RegionSplitterT(QList<RegDescT>& regList, 
                  Ga::Image& labelImg, 
                  RegionFinderClassT& testClass, 
                  int minSize=1, 
                  int maxSize=0, 
                  int llx=-1, int lly=-1, int urx=-1, int ury=-1);
  
  /** split the data into regions and returns the next free id */
  int split();
  int startId(int id=-1);
  void setBBox(int llx, int lly, int urx, int ury);
  void setBgId(int id);
  void setSmallRegionsId(int id);
  
  /*
    Is only used when the class name in the region description
    is empty and when RegionFinder returns an empty string.
    This is only the fall back name, but necessary for backward 
    compability.
  */
  void setRegionClass(QString classname);
  void setLabelFile(QString labelfilename);
  
  /**
     if 
     true 
     the RegionClass name is used as class attribute
     in the region description.
     if 
     false
     the pixel value in the input file is used for the class attribute
     Thus, it can be redefined later for multiclass class names.
  */
  void setSplitMode(bool multiclass=false);
  
protected:
  void goWest(int x, int y, int value, int def_val);
  void goNorth(int x, int y, int value, int def_val);
  void goEast(int x, int y, int value, int def_val);
  void goSouth(int x, int y, int value, int def_val);
  QList<RegDescT>* classify(int minSize, int maxSize);
  int setPixel(int x, int y, int val);
protected:
  QList<RegDescT>& regionList_;
  Ga::Image& lpic_;
  RegionFinderClassT& tclass_;
  int minSize_;
  int maxSize_;
  int llx_,lly_,urx_,ury_;
  int startId_;
  QString regionClass_;
  QString labelfilename_;
  bool splitmode_multiclass_;

  /* Pixel in the labelImg with this value are not bound to a region */
  int bgId_;
  /* Set regions smalle then minsize to this value */
  int smallRegionsId_;
private:
  int rl_;
  RegDescT region_;
};


/*
  Class to provide the basic region description for one region.
  This class should be derived to get specified region descriptions
  e.g. when having operator depending attributes which should  be 
  included to the region description.
*/

template <class DataPicT, class LabelPicT>
class RegDescT
{
public:
  RegDescT()
  {
    llx_=INT_MAX;
    ury_=INT_MAX;
    urx_=0;
    lly_=0;
    size_=0;
    sum_=0;
    numValidValues_=0;
    id_=0;
    class_ = "";
    file_="";
    
  };
  int setPixel(LabelPicT& lpic, int x, int y, int val)
  {
    lpic.set(x, y, val);
    size_++;
    /* 
       float v=dpic.getFloat(x,y);
       if (!isnanf(v)) {
       sum_+=v;
       numValidValues_++;
       }
    */
    if (x < llx_)
      llx_ = x;
    if (y < ury_)
      ury_ = y;
    if (x > urx_)
      urx_ = x;
    if (y > lly_)
      lly_ = y;
    return val;
  }
  void setLlx(int llx) 
  {
    llx_=llx;
  }
  void setLly(int lly) 
  {
    lly_=lly;
  }   
  void setUrx(int urx) 
  {
    urx_=urx;
  }
  void setUry(int ury) 
  {
    ury_=ury;
  }
  
  int setId(int val) {id_=val;return val;}
  int size() {return size_;}
  int id() {return id_;}
  int llx() {return llx_;}
  int lly() {return lly_;}
  int urx() {return urx_;}
  int ury() {return ury_;}
  
  virtual QXmlStreamWriter& writeAttributes(QXmlStreamWriter& fp) const
  {            
    fp.writeAttribute("class", class_);
    fp.writeAttribute("id", QString::number(id_));
    fp.writeAttribute("file", file_);
    fp.writeAttribute("llx", QString::number(llx_));
    fp.writeAttribute("lly", QString::number(lly_));
    fp.writeAttribute("urx", QString::number(urx_));
    fp.writeAttribute("ury", QString::number(ury_));
    if (!name_.isEmpty())
      fp.writeAttribute("name", name_);
    return fp;
  }
  
  virtual QXmlStreamWriter& write(QXmlStreamWriter& fp) const 
  {
    fp.writeEmptyElement("region");
    writeAttributes(fp);
    return fp;
  }
  
  QString class_, file_, name_;    
  int llx_,lly_,urx_,ury_;
  int id_;
protected:
  int size_, numValidValues_;
  double sum_;
  
  
};

template <class DataPicT, class LabelPicT>
class DifferenceT
{
 public:
  DifferenceT(float level) { level_=level;}
  bool operator()(const DataPicT &hpic,
	     const LabelPicT &lpic,
	     int x_center, int y_center,
	     int x_neighbour, int y_neighbour)
    {
      if (!valid(hpic,lpic,x_neighbour,y_neighbour)) return false;
      float v=hpic.getFloat(x_center,y_center);
      float vn=hpic.getFloat(x_neighbour,y_neighbour);
      if (isnan(v) && isnan(vn)) return true;
      if (isnan(v) || isnan(vn)) return false;
      return fabs(v-vn)<level_;
    }
  bool valid(const DataPicT &hpic, const LabelPicT &lpic, int x, int y) {
    return true;
  }
 private:
  float level_;
};

/*
template <class RegDescT, class RegionFinderClassT>
vector<RegDescT>* splitIntoRegionsT(Ga::Image& lpic, RegionFinderClassT tclass, int minSize, int maxSize);
*/


#include "garegionsplittert.hpp"



#endif
