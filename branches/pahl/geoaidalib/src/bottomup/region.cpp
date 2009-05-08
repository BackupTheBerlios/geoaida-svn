/***************************************************************************
                          region.cpp  -  description
                             -------------------
    begin                : Thu Oct 19 2000
    copyright            : (C) 2000 by J�rgen B�ckner
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

#ifdef DEBUG
#define DEBUGMSG
#endif

#include "region.h"
#include "regionlist.h"

using BottomUp;

Region::Region() {
  init();
  setAutoDelete(TRUE);
}

Region::Region(MLParser& parser) {
  init();
  read(parser);
}

Region::~Region() {
}

void Region::init() {
  setAutoDelete(true);
  data_ = 0;
#if 0	
  type_ = UNKNOWN;
#endif	
  cols_ = rows_ = id_ = 0;
  geoSouth_ = geoNorth_ = geoEast_ = geoWest_ = 0.0;
  valuation_ = 1.0;
  filename_ = ""; class_ = "";
}

//** set value in region */07

void Region::setValue(QString key, QString val) {
	replace(key, val);
}
//** get value from region */
QString* Region::getValue(const QString key) {
  return (this)->find(key);
}

/** set label id */
void Region::id(int l) {
  id_ = l;
  QString *s = new QString;
  s->setNum(l);
  replace("id",s);
}

/** set label weighing */
void Region::p(float f) {
  QString *s = new QString;
  s->setNum(f);
  replace("p",s);
}

#if 0
/** set data typ */
void Region::dataTyp(int t) {
  type_ = (IMGTYPE)t;
  QString *s = new QString;
  s->setNum(type_);
  replace("type",s);
}
#endif

/** set the image filename */
void Region::filename(QString fn) {
  filename_ = fn;	
  replace("file",&filename_);
}

/** set class name*/
void Region::classname(QString str) {
  class_ = str;
  replace("class",&class_);
}

/** set  geoNorth*/
void Region::geoNorth(float value) 
{
  geoNorth_=value;
  if (getValue("file_geoNorth"))
    replace("file_geoNorth",value);
}

/** set  geoSouth*/
void Region::geoSouth(float value)
{
  geoSouth_=value;
  if (getValue("file_geoSouth"))
    replace("file_geoSouth",value);
}

/** set  geoEast*/
void Region::geoEast(float value)
{
  geoEast_=value;
  if (getValue("file_geoEast"))
    replace("file_geoEast",value);
}

/** set  geoWest*/
void Region::geoWest(float value)
{
  geoWest_=value;
  if (getValue("file_geoWest"))
    replace("file_geoWest",value);
}

/** return x-resolution of the image*/
float Region::xResolution() {
  if(cols_)
    return float(geoNorth_-geoSouth_) / float(rows_);
  else return 0.0;
}

/** return y-resolution of the image*/
float Region::yResolution() {
  if(rows_)
    return float(geoEast_-geoWest_) / float(cols_);
  else return 0.0;
}

/** read attributes for this operator through parser */
void Region::read(MLParser& parser) { //bekommt einen Parser �bergeben und liest die Attribute ein
  qDebug("Region::read(MLParser& parser)");
  parser.args(this);
  update();
  //if (!find("type")) insert("type",new QString("not set"));
}

/** update the internal variables using the dictionary */
void Region::update()
{
  MLParser::setString(addr_,this,"addr");
  MLParser::setInt(llx_,this,"llx");
  MLParser::setInt(lly_,this,"lly");
  MLParser::setInt(urx_,this,"urx");
  MLParser::setInt(ury_,this,"ury");
  MLParser::setFloat(geoNorth_,this,"file_geoNorth");
  MLParser::setFloat(geoSouth_,this,"file_geoSouth");
  MLParser::setFloat(geoWest_,this,"file_geoWest");
  MLParser::setFloat(geoEast_,this,"file_geoEast");
  MLParser::setFloat(valuation_,this,"p");
  MLParser::setInt(id_,this,"id");
  MLParser::setString(filename_,this,"file");
  MLParser::setString(name_,this,"name");
  MLParser::setString(class_,this,"class");
  if (addr_.isEmpty()) key_=QString().sprintf("%04d",id_);
  else key_=addr_;
}

/** load image info - not the data */
void Region::load(RegionList& regionList) {
  //qDebug("Region::load(%s)",(const char*)filename_);
  data_=&(regionList.readLabelFile(filename_,geoWest_,geoNorth_,geoEast_,geoSouth_));
  cols_=data_->sizeX();
  rows_=data_->sizeY();
  if ((geoNorth_==geoSouth_) || (geoWest_==geoEast_)) {
    geoNorth_=rows_;
    geoSouth_=0;
    geoWest_=0;
    geoEast_=cols_;
    data_->setGeoCoordinates(geoWest_,geoNorth_,geoEast_,geoSouth_);
  }
  if (!(*this)["llx"]) llx_=0;
  if (!(*this)["urx"]) urx_=cols_-1;
  if (!(*this)["lly"]) lly_=rows_-1;
  if (!(*this)["ury"]) ury_=0;
#if 0
  FILE *fp;
  fp=fopen(filename_,"r");
  if (!fp) {
    qWarning("#  (ERROR)Region::load(%s) Can't open file for reading!",(const char*)filename_);
    return;
  }
  int pxmtype;
//  unsigned char max_x;
  data_ = 0;
  type_ = UNKNOWN;
  float minval_, maxval_;

  if (pfm_readpfm_header(fp, &cols_, &rows_, &minval_, &maxval_, &pxmtype))
    type_ = (IMGTYPE)pxmtype; //0 = FALSE, 1 = TRUE
  else { /* now check for all the other fucking p?m-formats */
  	xelval maxx;
    pnm_readpnminit(fp, &cols_, &rows_, &maxx, &pxmtype);
    rewind(fp);
    minval_ = 0.0; maxval_ = (float)maxx;
    if (pxmtype == PBM_FORMAT || pxmtype == RPBM_FORMAT) type_ = PBM;
    if (pxmtype == PGM_FORMAT || pxmtype == RPGM_FORMAT) type_ = PGM;
    if (pxmtype == PPM_FORMAT || pxmtype == RPPM_FORMAT) type_ = PPM;
  }
  fclose(fp);
  //qDebug("##  Image: (%d, %d) %f - %f, type: %d - %s",
   //       cols_, rows_, minval_, maxval_, type_, (const char*)filename_);
  if (type_ == UNKNOWN) qDebug("##  (ERROR) unknown image type!");
#endif
}

/** return data */
Ga::Image* Region::data() {
  qDebug("Region::data() - load data");
  return data_;
#if 0
  if(data_) return data_;
  qDebug("Region::data() %s",(const char*)filename_);

  FILE *fp;
  fp=fopen(filename_,"r");
  if (!fp) {
    qWarning("#  (ERROR)Region::load(%s) Can't open file for reading!",(const char*)filename_);
    return 0;
  }
  int cols, rows;
  //float minval, maxval;
  if (type_ == UNKNOWN) {fclose(fp); return 0;}
  if (type_ <= PFM_UINT16) {
    data_ = pfm_readpfm_type(fp, &cols, &rows, &minval_, &maxval_, type_, 0);
    testSize(cols, rows, type_);
  } else if (type_ == PBM) {
    data_ = (void*) pbm_readpbm(fp, &cols, &rows);
    minval_ = 0.0, maxval_ = 1.0;
    testSize(cols, rows, type_);
  } else if (type_ == PGM) {
    gray maxval, **data_g;
    data_g = pgm_readpgm(fp, &cols, &rows, &maxval);
    data_ = (void*) data_g;
    minval_ = 0.0, maxval_ = (float)maxval;
    testSize(cols, rows, type_);
  } else if (type_ == PPM) {
    pixval maxval;
    pixel **dat_p;
    dat_p = ppm_readppm(fp, &cols, &rows, &maxval);
    data_ = (void*) dat_p;
    minval_ = 0.0, maxval_ = (float)maxval;
    testSize(cols, rows, type_);
  }
  fclose(fp);
  return data_;
#endif
}

bool Region::testSize(int cols, int rows, IMGTYPE type) {
  if (cols!=cols_ || rows!=rows_) {
#if 0
    qWarning("##  (ERROR) inconsistent image data!");
    if (type_ <= PFM_UINT16) delete (float*)data_;
    if (type_ == PBM) delete (bit*)data_;
    if (type_ == PGM) delete (gray*)data_;
    if (type_ == PPM) delete (pixel*)data_;
    data_ = 0;
    type_ = UNKNOWN;
#endif
    return FALSE;

  }
  return TRUE;
}

/** get the filename */
QString Region::filename() {
  return filename_;
}

#if 0
/** write a scrap of the data,
  * return the filename,
  * if the file exist do nothing
  * argument fname is optional
  * the coordinates of the image part are geodata e.g. Gauss Krueger **/
QString Region::part(float north, float south, float west, float east, QString fname) {
  if(fname.isEmpty()) //create output filname
  fname.sprintf("/tmp/%s_%f_%f_%f_%f",(const char*)(*(find("key"))),north,south,west,east);
  qDebug("#  Region::part %s (%f, %f, %f, %f)",(const char*)fname, north, south,  west, east);
  QFile f(fname);
  if (f.exists()) return fname;

  void* data_p = data(); //get pointer to data
  if (type_ == UNKNOWN) return 0;
  int dx, dy, i, j, rx1, ry1, rx2, ry2;
  geo2pic(east, north, &rx2, &ry1);
  geo2pic(west, south, &rx1, &ry2);
  dx = rx2 - rx1;
  dy = ry2 - ry1;
  if (dx<=0 || dy<=0) qWarning("#  (ERROR) Region::part: (dx=%d=%d-%d || dy=%d=%d-%d)",dx, rx2, rx1,dy, ry2, ry1);

  FILE *of=fopen(fname,"w");
  if (!of) {
      fprintf(stderr,"#  (ERROR) Can't open file %s for writing!\n",(const char*)fname);
      return "";
  }

  if (type_ <= PFM_UINT16) {
    pfm_writepfm_region_type(of, data_, cols_, rows_, minval_, maxval_,
        rx1, ry1, rx2, ry2, type_);
    return fname;
  } else if (type_ == PBM) {
    bit *bpi, *bpo, **dat_b_out = pbm_allocarray( dx, dy );
    bit **dat_b = (bit **)data_p;
    for (i = 0; i < dy; i ++) {
      bpi = dat_b[i + ry1] + rx1;
      bpo = dat_b_out[i];
      for (j = 0; j < dx; j ++) {
        *bpo = *bpi;
        bpo ++;
        bpi ++;
      }
    }
    pbm_writepbm(of, (bit**)dat_b_out, dx, dy, 0);
  } else if (type_ == PGM) {
    gray *gpi, *gpo, **dat_g_out = pgm_allocarray( dx, dy );
    gray **dat_g = (gray **)data_p;
    for (i = 0; i < dy; i ++) {
      gpi = dat_g[i + ry1] + rx1;
      gpo = dat_g_out[i];
      for (j = 0; j < dx; j ++) {
        *gpo = *gpi;
        gpo ++;
        gpi ++;
      }
    }
    pgm_writepgm(of, (gray**)dat_g_out, dx, dy, (gray)maxval_, 0);
  } else if (type_ == PPM) {
    pixel *ppi, *ppo, **dat_p_out = ppm_allocarray( dx, dy );
    pixel **dat_p = (pixel **)data_p;
    for (i = 0; i < dy; i ++) {
      ppi = dat_p[i + ry1] + rx1;
      ppo = dat_p_out[i];
      for (j = 0; j < dx; j ++) {
        *ppo = *ppi;
        ppo ++;
        ppi ++;
      }
    }
    ppm_writeppm(of, (pixel**)dat_p_out, dx, dy, (pixval)maxval_, 0);
  }
  fclose(of);
  return fname;
}
#endif

int Region::geo2pic(float x, float y, int *rx, int *ry) {
  float f1, f2;
  /* f1 stores the geocoordinate spacing of the x-axis, geocoord[0] (left) < geocoord[2] (right)
     f2 stores the distance of the test point x to the (left) geocoord[0] */
  f1 = geoEast_ - geoWest_;
  f2 = x - geoWest_;

  if (f1 < 0)
  {
    fprintf(stderr, "Error! geo2pic - order of x-geocoordinates wrong\n");
    return(-2);
  }
  if (f2 > f1 || f2 < 0)
  {
    fprintf(stderr, "Error! geo2pic - x value out of range\n");
    return(-1);
  }

  *rx = (int)(f2 / f1 * (float)cols_ + 0.5);

  /* f1 stores the geocoordinate spacing of the y-axis, geocoord[1] (top) > geocoord[3] (bottom)
     f2 stores the distance of the test point y to the (top) geocoord[1] */
  f1 = geoNorth_ - geoSouth_;
  f2 = geoNorth_ - y;

  if (f1 < 0)
  {
    fprintf(stderr, "Error! geo2pic - order of y-geocoordinates wrong\n");
    return(-2);
  }
  if (f2 > f1 || f2 < 0)
  {
    fprintf(stderr, "Error! geo2pic - y value out of range\n");
    return(-1);
  }

  *ry = (int)(f2 / f1 * (float)rows_ + 0.5);

  return(0);
}

/** write data to file */
void Region::write(QTextStream& fp, QString keyword) {
  if (keyword=="region") {
    QDictIterator<QString> it(*this);
    fp <<"  <" << keyword << " ";
    for (;it.current(); ++it) {
      if (! it.currentKey().contains("file_geo",false)) 
	fp<<it.currentKey().latin1()<<"=\""<<(it.current())->latin1()<<"\" ";
    }
    fp <<" />"<<endl;
  }
  else {
    QDictIterator<QString> it(*this);
    fp <<"  <" << keyword << " ";
    for (;it.current(); ++it) {
      fp<<it.currentKey().latin1()<<"=\""<<(it.current())->latin1()<<"\" ";
    }
    fp <<" />"<<endl;
  }
}
/** return stack - for bottom-up */
Stack& Region::stack(void){
  return stack_;
}
/** Removes the top item from the local stack and returns it. */
StackElem* Region::stackPop(){
 return stack_.pop();
}
/** Adds an element d to the top of the local stack. */
void Region::stackPush(const StackElem* d){
  stack_.push(d);
}
/** Returns the number of items in the local stack.  */
uint Region::stackCount(void){
  return stack_.count();
}
/** Removes all items from the local stack, deleting them if autoDelete() is TRUE.  */
void Region::stackClear(void){
  stack_.clear();
}
/** Removes the top item from the local stack and deletes it if autoDelete() is TRUE. Returns TRUE if there was an item to pop;
otherwise returns FALSE.  */
bool Region::stackRemove (void) {
  return stack_.remove();
}
/** Returns TRUE is the local stack contains no elements to be popped; otherwise returns FALSE.  */
bool Region::stackIsEmpty (){
  return stack_.isEmpty();
}
