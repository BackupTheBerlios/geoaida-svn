/***************************************************************************
                          nodelist.cpp  -  description
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


//  #define DEBUGMSG

#include "nodelist.h"
#include "gaimage.h"
#include "garegion.h"

using namespace std;
using namespace Ga;
using namespace BottomUpLib;

QHash<QString, Ga::Image*> NodeList::imageDict_;

/** default constructor */
NodeList::NodeList()
{
  //  setAutoDelete(FALSE);
#if 0
  p_=0.0;
#endif
  gW_=gN_=gE_=gS_=0.0;
}

/** Copy constructor */
NodeList::NodeList(const NodeList& nl) : QHash<QString, BottomUpLib::Node*>(nl)
{
qDebug("### NodeList::NodeList(NodeList& nl)");
  copy(nl,true);
}


/** Copy constructor */
NodeList::NodeList(const NodeList& nl, bool deep) : QHash<QString, BottomUpLib::Node*>(nl)
{
  qDebug("### NodeList::NodeList(NodeList& nl, deep=%d)",deep);
  copy(nl,deep);
}

NodeList& NodeList::operator=(const NodeList& nl)
{
  if (this!=&nl) copy(nl,true);
  return *this;
}

void NodeList::copy(const NodeList& nl, bool deep)
{
qDebug("### NodeList::copy(NodeList& nl, deep=%d)",deep);
  QHash<QString, BottomUpLib::Node*>::operator=(nl);
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
NodeList::NodeList(QString filename) {
  read(filename);
#if 0
  p_=0.0;
#endif
}

/** denstructor */
NodeList::~NodeList()
{
}

/** read a list of Node and the attributes from the provided filename */
void NodeList::read(QString filename) {
  QFile fp(filename);
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("NodeList::read(%s): file not founed\n",filename.toLatin1().constData());
    return;
  }
	read(fp);
	fp.close();
}

/** read a list of Node and the attributes from the provided filepointer */
void NodeList::read(QIODevice& fp) {
  MLParser parser(&fp);
  read(parser);
}

/** set nodelist attribute (group attribute) */
void NodeList::set(QString name, QString val)
{
  attribList_.replace(name,val);
}

/** get nodelist attribute (group attribute) */
QString NodeList::get(QString name)
{
  if (!attribList_.contains(name)) {
    cerr << "Warning: Attribute not set in nodelist: " 
	 << name.toLatin1().constData() << endl;
    return "";
  }  
  return attribList_.value(name);
}

/** calculate new geo coordinates and resolution using all images belong to included nodes*/
void NodeList::calcNewGEOValues(float &gN,float &gS,float &gW,float &gE,
				int &sizeX,int &sizeY,float &xRes,float &yRes) {
  sizeX = sizeY = 0;
  gN = gS = gW = gE = 0.0;
  xRes=0;
  yRes=0;
  if (isEmpty()) return;
  QStringList::ConstIterator it = list_.begin();
  BottomUpLib::Node* node = value(*it);
  if (node) { //initialize variables
    gN=node->geoNorth();
    gS=node->geoSouth();
    gW=node->geoWest();
    gE=node->geoEast();
    yRes = (gN-gS) / float(node->sizeY()); //(node->lly() - node->ury()); //resolution
    xRes = (gE-gW) / float(node->sizeX()); //(node->urx() - node->llx()); //resolution
  }
  for ( it = list_.begin(); it != list_.end(); ++it ) {
    node = value(*it);
    if (gN < node->geoNorth()) gN = node->geoNorth();// calculate coordinates of the corner points
    if (gS > node->geoSouth()) gS = node->geoSouth();
    if (gW > node->geoWest()) gW = node->geoWest();
    if (gE < node->geoEast()) gE = node->geoEast();
    float xr = (node->geoEast()-node->geoWest()) / float(node->sizeX());//calculate resolution
    float yr = (node->geoNorth()-node->geoSouth()) / float(node->sizeY());
    if (xr < xRes) xRes = xr;
    if (yr < yRes) yRes = yr;
    qDebug("#+#+#geoNorth: %f,geoSouth: %f,geoWest: %f, geoEast: %f, rows: %d, cols: %d, lly: %d, ury: %d, llx: %d, urx: %d, dy: %f, dx: %f (%f,%f)",
           node->geoNorth(),node->geoSouth(),node->geoWest(),node->geoEast(),
           node->sizeX(),node->sizeY(), node->lly(), node->ury(), node->llx(), node->urx(),
           yr, xr,xRes,yRes);
  }
  if (xRes==0.0 || yRes==0.0) return;
  sizeX = int((gE-gW) / xRes+0.5);//calculate new image size
  sizeY = int((gN-gS) / yRes+0.5);//calculate new image size
  qDebug("#+#+#newX: %d, newY: %d, xRes: %f, yRes: %f, gN: %f, gS: %f, gE: %f, gW: %f",
	 sizeX, sizeY, xRes, yRes, gN, gS, gE, gW);
}

/** read a list of Node and the attributes through parser */
void NodeList::read(MLParser& parser) {
  qDebug("NodeList::read(parser)");
  list_.clear();
  QString keywords[]={"node","region",""};
  const MLTagTable nodeTagTable(keywords);
  const int TOK_NODE=1;
  const int TOK_REGION=2;
  const int TOK_ARG_LIST=3;
  int tag;
  do {
    tag=parser.tag(nodeTagTable);
    switch (tag) {
    case TOK_NODE:
    case TOK_REGION: {
      BottomUpLib::Node* gi=new BottomUpLib::Node(parser); //parse node
      if (gi->filename()=="") {
	qDebug("NodeList::read() Missing filename");
	delete gi;
	break;
      }
      this->insert(gi->key(),gi); //insert read node
      qDebug("####### %s (%s)",
	     gi->key().toLatin1().constData(),
	     gi->name().toLatin1().constData());
      break;
    }
    default: {
      ArgDict* args=parser.args();
      delete args;
      break;
    }
    }
  } while ((tag!=MLParser::END_OF_FILE ) && (tag != -TOK_ARG_LIST));
  
  qDebug("NodeList::read: num nodes=%d",count());
  if (isEmpty()) return;
  //test images and create a structure for the images
  QString key;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    key = *it;
    BottomUpLib::Node *img = value(key);
    //qDebug("## NodeList::read(parser)" + img->filename());
    img->load(*this);
  }
  calcNewGEOValues(gN_,gS_,gW_,gE_,sizeX_, sizeY_, xRes_,yRes_);
  qDebug("$$read gN_:%f gS_:%f gW_:%f gE_:%f sizeX_:%d sizeY_:%d xRes_:%f yRes_:%f ",gN_,gS_,gW_,gE_,sizeX_, sizeY_, xRes_,yRes_);
  
  outImage_=Image(typeid(signed int),sizeX_,sizeY_,1);
  outImage_.setGeoCoordinates(gW_, gN_, gE_, gS_);
}

/** operator +=  for NodeList*/
NodeList& NodeList::operator += (NodeList& nl){
  for (ConstIterator it=nl.begin(); it!=end(); ++it) {
    insert(it.key(), it.value());
  }
  return *this;
}

/** return void pointer to the Node class */
BottomUpLib::Node* NodeList::node(QString imgKey) {
  return value(imgKey);
}
/** return void pointer to the image data */
void* NodeList::data(QString imgKey) {
  return (value(imgKey))->data();
}
/** return the cols numbers of the image */
int NodeList::cols(QString imgKey) {
  return (value(imgKey))->cols();
}
/** return the rows numbers of the image */
int NodeList::rows(QString imgKey) {
  return (value(imgKey))->rows();
}
/** return geoNorth value of the image*/
float NodeList::geoNorth(QString imgKey) {
  return (value(imgKey))->geoNorth();
}
/** return geoSouth value of the image*/
float NodeList::geoSouth(QString imgKey) {
  return (value(imgKey))->geoSouth();
}
/** return geoEast value of the image*/
float NodeList::geoEast(QString imgKey) {
  return (value(imgKey))->geoEast();
}
/** return geoWest  valueof the image*/
float NodeList::geoWest(QString imgKey) {
  return (value(imgKey))->geoWest();
}
/** return Info the image*/
void NodeList::info(QString imgKey) {
  (value(imgKey))->info();
}

#if 0
//** write a scrap of the data to a file*/
QString NodeList::part(QString imgKey, float north, float south, float west, float east,
  QString fname) {
  if (value(imgKey))
    return (value(imgKey))->part(north, south, west, east, fname);
  else
    qWarning("## (Warning) NodeList::part(%s, %f, %f, %f, %f) no entry",
          (const char*)imgKey, north, south, west, east);
}
#endif

/** print info  */
void  NodeList::info(void){
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    BottomUpLib::Node* node = value(*it);
    node->info();
    printf("\n");
  }

}

void NodeList::insert(QString key, BottomUpLib::Node* node){
  insert( key, node );//insert node
//	QHash<QString, BottomUpLib::Node*>::insert( key, node );//insert node
  list_ += key; //fill additional list of image names
}

/** return a list of pointer to the objects of specified class type */
NodeList* NodeList::selectClass (QString classname){
  NodeList* nl = new NodeList;
  BottomUpLib::Node* node;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    node = value(*it);
    if (!node) {
      qDebug("NodeList::selectClass: %s Warning: %s not found in nodelist",
	     classname.toLatin1().constData(),(*it).toLatin1().constData());
      continue;
    }
    if (classname.compare(node->classname()) == 0) {
      nl->insert (node->key(), node );//insert node
    }
  }
  return nl;
}

/** calculate for all classes 'classname' the term 'term' with the function 'fkt'.
		Fkt. return int*/
void NodeList::calcForSelect (QString classname, QString term, int (*fkt)(void)) {
  BottomUpLib::Node* node;
  //NodeList* nl = selectClass(classname);
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    node = value(*it);
    if (!node) {
      qDebug("NodeList::calcForSelect: node %s not found",(*it).toLatin1().constData());
      continue;
    }
    if (classname.compare(node->classname()) == 0) {
      QString *val = new QString;
      val->setNum(fkt());
      if (!contains(term)) node->insert(term, val);
      else qWarning("##. (Warning) NodeList::calcForSelect: %s - exist",term.toLatin1().constData());
    } //else qDebug("## %s ##",node->classname().toLatin1().constData());
  }
}

/** calculate for all classes 'classname' the term 'term' with the function 'fkt'.*/
void NodeList::calcForSelect (QString classname, QString term, QString (*fkt)(void)) {
  BottomUpLib::Node* node;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    node = value(*it);
    if (!node) {
      qDebug("NodeList::calcForSelect: node %s not found",(*it).toLatin1().constData());
      continue;
    }
    if (classname.compare(node->classname()) == 0) {
      QString *val = new QString;
      *val=fkt();
      if (!contains(term)) node->insert(term, val);
      else qDebug("## (Warning) NodeList::calcForSelect: %s - exist",term.toLatin1().constData());
    } //else qDebug("## .%s. ##",node->classname().toLatin1().constData());
  }
}

/** calculate for all classes 'classname' the term 'term' with the fun*/
void NodeList::calcForSelect(QString classname, 
			       QString term, 
			       Sensor* sensor) {
  BottomUpLib::Node* node;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    node = value(*it);
    if (!node) {
      qDebug("NodeList::calcForSelect: node %s not found",(*it).toLatin1().constData());
      continue;
    }
    if (classname.compare(node->classname())==0 || classname.compare("ALL")==0) {
      Image* img=node->data();
      if (!img) {
	cerr << "NodeList::calcForSelect: node data not read" << endl;
	exit(1);
      }
      sensor->image(*img);
#if 0		
      sensor->readImage(*((*node)["file"])); //["labelpic"]));
#endif
      sensor->node(node);
      sensor->calc(term);
    } else qDebug("## .%s. ##",node->classname().toLatin1().constData());
  }
}

/** calculate for all classes 'classname' the term 'term' with the fun*/
void NodeList::calcForSelect(QString term, 
			       Sensor* sensor) {
  BottomUpLib::Node* node;
  for ( QStringList::Iterator it = list_.begin(); it != list_.end(); ++it ) {
    node = value(*it);
    if (!node) {
      qDebug("NodeList::calcForSelect: node %s not found",(*it).toLatin1().constData());
      continue;
    }
    //if (classname.compare(node->classname())==0 || classname.compare("ALL")==0) {
    Image* img=node->data();
    if (!img) {
      cerr << "NodeList::calcForSelect: node data not read" << endl;
      exit(1);
    }
    sensor->image(*img);
#if 0
    sensor->readImage(*((*node)["file"])); //["labelpic"]));
#endif
    sensor->node(node);
    sensor->calc(term);
    //} else qDebug("## .%s. ##",node->classname().toLatin1().constData());
  }
}

/** merge for all images in list (scale to a common resulution)
* write result to file 'fname'
  flag 'newReg'
    true - generate new nodes when nodes are split into subnodes
    false - one node can associate to more than on node
*/
//NodeList* NodeList::merge (QString outImgName, bool newReg = 0) {
NodeList* NodeList::merge (bool newReg, QString outImgName) {
  //calc new geo coordinates using all nodes of liste 'this'
  if (gN_==0.0 || gS_==0.0) calcNewGEOValues(gN_,gS_,gW_,gE_,sizeX_, sizeY_, xRes_,yRes_);
  qDebug("$#$merge gN_:%f gS_:%f gW_:%f gE_:%f sizeX_:%d sizeY_:%d xRes_:%f yRes_:%f ",gN_,gS_,gW_,gE_,sizeX_,sizeY_,xRes_,yRes_);
  
  //generate a sorted list of nodes using 'p' the weighing
  SortPtrList sortlist;
  for (ConstIterator it=begin(); it!=end(); ++it) {
    BottomUpLib::Node* node = it.value();
    SortElement* e;
    if (node->contains( "p" ))
      e = new SortElement(node,
			  node->value("p").toFloat(),
			  it.key());
    else
      e = new SortElement(node,
			  node->p(),
			  it.key());
    sortlist.append(e);
    ++it;
  }
  qSort(sortlist.begin(), sortlist.end());
  
  //merge and relabel the images of all nodes in nodelist
  int freeID=maxID_;
  maxID_++; //label count -> all label are relabeled / label 1 is reserved
  int nllx=0, nlly=0, nurx=0, nury=0;
  int changeVec[sortlist.count()+maxID_]; //vektor, um IDs der veränderten nodeen zu erfassen
  for (unsigned int j=0; j<sortlist.count()+maxID_; j++)changeVec[j]=0;//initialisierung
  qDebug("NodeList::merge: changeVec-size %d",  sortlist.count()+maxID_);
  for (SortPtrList::Iterator se=sortlist.begin(); 
       se != sortlist.end(); 
       ++se ) {
    BottomUpLib::Node* node=(*se)->node();
    qDebug("### merge: p=%f, l_id=%d, n_id=%d, read:%s",
	   (*se)->p(), 
	   node->id(),
	   maxID_, 
	   node->filename().toLatin1().constData());
    Image& img=readLabelFile(node->filename(),
			     node->geoWest(),
			     node->geoNorth(), 
			     node->geoEast(),
			     node->geoSouth());
    if (img.isEmpty()) continue;
    qDebug("NodeList::merge: geoMerge: %4d %4d %4d %4d",
	   node->llx(),node->lly(),node->urx(),node->ury());
    
    outImage_.geoMerge(img, node->id(), maxID_,
                       node->llx(),node->lly(),node->urx(),node->ury(),
                       &nllx, &nlly, &nurx, &nury,
                       (int*)(&changeVec));//merge to out image
    qDebug("Done                        %4d %4d %4d %4d",
	   nllx,nlly,nurx,nury);
    node->id(maxID_);
    (*se)->setGeoPos(nllx, nlly, nurx, nury);
    maxID_++;
  } //variable maxID is used later ([2..maxID-1] = count_of_old_nodes)
  outImage_.write(outImgName.toLatin1());	//NEW
  
  //calculate new Node attibutes and look for divided nodes
  NodeList* selected=new NodeList(*this, false);// new nodelist for result, no deep copy
  //  int *pic=(int*)outImage_.begin();
  for (SortPtrList::Iterator se=sortlist.begin(); 
       se != sortlist.end(); 
       ++se ) {
    BottomUpLib::Node* node=(*se)->node();
    int oldID = node->id(); //urspruengliche ID der node - bleibt in der Schleife immer gleich
    QString addr = node->addr(); //daten zum uebertragen speichern
    QString classname = node->classname();
    QString name = node->name();
    float p =node->p();
    if (node->contains( "p" )) 
      p=node->value("p").toFloat();
    node->clear(); //daten loeschen
    node->classname(classname);//neue daten eintragen
    node->filename(outImgName);//node->setValue("file",outImgName);
    node->setValue("addr",addr);
    node->setValue("name",name);
    qDebug("### merge: GEO id=%d; gn=%f, gs=%f, ge=%f, gw=%f, p=%f",
	   node->id(),gN_,gS_,gE_,gW_,p);
    node->replace("p",p);
    node->replace("id",node->id());
    node->replace("file_geoNorth",gN_);
    node->replace("file_geoSouth",gS_);
    node->replace("file_geoEast", gE_);
    node->replace("file_geoWest", gW_);
    node->load(*this);
    node->update();
//node->info();
    if(changeVec)
      if(changeVec[oldID]){ //veraendert?
        node->setValue("change",(new QString)->sprintf("true"));
        if(!newReg) selected->insert(node->key(),node);			
        else {
          for (int y=(*se)->ury; y<=(*se)->lly; y++)
            for (int x=(*se)->llx; x<=(*se)->urx; x++) {
              if (outImage_.getInt(x,y)==oldID) {
                if(oldID != freeID) {
                  int newID=freeID;
                  label4(outImage_, y*outImage_.sizeX()+x, oldID,  newID);
//                  label4(pic, outImage_.sizeX(), outImage_.sizeY(), y*outImage_.sizeX()+x, oldID,  newID);
                  node->id(newID);
                  freeID=oldID;
                  selected->insert(node->key(),node);

                } else {//es sind neue nodeen entstanden
                  label4(outImage_, y*outImage_.sizeX()+x, oldID,  maxID_);
                  BottomUpLib::Node* newNode = new BottomUpLib::Node; //neuer Knoten
                  newNode->id(maxID_); //neue daten eintragen
                  newNode->classname(classname);
                  //newNode->replace("file",outImgName);
                  node->filename(outImgName);//node->setValue("file",outImgName);
                  newNode->replace("status","HI");
                  newNode->replace("name",name);
                  node->replace("id",node->id());
                  newNode->replace("file_geoNorth",gN_);
                  newNode->replace("file_geoSouth",gS_);
                  newNode->replace("file_geoEast", gE_);
                  newNode->replace("file_geoWest", gW_);
                  newNode->replace("addr",
				     QString().sprintf("%s#%d",
						       newNode->name().toLatin1().constData(),
						       maxID_));
                  newNode->replace("change","true");
                  newNode->load(*this);
                  //newNode->info();
                  node->update();
                  selected->insert((new QString)->sprintf("%d",maxID_),newNode);
                  maxID_++;
                }
              }
            }
        }
      }
      else selected->insert(node->key(),node);//unveraenderte einfuegen
  }

//  qDebug("maxId=%d",maxID_);
  Image bb = calcBoundingBoxes(outImage_, maxID_);
  for (SortPtrList::Iterator se=sortlist.begin(); 
       se != sortlist.end(); 
       ++se ) {
    //set bounding box values
    BottomUpLib::Node* node=(*se)->node();
//    qDebug("id=%4d, bbox=%4d %4d %4d %4d",
//    node->id(),
//    bb.getInt(node->id(),0),
//    bb.getInt(node->id(),1),
//    bb.getInt(node->id(),2),
//    bb.getInt(node->id(),3));
#ifdef WIN32
    qDebug("nodelist.cpp 459:llx, llyl urx, ury");

//Was denn??
  /** get a pixel */
//  int getInt(int x, int y, int channel=0);
  /** get a pixel */
//  int getInt(const void *ptr);
//Quelltyp konnte von keinem Konstruktor angenommen werden, oder die Ueberladungsaufloesung des Konstruktors ist mehrdeutig
    node->replace("llx",&QString((float)(bb.getInt(node->id(),0))));
    node->replace("lly",&QString((float)(bb.getInt(node->id(),1))));
    node->replace("urx",&QString((float)(bb.getInt(node->id(),2))));
    node->replace("ury",&QString((float)(bb.getInt(node->id(),3))));
#else
    node->replace("llx",bb.getInt(node->id(),0));
    node->replace("lly",bb.getInt(node->id(),1));
    node->replace("urx",bb.getInt(node->id(),2));
    node->replace("ury",bb.getInt(node->id(),3));
#endif
    // XXX Was ist mit voellig ueberdeckten nodeen ??
    if( bb.getInt(node->id(),0)==0 && bb.getInt(node->id(),1)==0 &&
        bb.getInt(node->id(),2)==0 && bb.getInt(node->id(),3)==0 )
      selected->remove(node->key());
    node->update();
  }
  //outImage_.write(outImgName);
  return selected;
}

/** write a nodelist to the given textstream */
void NodeList::write(QXmlStreamWriter &fp, QString groupFileName)
{
  if (!isEmpty()) {
    fp.writeStartElement("group");
    if (!groupFileName.isEmpty()) {
      fp.writeAttribute("id",QString::number(groupId_));
      fp.writeAttribute("file",groupFileName);
    }
    fp << attribList_;
    fp.writeAttribute("file_geoNorth",QString::number(gN_,'g',8));
    fp.writeAttribute("file_geoWest",QString::number(gW_,'g',8));
    fp.writeAttribute("file_geoEast",QString::number(gE_,'g',8));
    fp.writeAttribute("file_geoSouth",QString::number(gS_,'g',8));
    
    for (ConstIterator it=begin();it!=end();++it) {
      (*it)->write(fp);
    }
    
    fp.writeEndElement();
  }
}

/** write a nodelist as a nodesfile */
void NodeList::writeNodeFile(QXmlStreamWriter &fp)
{
  if (!isEmpty()) {
    for (ConstIterator it=begin();it!=end();++it) {
   (*it)->write(fp,"node");
    }
  }

}

Image& NodeList::readLabelFile(QString filename, double gW, double gN, double gE, double gS)
{
  Image *im = imageDict_[filename];
  if (im)
    return *im;                  // image found in dictionary
#ifdef alt
  im = new Image();
  im->read(filename);
  im->setGeoCoordinates(gW, gN, gE, gS);
  imageDict_.insert(filename, im);
#else
  printf("###### read new\t%s\n",filename.toLatin1().constData());
  im = new Image(typeid(int));
  im->read(filename.toLatin1()); //DAS GEHT HIER NICHT!!!!!!!!!!!!!!!!!!!!!!!!
  if (!im->isEmpty()) {//ODER DAS !!!!!!!!!!!!!!!!!!!!!!!!
    im->setGeoCoordinates(gW, gN, gE, gS);
    imageDict_.insert(filename, im);
  }
#endif
  return *im;
}

/** write the resulting label image */
void NodeList::writeOutImage(QString filename)
{
  if (outImage_.isEmpty()) return;
  outImage_.write(filename.toLatin1());
}

/** write the resulting group label image */
void NodeList::writeGroupImage(QString filename)
{
  if (groupImage_.isEmpty()) return;
  groupImage_.write(filename.toLatin1());
}

void NodeList::genGroupImage()
{
  if (!groupIdCounter_) {
    groupImage_=Image(outImage_.typeId(),outImage_.sizeX(),outImage_.sizeY());
  }
  groupId_=++groupIdCounter_;
  for (Iterator it=begin();
       it!=end();
       ++it) {
    BottomUpLib::Node* node=it.value();
    int id=node->id();
    int llx=node->llx();
    int urx=node->urx();
    if (urx>=outImage_.sizeX()) urx=outImage_.sizeX()-1;
    int lly=node->lly();
    if (lly>=outImage_.sizeY()) lly=outImage_.sizeY()-1;
    int ury=node->ury();
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
Stack& NodeList::stack(void){
  return stack_;
}
/** Removes the top item from the local stack and returns it. */
StackElement* NodeList::stackPop(){
 return stack_.pop();
}
/** Adds an element d to the top of the local stack. */
void NodeList::stackPush(StackElement* d){
  stack_.push(d);
}
/** Returns the number of items in the local stack.  */
uint NodeList::stackCount(void){
  return stack_.count();
}
/** Removes all items from the local stack, deleting them if autoDelete() is TRUE.  */
void NodeList::stackClear(void){
  stack_.clear();
}
/** Removes the top item from the local stack and deletes it if autoDelete() is TRUE. Returns TRUE if there was an item to pop;
otherwise returns FALSE.  */
bool NodeList::stackRemove (void) {
  if (stack_.isEmpty() )
    return false;
  stack_.pop();
  return true;
}
/** Returns TRUE is the local stack contains no elements to be popped; otherwise returns FALSE.  */
bool NodeList::stackIsEmpty (){
  return stack_.isEmpty();
}

int NodeList::maxID_=1;
int NodeList::groupIdCounter_=0;
Image NodeList::outImage_;
Image NodeList::groupImage_;
/** copy the nodes in this nodelist into the group image */

/** set the out-image-file-name in all nodes of NODELIST */
void NodeList::setImgName(QString outImgName){
  for (Iterator it=begin(); it!=end(); ++it) {
    BottomUpLib::Node* node = it.value();
    node->replace("file",outImgName);
  }
}

