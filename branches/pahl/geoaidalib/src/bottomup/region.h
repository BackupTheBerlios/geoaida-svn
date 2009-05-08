/***************************************************************************
                          region.h  -  description
                             -------------------
    begin                : Thu Oct 19 2000
    copyright            : (C) 2000 by Jürgen Bückner
    email                : bueckner@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef REGION_H
#define REGION_H

#include <QString>
#include <QPoint>
#include <QFile>

#include <MLParser>
#include <vips/vips>
#include "Stack"

class Ga::Image;

namespace BottomUp {

class RegionList;

/**class to handel the infos for one image
  *@author Jürgen Bückner
  */

class Region : public ArgDict {
public: 
  /** default constructor */
  Region();
  
  /** constructor read attributes for this Region through parser*/
  Region(MLParser& parser);
  
  /** denstructor */
  ~Region();
  
 protected: // Protected attributes
  enum IMGTYPE {
    PFM_FLOAT=0,
    PFM_SINT,
    PFM_UINT,
    PFM_SINT16,
    PFM_UINT16,
    PBM,
    PGM,
    PPM,	
    UNKNOWN
  };
  
 public:
  /** init routine */
  void init();
  
  /** read attributes for this Region through parser */
  void read(MLParser& parser);
  
  /** load image info - not the data */
  void load(RegionList& regionList);
  
  /** write data to file */
  void write(QTextStream& fp, QString keyword="region");
  
  /** return the image filename */
  QString filename();

  /** set the image filename */
  void filename(QString);
  
  /** return data */
  Ga::Image* data();

  //** set value in region */
  void setValue(QString key, QString val);

  //** get value from region */
  QString* getValue(const QString key);

  /** return data label id */
  int id() {return id_;}
  /** set label id */
  void id(int l);
  /** set label weighing */
  void p(float f);
  /** get label weighing */
  float p(void) {return valuation_;}
#if 0	
  /** return data typ */
  int dataTyp() {return type_;}	
  /** set data typ */
  void dataTyp(int t);
#endif	
  /** return cols */
  int sizeX() {return cols_;}
  /** return rows */
  int sizeY() {return rows_;}
  /** return cols */
  int cols() {return cols_;}
  /** return rows */
  int rows() {return rows_;}
  /** set  geoNorth*/
  void geoNorth(float value);
  /** return  geoNorth*/
  float geoNorth() {return geoNorth_;}
  /** set  geoSouth*/
  void geoSouth(float value);
  /** return  geoSouth*/
  float geoSouth() {return geoSouth_;}
  /** set  geoEast*/
  void geoEast(float value);
  /** return  geoEast*/
  float geoEast() {return geoEast_;}
  /** set  geoWest*/
  void geoWest(float value);
  /** return  geoWest*/
  float geoWest() {return geoWest_;}
  /** return lower left x-value*/
  int llx(int v=-1) {if (v==-1) return llx_; else llx_=v; return v;}
  /** return lower left y-value*/
  int lly(int v=-1) {if (v==-1) return lly_; else lly_=v; return v;}
  /** return upper right x-value*/
  int urx(int v=-1) {if (v==-1) return urx_; else urx_=v; return v;}
  /** return upper right y-value*/
  int ury(int v=-1) {if (v==-1) return ury_; else ury_=v; return v;}
  /** return x-resolution of the image*/
  float xResolution();
  /** return y-resolution of the image*/
  float yResolution();
  /** set class name*/
  void classname(QString str);
  /** return class name*/
  QString classname() {return class_;}
  /** return identifier name*/
  QString name() {return name_;}
  /** return region address*/
  QString addr() {return addr_;}
  /** return region address*/
  QString key() {return key_;}
  /** return info about this region*/
  void info() {
    ArgDictIterator it = iterator();
    for (it = begin(); it != end(); ++it){
      const QString& key = it.key();
      const QString& val = it.value();
      printf("%s=\"%s\"", key.toStdString().c_str(), val.toStdString().c_str());
    }
  };
  /** return stack - for bottom-up */
  Stack& stack(void);
  /** Removes the top item from the local stack and returns it. */
  StackElement* stackPop();
  /** Adds an element d to the top of the local stack. */
  void stackPush(const StackElement* d);
  /** Returns the number of items in the local stack.  */
  uint stackCount(void);
  /** Returns TRUE is the local stack contains no elements to be popped; otherwise returns FALSE.  */
  bool stackIsEmpty ();
  /** Removes the top item from the local stack and deletes it if autoDelete() is TRUE. Returns TRUE if there was an item to pop;
      otherwise returns FALSE.  */
  bool stackRemove (void) ;
  /** Removes all items from the local stack, deleting them if autoDelete() is TRUE.  */
  void stackClear(void);
  /** update the internal variables using the dictionary */
  void update();
  
  //Operatoren:
  /** return class name*/
  //friend ostream& operator<< (ostream& os, Region& region);
  
 protected: // Protected attributes
  QString filename_, class_, name_, addr_, key_;
  Ga::Image* data_;
  int cols_, rows_, id_, llx_, lly_, urx_, ury_;
  float geoNorth_, geoSouth_, geoEast_, geoWest_, valuation_;
  float minval_, maxval_;
  
private:	 // Private attributes
  /** test the consistent of the image date */
  bool testSize(int cols, int rows, IMGTYPE type);
  /** geo position to image position */
  int geo2pic(float x, float y, int *rx, int *ry);
  /** stack for bottom-up operator */
  Stack stack_;
  
};

}  // namespace BottomUp

#endif

