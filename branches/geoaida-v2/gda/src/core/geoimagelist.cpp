/***************************************************************************
                          geo_image.cpp  -  description
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



#include "geoimagelist.h"
#ifdef WIN32

#else
#include <math.h>
#endif

// #define DEBUGMSG

/** default constructor */
GeoImageList::GeoImageList()
{
}

/** constructor resd from the provided filename*/
GeoImageList::GeoImageList(QString filename)
{
  minMaxResUptodate_ = false;
  read(filename);
}

/** destructor */
GeoImageList::~GeoImageList()
{
  GeoImage::clearCache();
  clear();
}

void GeoImageList::clear()
{
  qDeleteAll(*this);
  QHash< QString, GeoImage* >::clear();
  minMaxResUptodate_ = false;
}

/** Remove given image from list. The image is deleted. */
void
GeoImageList::remove(GeoImage * img)
{
  if (!img)
    return;
  minMaxResUptodate_ = false;
  QHash< QString, GeoImage* >::remove(img->filename());
  list_.removeAll(img->filename());
  delete img;
}

/** read a list of GeoImage and the attributes from the provided filename */
void
GeoImageList::read(QString filename)
{
  QFile fp(filename);
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("GeoImageList::read(%s): file not founed\n",
           filename.toLatin1().constData());
    return;
  }
  read(fp);
  fp.close();
}

/** read a list of GeoImage and the attributes from the provided filepointer */
void
GeoImageList::read(QIODevice & fp)
{
  MLParser parser(&fp);
  read(parser);
}

/** read a list of GeoImage and the attributes through parser */
void
GeoImageList::read(MLParser & parser)
{
  minMaxResUptodate_ = false;
  qDebug("GeoImageList::read(parser)");
  list_.clear();
  QString keywords[] = { "geoimage", "geoimagelist", "" };
  const MLTagTable nodeTagTable(keywords);
  const int TOK_GEOIMAGE = 1;
  const int TOK_GEOIMAGE_LIST = 2;
  int tag;
#ifdef WIN32 
  geoNorth_ = -1e38F;
  geoSouth_ = 1e38F;
  geoWest_  = 1e38F;
  geoEast_  = -1e38F;
#else
  geoNorth_ = -1e36;
  geoSouth_ = +1e36;
  geoWest_ = +1e36;
  geoEast_ = -1e36;
#endif
  do {
    tag = parser.tag(nodeTagTable);
    switch (tag) {
    case TOK_GEOIMAGE:{
        GeoImage *gi = new GeoImage(parser);    //parse geoimage
#ifdef WIN32
  if (gi == 0){
    //cout << "Out of Memory..6";
    exit(1);
  }
#endif
        if (gi->geoNorth() > geoNorth_)
          geoNorth_ = gi->geoNorth();
        if (gi->geoSouth() < geoSouth_)
          geoSouth_ = gi->geoSouth();
        if (gi->geoEast() > geoEast_)
          geoEast_ = gi->geoEast();
        if (gi->geoWest() < geoWest_)
          geoWest_ = gi->geoWest();
        insert(gi->value("key"), gi);    //insert read geoimage
        list_ += gi->value("key");       //fill additional list of image names
        break;
      }
    default:{
        ArgDict *args = parser.args();
        delete args;
        break;
      }
    }
  } while ((tag != MLParser::END_OF_FILE) && (tag != -TOK_GEOIMAGE_LIST));

  //test images and create a structure for the images
  QString key;
  for (QStringList::Iterator it = list_.begin(); it != list_.end(); ++it) {
    key = *it;
    GeoImage *img = value(key);
    qDebug("## GeoImageList::read(parser) %s",img->filename().toLatin1().constData());
    img->load();
  }
}


/** return void pointer to the GeoImage class */
GeoImage* GeoImageList::geoImage(QString imgKey)
{
  return value(imgKey);
}

/** return void pointer to the image data */
const void* GeoImageList::data(QString imgKey)
{
  return (value(imgKey))->data();
}

/** return the cols numbers of the image */
int GeoImageList::cols(QString imgKey)
{
  return (value(imgKey))->cols();
}

/** return the rows numbers of the image */
int GeoImageList::rows(QString imgKey)
{
  return (value(imgKey))->rows();
}

/** return geoNorth value of the image*/
float GeoImageList::geoNorth(QString imgKey)
{
  if (imgKey.isEmpty())
    return geoNorth_;
  return (value(imgKey))->geoNorth();
}

/** return geoSouth value of the image*/
float
GeoImageList::geoSouth(QString imgKey)
{
  if (imgKey.isEmpty())
    return geoSouth_;
  return (value(imgKey))->geoSouth();
}

/** return geoEast value of the image*/
float
GeoImageList::geoEast(QString imgKey)
{
  if (imgKey.isEmpty())
    return geoEast_;
  return (value(imgKey))->geoEast();
}

/** return geoWest value of the image*/
float
GeoImageList::geoWest(QString imgKey)
{
  if (imgKey.isEmpty())
    return geoWest_;
  return (value(imgKey))->geoWest();
}

/** return Info about the image*/
QString
GeoImageList::info(QString imgKey)
{
  return (value(imgKey))->info();
}

#pragma warning (disable : 4715 )
//Nicht alle Steuerelementpfade geben einen Wert zurück
//** write a scrap of the data to a file*/
QString
GeoImageList::part(QString imgKey, float north, float south, float west,
                   float east, QString fname)
{
  if (contains(imgKey))
    return (value(imgKey))->part(west, north, east, south, fname);
  else
    qDebug("## (Warning) GeoImageList::part(%s, %f, %f, %f, %f) no entry",
           imgKey.toLatin1().constData(), west, north, east, south);
}
#pragma warning (default : 4715 )

/** return a list of included images*/
QStringList GeoImageList::list(QString type)
{
  if (type.isEmpty())
    return list_;
  QStringList list;
  QStringList::ConstIterator it;
  for (it = list_.begin(); it != list_.end(); ++it) {
    GeoImage *im = geoImage(*it);
    if (QString::compare(im->type(), type, Qt::CaseInsensitive) == 0)
      list += (*it);
    qDebug("GeoImageList::list(item=%s)", it->toLatin1().constData());
  }
  return list;
}

/** loads the given labelimage fname and returns a pointer to the new image. If the image
was already requested, only a pointer to the image is returned and the reference
counter of the image is incremented. */
GeoImage *GeoImageList::loadLabelImage(QString fname,
                             QString key,
                             float west, float north, float east, float south)
{
  minMaxResUptodate_ = false;
  GeoImage *img = value(fname);
  if (img)
    return img->shallowCopy();
  GeoImage *gi = new GeoImage(fname, key, west, north, east, south);
#ifdef WIN32
  if (gi == 0){
   // cout << "Out of Memory..7";
    exit(1);
  }
#endif
  insert(fname, gi);            //insert read geoimage
  list_ += fname;               //fill additional list of image names
  return gi;
}


/** return the maximum resolution in the list of images */
float
GeoImageList::maxResolution()
{
  if (!minMaxResUptodate_) {
    minRes_ = -1;
    maxRes_ = 500;
    qDebug("GeoImageList::maxResolution");
    Iterator it = begin();
    for (; it!=end(); ++it) {
      GeoImage *img = it.value();
      qDebug("%s: xres: %f yres: %f",
             img->filename().toLatin1().constData(),
             img->resolutionX(), 
	     img->resolutionY());
      float res = img->resolutionX();
      if (res > minRes_)
        minRes_ = res;
      if (res < maxRes_)
        maxRes_ = res;
      res = img->resolutionY();
      if (res > minRes_)
        minRes_ = res;
      if (res < maxRes_)
        maxRes_ = res;
    }
    minMaxResUptodate_ = true;
  }
  return maxRes_;
}

/** return the minimum resolution in the list of images */
float
GeoImageList::minResolution()
{
  if (!minMaxResUptodate_)
    maxResolution();
  return minRes_;
}

GeoImageList geoImageList_;
//GeoImageList labelImageList_;

