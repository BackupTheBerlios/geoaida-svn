/***************************************************************************
                          regionlist.cpp  -  description
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

using namespace std;

//  #define DEBUGMSG

#include "regionlist.h"
#include "gaimage.h"
//#include "gaimaget.h"
#include "garegion.h"
//#include "garegiont.h"

using namespace Ga;

QDict<Ga::Image> RegionList::imageDict_;

/** default constructor */
RegionList::RegionList()
{
  setAutoDelete(FALSE);
#if 0
  p_=0.0;
#endif
  gW_=gN_=gE_=gS_=0.0;
}

/** Copy constructor */
RegionList::RegionList(const RegionList& nl) : QDict<Region>(nl)
{
qDebug("### RegionList::RegionList(RegionList& nl)");
  copy(nl,true);
}


/** Copy constructor */
RegionList::RegionList(const RegionList& nl, bool deep) : QDict<Region>(nl)
{
  qDebug("### RegionList::RegionList(RegionList& nl, deep=%d)",deep);
  copy(nl,deep);
}

RegionList& RegionList::operator=(const RegionList& nl)
{
  if (this!=&nl) copy(nl,true);
  return *this;
}

void RegionList::copy(const RegionList& nl, bool deep)
{
qDebug("### RegionList::copy(RegionList& nl, deep=%d)",deep);
  QDict<Region>::operator=(nl);
  if (deep) {
    list_=nl.list_;
    maxID_=nl.maxID_;
    stack_=nl.stack_;
    attribList_=nl.attribList_;
  }
#if 0
  p_=nl.judgement();
#endif
  gW_=nl.gW_;
  gN_=nl.gN_;
  gE_=nl.gE_;
  gS_=nl.gS_;
  xRes_=nl.xRes_;
  yRes_=nl.yRes_;
  sizeX_=nl.sizeX_;
  sizeY_=nl.sizeY_;
  maxID_=nl.maxID_;
}

/** constructor resd from the provided filename*/
RegionList::RegionList(QString filename) {
  read(filename);
#if 0
  p_=0.0;
#endif
}

/** denstructor */
RegionList::~RegionList()
{
}

/** read a list of Region and the attributes from the provided filename */
void RegionList::read(QString filename) {
  QFile fp(filename);
  if (!fp.open(IO_ReadOnly)) {
    qDebug("RegionList::read(%s): file not founed\n",(const char*)filename);
    return;
  }
	read(fp);
	fp.close();
}

/** read a list of Region and the attributes from the provided filepointer */
void RegionList::read(QIODevice& fp) {
  MLParser parser(&fp);
  read(parser);
}

/** set regionlist attribute (group attribute) */
void RegionList::set(QString name, QString val)
{
  attribList_.replace(name,val);
}

/** get regionlist attribute (group attribute) */
QString RegionList::get(QString name)
{
  QString *val=attribList_[name];
  if (val) return *val;
  else {
    cerr << "Warning: Attribute not set in regionlist: " << name << endl;
    return "";
  }
}

/** calculate new geo coordinates and resolution using all images belong to included regions*/
void RegionList::calcNewGEOValues(float &gN,float &gS,float &gW,float &gE,
				int &sizeX,int &sizeY,float &xRes,float &yRes) {
  sizeX = sizeY = 0;
  gN = gS = gW = gE = 0.0;
  xRes=0;
  yRes=0;
  if (isEmpty()) return;
  Region* region;
  QStringList::Iterator it = list_.begin();
  region = find(*it);
  if (region) { //initialize variables
    gN=region->geoNorth();
    gS=region->geoSouth();
    gW=region->geoWest();
    gE=region->geoEast();
    yRes = (gN-gS) / float(region->sizeY()); //(region->lly() - region->ury()); //resolution
    xRes = (gE-gW) / float(region->sizeX()); //(region->urx() - region->llx()); //resolution
  }
  for ( it = list_.begin(); it != list_.end(); ++it ) {
    region = find(*it);
    if (gN < region->geoNorth()) gN = region->geoNorth();// calculate coordinates of the corner points
    if (gS > region->geoSouth()) gS = region->geoSouth();
    if (gW > region->geoWest()) gW = region->geoWest();
    if (gE < region->geoEast()) gE = region->geoEast();
    float xr = (region->geoEast()-region->geoWest()) / float(region->sizeX());//calculate resolution
    float yr = (region->geoNorth()-region->geoSouth()) / float(region->sizeY());
    if (xr < xRes) xRes = xr;
    if (yr < yRes) yRes = yr;
    qDebug("#+#+#geoNorth: %f,geoSouth: %f,geoWest: %f, geoEast: %f, rows: %d, cols: %d, lly: %d, ury: %d, llx: %d, urx: %d, dy: %f, dx: %f (%f,%f)",
           region->geoNorth(),region->geoSouth(),region->geoWest(),region->geoEast(),
           region->sizeX(),region->sizeY(), region->lly(), region->ury(), region->llx(), region->urx(),
           yr, xr,xRes,yRes);
  }
  if (xRes==0.0 || yRes==0.0) return;
  sizeX = int((gE-gW) / xRes+0.5);//calculate new image size
  sizeY = int((gN-gS) / yRes+0.5);//calculate new image size
  qDebug("#+#+#newX: %d, newY: %d, xRes: %f, yRes: %f, gN: %f, gS: %f, gE: %f, gW: %f",
	 sizeX, sizeY, xRes, yRes, gN, gS, gE, gW);
}

/** read a list of Region and the attributes through parser */
void RegionList::read(MLParser& parser) {
  qDebug("RegionList::read(parser)");
  list_.clear();
  QString keywords[]={"region","region",""};
  const MLTagTable regionTagTable(keywords);
  const int TOK_REGION=1;
  const int TOK_REGION=2;
  const int TOK_ARG_LIST=3;
  int tag;
  do {
    tag=parser.tag(regionTagTable);
    switch (tag) {
    case TOK_REGION:
    case TOK_REGION: {
      Region* gi=new Region(parser); //parse region
      if (gi->filename()=="") {
	qDebug("RegionList::read() Missing filename");
	delete gi;
	break;
      }
      this->insert(gi->key(),gi); //insert read region
      qDebug("####### %s (%s)",gi->key().latin1(),gi->name().latin1());
      break;
    }
    default: {
      ArgDict* args=parser.args();
      delete args;
      break;
    }
    }
  } while ((tag!=MLParser::END_OF_FILE ) && (tag != -TOK_ARG_LIST));
  
  qDebug("RegionList::read: num regions=%d",count());
  if (isEmpty()) return;
  //test images and create a structure for the images
  QString key;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    key = *it;
    Region *img = find(key);
    //qDebug("## RegionList::read(parser)" + img->filename());
    img->load(*this);
  }
  calcNewGEOValues(gN_,gS_,gW_,gE_,sizeX_, sizeY_, xRes_,yRes_);
  qDebug("$$read gN_:%f gS_:%f gW_:%f gE_:%f sizeX_:%d sizeY_:%d xRes_:%f yRes_:%f ",gN_,gS_,gW_,gE_,sizeX_, sizeY_, xRes_,yRes_);
  
  outImage_=Image(typeid(signed int),sizeX_,sizeY_,1);
  outImage_.setGeoCoordinates(gW_, gN_, gE_, gS_);
}

/** operator +=  for RegionList*/
RegionList& RegionList::operator += (RegionList& nl){
	QDictIterator<Region> it( nl );
	while ( it.current() ) {
		this->insert(it.currentKey().latin1(),it.current());
		++it;
	}
	return *this;
}

/** return void pointer to the Region class */
Region* RegionList::region(QString imgKey) {
  return find(imgKey);
}
/** return void pointer to the image data */
void* RegionList::data(QString imgKey) {
  return (find(imgKey))->data();
}
/** return the cols numbers of the image */
int RegionList::cols(QString imgKey) {
  return (find(imgKey))->cols();
}
/** return the rows numbers of the image */
int RegionList::rows(QString imgKey) {
  return (find(imgKey))->rows();
}
/** return geoNorth value of the image*/
float RegionList::geoNorth(QString imgKey) {
  return (find(imgKey))->geoNorth();
}
/** return geoSouth value of the image*/
float RegionList::geoSouth(QString imgKey) {
  return (find(imgKey))->geoSouth();
}
/** return geoEast value of the image*/
float RegionList::geoEast(QString imgKey) {
  return (find(imgKey))->geoEast();
}
/** return geoWest  valueof the image*/
float RegionList::geoWest(QString imgKey) {
  return (find(imgKey))->geoWest();
}
/** return Info the image*/
void RegionList::info(QString imgKey) {
  (find(imgKey))->info();
}

#if 0
//** write a scrap of the data to a file*/
QString RegionList::part(QString imgKey, float north, float south, float west, float east,
  QString fname) {
  if (find(imgKey))
    return (find(imgKey))->part(north, south, west, east, fname);
  else
    qWarning("## (Warning) RegionList::part(%s, %f, %f, %f, %f) no entry",
          (const char*)imgKey, north, south, west, east);
}
#endif

/** print info  */
void  RegionList::info(void){
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    Region *region = find(*it);
		region->info();
		printf("\n");
  }

}

void RegionList::insert(QString key, Region* region){
	QDict<Region>::replace( key, region );//insert region
//	QDict<Region>::insert( key, region );//insert region
	list_ += key; //fill additional list of image names
}

/** return a list of pointer to the objects of specified class type */
RegionList* RegionList::selectClass (QString classname){
  RegionList* nl = new RegionList;
  Region* region;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    region = find(*it);
    if (!region) {
      qDebug("RegionList::selectClass: %s Warning: %s not found in regionlist",
	     classname.latin1(),(*it).latin1());
      continue;
    }
    if (classname.compare(region->classname()) == 0) {
      nl->insert (region->key(), region );//insert region
    }
  }
  return nl;
}

/** calculate for all classes 'classname' the term 'term' with the function 'fkt'.
		Fkt. return int*/
void RegionList::calcForSelect (QString classname, QString term, int (*fkt)(void)) {
  Region* region;
  //RegionList* nl = selectClass(classname);
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    region = find(*it);
    if (!region) {
      qDebug("RegionList::calcForSelect: region %s not found",(*it).latin1());
      continue;
    }
    if (classname.compare(region->classname()) == 0) {
      QString *val = new QString;
      val->setNum(fkt());
      if (!find(term)) region->insert(term, val);
      else qWarning("##. (Warning) RegionList::calcForSelect: %s - exist",term.latin1());
    } //else qDebug("## %s ##",region->classname().latin1());
  }
}

/** calculate for all classes 'classname' the term 'term' with the function 'fkt'.*/
void RegionList::calcForSelect (QString classname, QString term, QString (*fkt)(void)) {
  Region* region;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    region = find(*it);
    if (!region) {
      qDebug("RegionList::calcForSelect: region %s not found",(*it).latin1());
      continue;
    }
    if (classname.compare(region->classname()) == 0) {
      QString *val = new QString;
      *val=fkt();
      if (!find(term)) region->insert(term, val);
      else qDebug("## (Warning) RegionList::calcForSelect: %s - exist",term.latin1());
    } //else qDebug("## .%s. ##",region->classname().latin1());
  }
}

/** calculate for all classes 'classname' the term 'term' with the fun*/
void RegionList::calcForSelect (QString classname, QString term, GASensor* sensor) {
  Region* region;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    region = find(*it);
    if (!region) {
      qDebug("RegionList::calcForSelect: region %s not found",(*it).latin1());
      continue;
    }
    if (classname.compare(region->classname())==0 || classname.compare("ALL")==0) {
      Image* img=region->data();
      if (!img) {
	cerr << "RegionList::calcForSelect: region data not read" << endl;
	exit(1);
      }
      sensor->image(*img);
#if 0		
      sensor->readImage(*((*region)["file"])); //["labelpic"]));
#endif
      sensor->region(region);
      sensor->calc(term);
    } else qDebug("## .%s. ##",region->classname().latin1());
  }
}

/** calculate for all classes 'classname' the term 'term' with the fun*/
void RegionList::calcForSelect (QString term, GASensor* sensor) {
  Region* region;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    region = find(*it);
    if (!region) {
      qDebug("RegionList::calcForSelect: region %s not found",(*it).latin1());
      continue;
    }
    //if (classname.compare(region->classname())==0 || classname.compare("ALL")==0) {
    Image* img=region->data();
    if (!img) {
      cerr << "RegionList::calcForSelect: region data not read" << endl;
      exit(1);
    }
    sensor->image(*img);
#if 0
    sensor->readImage(*((*region)["file"])); //["labelpic"]));
#endif
    sensor->region(region);
    sensor->calc(term);
    //} else qDebug("## .%s. ##",region->classname().latin1());
  }
}

/** merge for all images in list (scale to a common resulution)
* write result to file 'fname'
  flag 'newReg'
    true - generate new regions when regions are split into subregions
    false - one region can associate to more than on region
*/
//RegionList* RegionList::merge (QString outImgName, bool newReg = 0) {
RegionList* RegionList::merge (bool newReg, QString outImgName) {
  //calc new geo coordinates using all regions of liste 'this'
  if (gN_==0.0 || gS_==0.0) calcNewGEOValues(gN_,gS_,gW_,gE_,sizeX_, sizeY_, xRes_,yRes_);
  qDebug("$#$merge gN_:%f gS_:%f gW_:%f gE_:%f sizeX_:%d sizeY_:%d xRes_:%f yRes_:%f ",gN_,gS_,gW_,gE_,sizeX_,sizeY_,xRes_,yRes_);
  
  //generate a sorted list of regions using 'p' the weighing
  SortPtrList sortlist;
  QDictIterator<Region> it( *this ); //iterator for regionlist
  it.toFirst();
  while(it.current()) {
    Region *region = it.current();
    //float ff = ((*region)["p"])->toFloat();
    //SortElement* e = new SortElement(it.current(),(it.current())->p(),it.currentKey());
    SortElement* e;
    if (region->find( "p" ))
      e = new SortElement(it.current(),((*region)["p"])->toFloat(),it.currentKey());
    else
      e = new SortElement(it.current(),(it.current())->p(),it.currentKey());
    sortlist.append(e);
    ++it;
  }
  sortlist.sort();
  
  //merge and relabel the images of all regions in regionlist
  int freeID=maxID_;
  maxID_++; //label count -> all label are relabeled / label 1 is reserved
  SortElement *se;
  int nllx=0, nlly=0, nurx=0, nury=0;
  int changeVec[sortlist.count()+maxID_]; //vektor, um IDs der veränderten regionen zu erfassen
  for (unsigned int j=0; j<sortlist.count()+maxID_; j++)changeVec[j]=0;//initialisierung
  qDebug("RegionList::merge: changeVec-size %d",  sortlist.count()+maxID_);
  for ( se=sortlist.first(); se != 0; se=sortlist.next() ) {
    Region *region=se->region();
    qDebug("### merge: p=%f, l_id=%d, n_id=%d, read:%s",se->p(), region->id(),maxID_, region->filename().latin1());
    Image& img=readLabelFile(region->filename(),region->geoWest(), region->geoNorth(), region->geoEast(), region->geoSouth());
    if (img.isEmpty()) continue;
    qDebug("RegionList::merge: geoMerge: %4d %4d %4d %4d",
	   region->llx(),region->lly(),region->urx(),region->ury());
    
    outImage_.geoMerge(img, region->id(), maxID_,
                       region->llx(),region->lly(),region->urx(),region->ury(),
                       &nllx, &nlly, &nurx, &nury,
                       (int*)(&changeVec));//merge to out image
    qDebug("Done                        %4d %4d %4d %4d",
	   nllx,nlly,nurx,nury);
    region->id(maxID_);
    se->setGeoPos(nllx, nlly, nurx, nury);
    maxID_++;
  } //variable maxID is used later ([2..maxID-1] = count_of_old_regions)
  outImage_.write(outImgName);	//NEW
  
  //calculate new Region attibutes and look for divided regions
  RegionList* selected=new RegionList(*this, false);// new regionlist for result, no deep copy
  //  int *pic=(int*)outImage_.begin();
  for ( se=sortlist.first(); se != 0; se=sortlist.next() ) {
    Region *region=se->region();
    int oldID = region->id(); //urspruengliche ID der region - bleibt in der Schleife immer gleich
    QString addr = region->addr(); //daten zum uebertragen speichern
    QString classname = region->classname();
    QString name = region->name();
    float p =region->p();
    if (region->find( "p" )) p=((*region)["p"])->toFloat();
    region->clear(); //daten loeschen
    region->classname(classname);//neue daten eintragen
    region->filename(outImgName);//region->setValue("file",outImgName);
    region->setValue("addr",addr);
    region->setValue("name",name);
    qDebug("### merge: GEO id=%d; gn=%f, gs=%f, ge=%f, gw=%f, p=%f",region->id(),gN_,gS_,gE_,gW_,p);
    region->replace("p",p);
    region->replace("id",region->id());
    region->replace("file_geoNorth",gN_);
    region->replace("file_geoSouth",gS_);
    region->replace("file_geoEast", gE_);
    region->replace("file_geoWest", gW_);
    region->load(*this);
    region->update();
//region->info();
    if(changeVec)
      if(changeVec[oldID]){ //veraendert?
        region->setValue("change",(new QString)->sprintf("true"));
        if(!newReg) selected->insert(region->key(),region);			
        else {
          for (int y=se->ury; y<=se->lly; y++)
            for (int x=se->llx; x<=se->urx; x++) {
              if (outImage_.getInt(x,y)==oldID) {
                if(oldID != freeID) {
                  int newID=freeID;
                  label4(outImage_, y*outImage_.sizeX()+x, oldID,  newID);
//                  label4(pic, outImage_.sizeX(), outImage_.sizeY(), y*outImage_.sizeX()+x, oldID,  newID);
                  region->id(newID);
                  freeID=oldID;
                  selected->insert(region->key(),region);

                } else {//es sind neue regionen entstanden
                  label4(outImage_, y*outImage_.sizeX()+x, oldID,  maxID_);
                  Region* newRegion = new Region; //neuer Knoten
                  newRegion->id(maxID_); //neue daten eintragen
                  newRegion->classname(classname);
                  //newRegion->replace("file",outImgName);
                  region->filename(outImgName);//region->setValue("file",outImgName);
                  newRegion->replace("status","HI");
                  newRegion->replace("name",name);
                  region->replace("id",region->id());
                  newRegion->replace("file_geoNorth",gN_);
                  newRegion->replace("file_geoSouth",gS_);
                  newRegion->replace("file_geoEast", gE_);
                  newRegion->replace("file_geoWest", gW_);
                  newRegion->replace("addr",QString().sprintf("%s#%d",(const char*)newRegion->name(),maxID_));
                  newRegion->replace("change","true");
                  newRegion->load(*this);
                  //newRegion->info();
                  region->update();
                  selected->insert((new QString)->sprintf("%d",maxID_),newRegion);
                  maxID_++;
                }
              }
            }
        }
      }
      else selected->insert(region->key(),region);//unveraenderte einfuegen
  }

//  qDebug("maxId=%d",maxID_);
  Image bb = calcBoundingBoxes(outImage_, maxID_);
  for ( se=sortlist.first(); se != 0; se=sortlist.next() ) {//set bounding box values
    Region *region=se->region();
//    qDebug("id=%4d, bbox=%4d %4d %4d %4d",
//    region->id(),
//    bb.getInt(region->id(),0),
//    bb.getInt(region->id(),1),
//    bb.getInt(region->id(),2),
//    bb.getInt(region->id(),3));
#ifdef WIN32
    qDebug("regionlist.cpp 459:llx, llyl urx, ury");

//Was denn??
  /** get a pixel */
//  int getInt(int x, int y, int channel=0);
  /** get a pixel */
//  int getInt(const void *ptr);
//Quelltyp konnte von keinem Konstruktor angenommen werden, oder die Ueberladungsaufloesung des Konstruktors ist mehrdeutig
    region->replace("llx",&QString((float)(bb.getInt(region->id(),0))));
    region->replace("lly",&QString((float)(bb.getInt(region->id(),1))));
    region->replace("urx",&QString((float)(bb.getInt(region->id(),2))));
    region->replace("ury",&QString((float)(bb.getInt(region->id(),3))));
#else
    region->replace("llx",bb.getInt(region->id(),0));
    region->replace("lly",bb.getInt(region->id(),1));
    region->replace("urx",bb.getInt(region->id(),2));
    region->replace("ury",bb.getInt(region->id(),3));
#endif
    // XXX Was ist mit voellig ueberdeckten regionen ??
    if( bb.getInt(region->id(),0)==0 && bb.getInt(region->id(),1)==0 &&
        bb.getInt(region->id(),2)==0 && bb.getInt(region->id(),3)==0 )
      selected->remove(region->key());
    region->update();
  }
  //outImage_.write(outImgName);
  return selected;
}

/** write a regionlist to the given textstream */
void RegionList::write(QTextStream &fp, QString groupFileName)
{
  QDictIterator<Region> it(*this);
  if (it.current()) {
    fp << "<group ";
    if (!groupFileName.isEmpty())
      fp << "id=\"" << groupId_  << "\" file=\"" << groupFileName << "\" ";
    fp << attribList_;
    fp.precision(8);
    fp << "file_geoNorth=\"" <<gN_<< "\"file_geoWest=\"" <<gW_<< "\"file_geoEast=\"" <<gE_<< "\"file_geoSouth=\"" <<gS_<< "\"";
    fp << " >" << endl;
    for (;it.current();++it) {
      (*it)->write(fp);
    }
    
    fp << "</group>" << endl;
  }
}

/** write a regionlist as a regionsfile */
void RegionList::writeRegionFile(QTextStream &fp)
{
  QDictIterator<Region> it(*this);
  if (it.current()) {
    for (;it.current();++it) {
#ifdef WIN32
      (*it).write(fp,"region");
#else
   (*it)->write(fp,"region");
#endif
    }
  }

}

Image& RegionList::readLabelFile(QString filename, double gW, double gN, double gE, double gS)
{
  Image *im = imageDict_[filename];
  if (im)
    return *im;                  // image found in dictionary
#ifdef alt
  im = new Image();
  im->read(filename.latin1());
  im->setGeoCoordinates(gW, gN, gE, gS);
  imageDict_.insert(filename, im);
#else
  printf("###### read new\t%s\n",filename.latin1());
  im = new Image(typeid(int));
  im->read(filename.latin1()); //DAS GEHT HIER NICHT!!!!!!!!!!!!!!!!!!!!!!!!
  if (!im->isEmpty()) {//ODER DAS !!!!!!!!!!!!!!!!!!!!!!!!
    im->setGeoCoordinates(gW, gN, gE, gS);
    imageDict_.replace(filename, im);
  }
#endif
  return *im;
}

/** write the resulting label image */
void RegionList::writeOutImage(QString filename)
{
  if (outImage_.isEmpty()) return;
  outImage_.write(filename);
}

/** write the resulting group label image */
void RegionList::writeGroupImage(QString filename)
{
  if (groupImage_.isEmpty()) return;
  groupImage_.write(filename);
}

void RegionList::genGroupImage()
{
  if (!groupIdCounter_) {
    groupImage_=Image(outImage_.typeId(),outImage_.sizeX(),outImage_.sizeY());
  }
  groupId_=++groupIdCounter_;
  QDictIterator<Region> it( *this );
  for (;it.current();++it) {
    Region* region=it.current();
    int id=region->id();
    int llx=region->llx();
    int urx=region->urx();
    if (urx>=outImage_.sizeX()) urx=outImage_.sizeX()-1;
    int lly=region->lly();
    if (lly>=outImage_.sizeY()) lly=outImage_.sizeY()-1;
    int ury=region->ury();
    for (int y=ury; y<=lly; y++) {
      void *ptr=outImage_.begin(y,0);
      outImage_.nextCol(ptr,llx);
      for (int x=llx; x<=urx; x++) {
        int v=outImage_.getInt(ptr);
        if (v==id) {
          groupImage_.set(x,y,groupIdCounter_);
	}
	outImage_.nextCol(ptr);
      }
    }
    
  }
}

/** return stack - for bottom-up */
Stack& RegionList::stack(void){
  return stack_;
}
/** Removes the top item from the local stack and returns it. */
StackElem* RegionList::stackPop(){
 return stack_.pop();
}
/** Adds an element d to the top of the local stack. */
void RegionList::stackPush(const StackElem* d){
  stack_.push(d);
}
/** Returns the number of items in the local stack.  */
uint RegionList::stackCount(void){
  return stack_.count();
}
/** Removes all items from the local stack, deleting them if autoDelete() is TRUE.  */
void RegionList::stackClear(void){
  stack_.clear();
}
/** Removes the top item from the local stack and deletes it if autoDelete() is TRUE. Returns TRUE if there was an item to pop;
otherwise returns FALSE.  */
bool RegionList::stackRemove (void) {
  return stack_.remove();
}
/** Returns TRUE is the local stack contains no elements to be popped; otherwise returns FALSE.  */
bool RegionList::stackIsEmpty (){
  return stack_.isEmpty();
}

int RegionList::maxID_=1;
int RegionList::groupIdCounter_=0;
Image RegionList::outImage_;
Image RegionList::groupImage_;
/** copy the regions in this regionlist into the group image */

/** set the out-image-file-name in all regions of REGIONLIST */
void RegionList::setImgName(QString outImgName){
  QDictIterator<Region> it( *this ); //iterator for regionlist
  it.toFirst();
  while(it.current()) {
    Region *region = it.current();
#ifdef WIN32
    region->replace("file",&outImgName);
#else
    region->replace("file",outImgName);
#endif
    ++it;
  }
}

