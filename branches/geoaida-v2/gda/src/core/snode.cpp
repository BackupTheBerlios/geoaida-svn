/***************************************************************************
                          snode.cpp  -  description
                             -------------------
    begin                : Wed Jul 12 2000
    copyright            : (C) 2000 by Martin Pahl
    email                : pahl@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "SNode"
#include "Task"
#include "OperatorList"
#include "FileList"
#include "QColor"
#include <QDir>
#include <QRegExp>
#include <QTextStream>
#include <QXmlStreamWriter>
#include "cleanup.h"
#ifdef WIN32
#include <stdlib.h> // für exit
#include <QMessageBox>
#endif

#include <QXmlStreamReader>

//#define DEBUG_MSG

#warning do we need tdCounter_ and buCounter_ ?
static QString GetTmpFilename(QString prefix = "", QString path = ".")
{
  static int no = 0;
  static QString dir = CleanUp::getTmpDirPID();
  QDir tmppath(dir+"/"+path+"/"+QString().sprintf("%04d-", no++)+prefix);
  return tmppath.path();
}

SNode::SNode()
{
  init();
}

SNode::~SNode()
{
}

/** Copy constructor */
SNode::SNode(SNode & node):TreeNode < SNode, GNode > (node)
{
//  min_ = node.min_;
//  max_ = node.max_;
#warning call init() here?
  tdCounter_ = 0;
  buCounter_ = 0;
  genericAttributes_ = node.genericAttributes_;
  topDown_ = node.topDown_;
  bottomUp_ = node.bottomUp_;
  evaluation_ = node.evaluation_;
  color_ = node.color_;
}

/** Construct a SNode by parsing a file */
SNode::SNode(MLParser & parser)
{
  init();
  read(parser);
}

/** Init member variables */
void SNode::init()
{
//      GNode::init();
//  order_=0;
  tdCounter_=0;
  buCounter_=0;
  refCounter_=0;
  topDown_ = 0;
  bottomUp_ = 0;
  evaluation_ = 0;
//  min_ = 1;
//  max_ = 1;
  sections_ += "bottomUp";
  sections_ += "topDown";
//  GILSON --
//  sections_+="evaluation";
  sections_+="evaluation";
  genericAttributes_.insert("resultNode",
                            new Attribute("resultNode", "FALSE", Attribute::BOOL,
                                          "Result node",
                                          "If this attribute is activated this node will be handled as a leaf node"));
  genericAttributes_.insert("name",
                            new Attribute("name", "name_", Attribute::STRING,
                                          "Name", "Name of Node"));
  genericAttributes_.insert("class",
                            new Attribute("class", "", Attribute::STRING,
                                          "Class",
                                          "Class of node<br>This is used for selecting nodes resulting from the topdown operator call"));
  genericAttributes_.insert("order",
                            new Attribute("order", "0", 0, 99999, "Order",
                                          "Node processing priority, lowest entry processed first"));
  genericAttributes_.insert("min",
                            new Attribute("min", "0", 0, 99999, "Min",
                                          "Minimal number of needed hypotheses"));
  genericAttributes_.insert("max",
                            new Attribute("max", "0", 0, 99999, "Max",
                                          "Maximal number of needed hypotheses"));
  genericAttributes_.insert("topDown",
                            new Attribute("topDown", "Dummy TopDown",
                                          "topdown", "TopDown Operator"));
  genericAttributes_.insert("bottomUp",
                            new Attribute("bottomUp", "Dummy BottomUp",
                                          "bottomup", "BottomUp Operator"));
  genericAttributes_.insert("evaluation",
                            new Attribute("giscomp", "***No Operator***",
                                          "giscomp", "EvaluationOperator"));
  genericAttributes_.insert("color",
                            new Attribute("color", "255:255:255",
                                          Attribute::COLOR, "Color",
                                          "Color that identifies occurrences of this concept in the result image"));
  genericAttributes_.insert("td_multiclass",
                            new Attribute("td_multiclass", "FALSE",
                                          Attribute::BOOL,
                                          "TopDown Multi-Class",
                                          "<ul>"
                                          "<li>Multi-Class Off- Operator only detects one class</li>"
                                          "<li>Multi-Class On - Operator detects multiple classes</li>"
                                          "</ul>"));
  genericAttributes_.insert("temporal_node",
                            new Attribute("temporal_node", "FALSE",
                                          Attribute::BOOL,
                                          "Temporal node",
                                          "Indicates whether or not it is a temporal node."));
  {
    QStringList *o = new QStringList();
    (*o) += "TopDown";
    (*o) += "BottomUp";
    (*o) += "DebugInfo";
    (*o) += "NoDebugInfo";
    genericAttributes_.insert("debug",
                              new Attribute("debug", "DebugInfo", 0, o, "Debug",
                                            "<ul>"
                                            "<li>TopDown - Start single step when topdown is executed</li>"
                                            "<li>BottomUp - Start single step when bottomup is executed</li>"
                                            "<li>DebugInfo - Keep temporary files</li>"
                                            "<li>NoDebugInfo - Clean up all temporary files</li>"
                                            "</ul>"));
  }
  attribute("debug", "DebugInfo");
}

/** Get min amount of SNodes */
int SNode::minNumNode() {
  return attributeInt("min");
}

/** Get max amount of SNode */
int SNode::maxNumNode() {
  return attributeInt("max");
}

/** Get order of SNode */
int SNode::order() {
  return attributeInt ("order");
}

/** Set the attribute key to val */
void SNode::attributeSet(QString key, QString val)
{
#ifdef DEBUG_MSG
  qDebug("SNode::attributeSet %s=%s\n", 
	 key.toLatin1().constData(),
         val.toLatin1().constData());
#endif
  GNode::attributeSet(key, val);
  if ((key == "name") && (!attribList_.contains("class")))
    attributeSet("class", val);
  if (key == "iscompound")
    isCompound_ = (val.toInt() != 0);
  if (key == "topdown")
    changeOperator(topDown_, val);
  if (key == "bottomup")
    changeOperator(bottomUp_, val);
  if (key == "evaluation")
    changeOperator(evaluation_, val);
  if (key == "color") {
    int r = 0;
    int g = 0;
    int b = 0;
    QString attrib = val;
    attrib.replace(QRegExp("[,;:]"), " ");
    QTextStream istr(&attrib);
    if (!istr.atEnd())
      istr >> r;
    if (!istr.atEnd())
      istr >> g;
    if (!istr.atEnd())
      istr >> b;
    color_ = qRgb(r, g, b);
  }
}

/** This node is a compound of its subnodes. */
bool SNode::isCompound()
{
  return isCompound_;
}

/** Set this node to be a compound or a generalization node. */
void SNode::isCompound(bool val)
{
  if (val)
    attribute("isCompound", 1);
  else
    attribute("isCompound", 0);
}

/** Makes a copy of this node */
GNode *SNode::copy()
{
  GNode *gn = new SNode(*this);
#ifdef WIN32
  if (gn == 0){
    //cout << "Out of Memory..15";
    exit(1);
  }
#endif
  return gn;
}

/** returns a new node of type SNode */
GNode *SNode::newNode()
{
  GNode *gn = new SNode();
#ifdef WIN32
  if (gn == 0){
    //cout << "Out of Memory..16";
    exit(1);
  }
#endif
  return gn;
}

/** set node variables using the attribute list */
void SNode::setVars()
{
  GNode::setVars();
//  MLParser::setInt(min_, &attribList_, "min");
//  MLParser::setInt(max_, &attribList_, "max");
  QString val;
  val = "";
  MLParser::setString(val, &attribList_, "topdown");
  if (!val.isEmpty())
    topDown_ = operatorList_[val];
#define OLD_NET_COMPATIBILITY
#ifdef OLD_NET_COMPATIBILITY
  if (topDown_) {
    AttributeDictionary *attribs = topDown_->attributeDesc();
    if (attribs) {
      for (AttributeDictionary::const_iterator it=attribs->constBegin(); 
	   it!=attribs->constEnd(); 
	   ++it) {
	Attribute* attrib= *it;
        QString name = attrib->name();
	if (attribList().contains(name)) {
	  QString val=attribList().value(name);
          qDebug("SNode::setVars: substituting %s with %s",name.toLatin1().constData(),(**it).fullname().toLatin1().constData());
          attribList().replace(attrib->fullname(),val);
          attribList().remove(name);
        }
      }
    }
  }
#endif
  val = "";
  MLParser::setString(val, &attribList_, "bottomup");
  bottomUp_ = operatorList_[val];

#ifdef OLD_NET_COMPATIBILITY
  if (bottomUp_) {
    AttributeDictionary *attribs = bottomUp_->attributeDesc();
    if (attribs) {
      for (AttributeDictionary::const_iterator it=attribs->constBegin();
	   it!=attribs->constEnd(); 
	   ++it) {
	Attribute* attrib= *it;
        QString name = attrib->name();
	if (attribList().contains(name)) {
	  QString val=attribList().value(name);
          qDebug("SNode::setVars: substituting %s with %s",name.toLatin1().constData(),(**it).fullname().toLatin1().constData());
          attribList().replace(attrib->fullname(),val);
          attribList().remove(name);
        }
      }
    }
  }
#endif
  val = "";
  MLParser::setString(val, &attribList_, "evaluation");
  evaluation_ = operatorList_[val];
  MLParser::setString(val, &attribList_, "color");
  attributeSet("color", val);
}

/** Execute the given operator. This function spawns a new process
  * and returns. When the task is finish iNode->taskFinished() is called. */
void SNode::execOperator(Operator * op, INode * iNode, AttribList & attribs,
                         QString path, int &counter)
{
  if (!op) {
    iNode->taskFinished(0,0);
    return;
  }
  CleanUp::mkdir(CleanUp::getTmpDirPID(), path);
  iNode->output(GetTmpFilename("output", path));
  attribs.replace("output", iNode->output());
  attribs.replace("tmpdir", CleanUp::getTmpDirPID());
	
#warning What is that?
#if 0
	//Returns the process pid
	unsigned int pid = cleanUp_.getPID();

	//Rodrigo

		QString projectFile;
		QString fuzzysetsFile;
		QString fileName = "./" + QString().setNum(pid) + ".tmp";

		 if (fileName.isEmpty())
			 return;

		 QFile file1(fileName);
                 if (file1.open(QFile::ReadOnly | QFile::Text)) {


		QXmlStreamReader reader(&file1);

		while (!reader.atEnd())
		{
			 reader.readNext();

			 if (reader.error())
			 {
				 return;
			 } else
				 if (reader.isStartElement() && reader.name().toString() == "geoproject") {
					 projectFile = reader.attributes().value("file").toString();
				 } else {
				 }
		}
		
		file1.close();

		/* Read project file */

		if (projectFile.isEmpty())
			return;

		QFile file3(projectFile);

		if (!file3.open(QFile::ReadOnly | QFile::Text)) {
			 return;
		 }

		QXmlStreamReader reader3(&file3);

		while (!reader3.atEnd())
		{
			 reader3.readNext();

			 if (reader3.error())
			 {
				 return;
			 } else
				 if (reader3.isStartElement() && reader3.name().toString() == "fuzzysets") {
					 fuzzysetsFile = reader3.attributes().value("file").toString();
				 }
		}
		
		file3.close();

  attribs.replace("fuzzysets", new QString(fuzzysetsFile));
                 }

//Rodrigo
#endif
  
  
  QString cmd=op->command(attribs);
  
 cmd.replace("\n"," "); //GILSON: must replace new line, or else Windows batch file will not run properly 

  QString stdout_file=iNode->output()+".stdout";
  QString stderr_file=iNode->output()+".stderr";
  QFile fp(iNode->output()+".cmd");
  if (fp.open(QIODevice::WriteOnly)) {
    fp.write(cmd.toLatin1().constData(),cmd.length());
    fp.close();
  }
//  cmd+=" >"+stdout_file+" 2>"+stderr_file;
  cmd+=" 2>"+stderr_file;
  taskTable.queue(cmd, iNode);
  counter++;
  stateChanged(guiPtr_);
}

static void putAttribs(SNode & snode, AttribList & attribs, QString section)
{
  AttributeDictionary *attribDict = snode.attributeDesc(section);
  AttribList & attribList = snode.attribList();
  if (attribDict) {
    for (AttributeDictionary::const_iterator it=attribDict->constBegin(); 
	 it!=attribDict->constEnd(); 
	 ++it) {
      QString name = (*it)->name();
      if (attribList.contains((*it)->fullname()))
	attribs.replace(name, attribList.value((*it)->fullname()));
    }
  }
}

/** Execute the topDown operator. This function spawns a new process
  * and returns. When the topDown task is finish iNode->taskFinished() is called. */
void SNode::execTopDownOp(INode * iNode)
{
  GeoImageList *geoImageList=iNode->analysis()->geoImageList();
  Q_ASSERT(geoImageList);
#ifdef DEBUG_MSG
  qDebug("#* SNode::execTopDown(%s) (%p)", name_.toLatin1().constData(), this);
#ifdef WIN32
  QMessageBox::information(0,"SNode","execTopDown",QMessageBox::Default);
#endif
#endif
  Q_ASSERT(iNode->parent());
//((*(TreeGNode*)(&(*(TreeNode<GNode,TreeGNode>*)(&(*(GNode*)(&(*(TreeNode<INode,GNode>*)(&*iNode))))))))).parent_

//Hier Absturz weil parent = 0
//Darf nur aus Analyze::start() gestartet werden, nicht aus MainGui::Start()!

  INode *parent = 0;

  if (iNode)
    if (iNode->parent()) {
    	if (((this->parent())->temporal()) && ((iNode->parent())->temporalProcessing()))
    		parent = iNode; //if iNode is a result of temporal top-down processing, its own info will be used 
    	else
    		parent = iNode->parent();
    }
    else
      parent = iNode;
  else return;
  
  AttribList attribs(parent->attribList());
  AttributeDictionary *topDownAttrib = attributeDesc("topDown");
  // now generate image specific attributes
  if (topDownAttrib) {
    for (AttributeDictionary::const_iterator it=topDownAttrib->constBegin();
	 it!=topDownAttrib->constEnd(); 
	 ++it) {
      Attribute *attrib=*it;
      switch (attrib->type()) {
      case Attribute::IMAGE:{
          QString imageName = attribute(attrib->fullname());
          QString keyName = attrib->name();
          GeoImage *image = geoImageList->geoImage(imageName);
          if (image) {
            for (ArgDictConstIterator it= image->constBegin(); 
		 it!=image->constEnd(); 
		 ++it) {
              attribs.replace(keyName + "_" + it.key(),it.value());
            }
            QString partName;
            if (topDown_ && topDown_->runGlobal())
              partName = image->part(geoImageList->geoWest(),
                                     geoImageList->geoNorth(),
                                     geoImageList->geoEast(),
                                     geoImageList->geoSouth());
            else
              partName = image->part(parent->attributeFloat("geoWest"),
                                     parent->attributeFloat("geoNorth"),
                                     parent->attributeFloat("geoEast"),
                                     parent->attributeFloat("geoSouth"));
            cleanUp_.append(partName);
            attribs.replace(keyName + "_file", partName);
          }
        }
        break;
      case Attribute::FILE:{
          QString fileName = attribute((**it).fullname());
          QString keyName = (**it).name();
          ArgDict *args = fileList_.value(fileName);
          if (args) {
            for (ArgDictConstIterator it=args->constBegin(); 
		 it!=args->constEnd(); 
		 ++it) {
              attribs.replace(keyName + "_" + it.key(),it.value());
            }
          }
        }
        break;
      default:
        break;
      }
    }
  }
  // create a mask image and store mask attributes
  GeoImage *labelImage = parent->labelImage();
  if (labelImage) {
    for (ArgDictConstIterator it = labelImage->constBegin();
	 it!=labelImage->constEnd(); 
	 ++it) {
      attribs.replace("mask_" + it.key(), it.value());
    }
    QString maskfile;
    if (topDown_ && topDown_->runGlobal()) {
      int global_mask_x, global_mask_y, x2, y2, local_mask_x, local_mask_y;
      labelImage->picBBox(geoImageList->geoWest(),geoImageList->geoNorth(),
              geoImageList->geoEast(),geoImageList->geoSouth(),
              global_mask_x, y2, x2, global_mask_y);
      int global_mask_size_x=x2-global_mask_x+1;
      int global_mask_size_y=y2-global_mask_y+1;
      labelImage->picBBox(parent->attributeFloat("geoWest"),
                          parent->attributeFloat("geoNorth"),
                          parent->attributeFloat("geoEast"),
                          parent->attributeFloat("geoSouth"),
                          local_mask_x, y2, x2, local_mask_y);

      attribs.replace("mask_x",local_mask_x-global_mask_x);
      attribs.replace("mask_y",local_mask_y-global_mask_y);
      attribs.replace("mask_size_x",global_mask_size_x);
      attribs.replace("mask_size_y",global_mask_size_y);
#ifdef DEBUG_MSG
      qDebug("SNode::execTopDown: mask: w=%f, n=%f, e=%f, s=%f\n"
             "                          x=%d, y=%d, size_x=%d, size_y=%d\n"
             "                          mw=%f, mn=%f, me=%f, ms=%f\n"
             "                          msize_x=%d, msize_y=%d\n",
              geoImageList->geoWest(),geoImageList->geoNorth(),
              geoImageList->geoEast(),geoImageList->geoSouth(),
              local_mask_x-global_mask_x, local_mask_y-global_mask_y,global_mask_size_x, global_mask_size_y,
              labelImage->geoWest(),labelImage->geoNorth(),labelImage->geoEast(),labelImage->geoSouth(),
              labelImage->cols(),labelImage->rows());
#endif
    }
    maskfile = labelImage->mask(parent->attributeFloat("geoWest"),
                                parent->attributeFloat("geoNorth"),
                                parent->attributeFloat("geoEast"),
                                parent->attributeFloat("geoSouth"),
                                parent->attributeInt("id"), parent->path());
    attribs.replace("mask_file", maskfile);
    cleanUp_.append(maskfile);
  }
  else 
    attribs.replace("mask_file", "\"\"");

  attribs.replace("minRes", geoImageList->minResolution());
  attribs.replace("maxRes", geoImageList->maxResolution());
  attribs.replace("numRegions",SNode::parent()->refCounter_);
#if 1
  putAttribs(*this, attribs, "generic");
  putAttribs(*this, attribs, "topDown");
#else
  {
    QDictIterator < QString > it(attribList());
    for (; it.current(); ++it) {
      attribs.replace(it.currentKey(), it.current());
    }
  }
#endif
#ifdef DEBUG_MSG
  {
    qDebug("SNode::execTopDown: attributes");
    //QTextOStream ts(stderr);
    //ts << attribs;
  }
#endif
  execOperator(topDown_, iNode, attribs, parent->path(), tdCounter_);
}

/** Execute the Temporal Top-Down operator. This function spawns a new process
  * and returns. When the Temporal Top-Down task is finish iNode->taskFinished() is called. */
void SNode::execTemporalTopDownOp(INode * iNode)
{
#ifdef DEBUG_MSG
  qDebug("#* SNode::execTemporalTopDown(%s)", name_.toLatin1().constData());
#endif

  //INode *inodeYoungerSibling;  // pointer to temporal node
  QList<INode*>::iterator inodeYoungerSibling;
  QList <INode*> *temporalSiblingList;  // list of temporal siblings
  
  temporalSiblingList = &((iNode->parent())->children()); // creates list of siblings for this INode
  inodeYoungerSibling = temporalSiblingList->begin();
  while ((((*inodeYoungerSibling)->sNode())->order()) != ((iNode->sNode())->order() - 1))  {
	  ++inodeYoungerSibling;
	  if (inodeYoungerSibling == temporalSiblingList->end()) {
		  qDebug("Temporal TD: Could not find earlier temporal node. Semantic Net definition error!\n");
		  iNode->taskFinished(0,0);
		  return;
	  }
  }
  
  CleanUp::mkdir(CleanUp::getTmpDirPID(), iNode->path());
  QString iname = GetTmpFilename("input", iNode->path());
  QFile ifp(iname);
  if (!ifp.open(QIODevice::WriteOnly)) {
    qDebug("Could not open tempfile %s\n", iname.toLatin1().constData());
    iNode->taskFinished(0,0);
    return;
  }

  QXmlStreamWriter str(&ifp);
  str.setAutoFormatting(true);
  str.writeStartDocument();
  str.writeStartElement("nodelist");

  QList<INode*> auxList = (*inodeYoungerSibling)->children();
  
  for (QList<INode*>::iterator itNode = auxList.begin(); itNode != auxList.end(); ++itNode) {
    INode *node = *itNode;
    if (node->status() == TRASH) // trashnodes must be skipped here
      continue;
    GeoImage *img = node->labelImage();
    if (img) {
        node->attribute("file_geoWest", img->geoWest());
        node->attribute("file_geoSouth", img->geoSouth());
        node->attribute("file_geoEast", img->geoEast());
        node->attribute("file_geoNorth", img->geoNorth());
    }
    node->attribute("temporal", (*inodeYoungerSibling)->attribute("class"));
    node->write(str, false);
  }

  str.writeEndElement();
  str.writeEndDocument();

  ifp.close();

  iNode->attribute("input", iname);

  AttribList attribs;
  putAttribs(*this, attribs, "generic");
  putAttribs(*this, attribs, "topDown");

  AttribListConstIterator it= iNode->attribList().constBegin();
  for (; it!=iNode->attribList().constEnd(); ++it) {
    attribs.replace(it.key(), it.value());
  }
  //int dummyCounter = 0; //no need to increment tdCounter_ here
  //execOperator(topDown_, iNode, attribs, iNode->path(), dummyCounter);
  execOperator(topDown_, iNode, attribs, iNode->path(), tdCounter_);
}

#if 0
QStringList & SNode::predefinedAttribs()
{
  return predefinedAttribs_;
}
#endif

/** Returns true if  this class is a cname */
bool SNode::isA(QString cname)
{
  return (cname == "SNode");
}

/** Get the attribute description for the specified section */
AttributeDictionary *SNode::attributeDesc(QString section)
{
#ifdef DEBUG_MSG
  qDebug("SNode::attributeDesc(%s)\n", section.toLatin1().constData());
#endif
  if (section == "generic")
    return &genericAttributes_;
  if ((section == "topDown") && (topDown_))
    return topDown_->attributeDesc();
  if ((section == "bottomUp") && (bottomUp_))
    return bottomUp_->attributeDesc();
  if ((section == "evaluation") && (evaluation_))
    return evaluation_->attributeDesc();
#ifdef DEBUG_MSG
  qDebug("SNode::attributeDesc(%s) not found\n", section.toLatin1().constData());
#endif
  return 0;
}

/** Execute the temporalBottomUp operator. This function spawns a new process
    and returns. When the temporalBottomUp task is finish iNode->taskFinished() is called. */
void SNode::execTemporalBottomUpOp(INode * iNode)
{
#ifdef DEBUG_MSG
  qDebug("#* SNode::execTemporalBottomUp(%s)", name_.toLatin1().constData());
#endif

  Operator *temporalBottomUp = NULL;
  AttribList attribs;
  int counter;
  
  CleanUp::mkdir(CleanUp::getTmpDirPID(), iNode->path());
  QString iname = GetTmpFilename("input", iNode->path());
  QFile ifp(iname);
  if (!ifp.open(QIODevice::WriteOnly)) {
    qDebug("Could not open tempfile %s\n", iname.toLatin1().constData());
    iNode->taskFinished(0,0);
    return;
  }

  QXmlStreamWriter str(&ifp);
  str.setAutoFormatting(true);
  str.writeStartDocument();
  str.writeStartElement("nodelist");
  
  QList<INode*> auxList = (iNode->parent())->children();
  
  QList<INode*>::iterator itSibling; // creates list of siblings for this INode
  for (itSibling = auxList.begin(); itSibling != auxList.end(); ++itSibling) {	
	INode *tNode = (*itSibling);
	
	QList<INode*> auxList2 = tNode->children();
	
    QList<INode*>::iterator itCousin;
    for (itCousin = auxList2.begin(); itCousin != auxList2.end(); ++itCousin) {
      INode *node = (*itCousin);
      if (node->status() == TRASH) // trashnodes must be skipped here
        continue;
      GeoImage *img = node->labelImage();
      if (img) {
        node->attribute("file_geoWest", img->geoWest());
        node->attribute("file_geoSouth", img->geoSouth());
        node->attribute("file_geoEast", img->geoEast());
        node->attribute("file_geoNorth", img->geoNorth());
      }
      //node->attribute("addr", QString().sprintf("%p", node));
      node->attribute("addr", QString("%1").arg((long) node, 0, 16));
      node->attribute("temporal", tNode->attribute("class"));
      node->write(str, false);
      node->attributeRemove("addr");
    }
    if ((tNode->sNode())->bottomUp_) {
      temporalBottomUp = (tNode->sNode())->bottomUp_;
      putAttribs(*(tNode->sNode()), attribs, "generic");
      putAttribs(*(tNode->sNode()), attribs, "bottomUp");
    }
  }
  str.writeEndElement();
  str.writeEndDocument();
  ifp.close();
    
  iNode->attribute("input", iname);
  
  AttribListConstIterator it= iNode->attribList().constBegin();
  for (; it!=iNode->attribList().constEnd(); ++it) {
      attribs.replace(it.key(), it.value());
  }
  
  counter = buCounter_;
  
  execOperator(temporalBottomUp, iNode, attribs, iNode->path(), buCounter_);
  
  if (counter != buCounter_){ // buCounter_ of all siblings must change in the same way
  
	QList<SNode*> auxList3 = ((iNode->sNode())->parent())->children();
  
	  QList<SNode*>::iterator itSSibling;
	  for (itSSibling = auxList3.begin(); itSSibling != auxList3.end(); ++itSSibling) {	
		SNode *sNode = (*itSSibling);
		if (sNode!=this) (sNode->buCounter_)++;
		stateChanged(sNode->guiPtr_);
	  }
  }
}

/** Execute the bottomUp operator. This function spawns a new process
    and returns. When the bottomUp task is finish iNode->taskFinished() is called. */
void SNode::execBottomUpOp(INode * iNode)
{
#ifdef DEBUG_MSG
  qDebug("#* SNode::execBottomUp(%s)", name_.toLatin1().constData());
#endif
  CleanUp::mkdir(CleanUp::getTmpDirPID(), iNode->path());
  QString iname = GetTmpFilename("input", iNode->path());
  QFile ifp(iname);
  if (!ifp.open(QIODevice::WriteOnly)) {
    qDebug("Could not open tempfile %s\n", iname.toLatin1().constData());
    iNode->taskFinished(0,0);
    return;
  }

  QXmlStreamWriter str(&ifp);
  str.setAutoFormatting(true);
  str.writeStartDocument();
  str.writeStartElement("nodelist");

  if (!iNode->parent()) {     //! Debug
    qDebug("SNode::execBottomUp: root node evaluated");
  }
#if 0                           // This code deletes the trashnodes
  {                           // Delete trash nodes
    QListIterator < INode > it =
      QListIterator < INode > (iNode->children());
    while (it.current()) {
      INode *node = it.current();
      if (node->status() == TRASH) {
	iNode->childUnlink(node);
	delete node;
      }
      else
	++it;
    }
  }
#endif
  for (QList < INode* >::ConstIterator it = iNode->children().constBegin(); 
       it!=iNode->children().constEnd();
       ++it) {
    INode *node = *it;
#if 1                           // If trashnode is not deleted (s.a.) trashnodes must be skipped here
    if (node->status() == TRASH)
      continue;
#endif
    GeoImage *img = node->labelImage();
    if (img) {
      node->attribute("file_geoWest", img->geoWest());
      node->attribute("file_geoSouth", img->geoSouth());
      node->attribute("file_geoEast", img->geoEast());
      node->attribute("file_geoNorth", img->geoNorth());
    }
    node->attribute("addr", QString().sprintf("%p", node));
    node->write(str, false);
    node->attributeRemove("addr");
  }
  
  str.writeEndElement();
  str.writeEndDocument();
  ifp.close();

#if 1
  AttribList attribs;
  putAttribs(*this, attribs, "generic");
  putAttribs(*this, attribs, "bottomUp");
#else
  AttribList attribs(attribList());
#endif
  iNode->attribute("input", iname);
  {
    for (AttribListConstIterator it= iNode->attribList().constBegin(); 
	 it!=iNode->attribList().constEnd(); 
	 ++it) {
      attribs.replace(it.key(), it.value());
    }
  }
  execOperator(bottomUp_, iNode, attribs, iNode->path(), buCounter_);
}

#if 0
QStringList & SNode::predefinedAttribs()
{
  return predefinedAttribs_;
}
#endif

/** Evaluates the result of TopDownOperator    */
QList < INode* > SNode::evalTopDown(INode * inode)
{
#ifdef DEBUG_MSG
  qDebug("#* SNode::evalTopDown(%s)", name_.toLatin1().constData());
#endif
  tdCounter_--;
  stateChanged(guiPtr_);
  Q_ASSERT(inode);
  QList < INode* > list;
  cleanUp_.append(inode->output());
  QFile fp(inode->output());
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("SNode::evalTopDown(%s): file not founed\n",
           (inode->output().toLatin1().constData()));
    return list;
  }
  MLParser parser(&fp);
  QString classname;
  MLParser::setString(classname, &(attribList()), "class");
#ifdef DEBUG_MSG
  qDebug("SNode::evalTopDown: Searching for %s\n", classname.toLatin1().constData());
#endif
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
#ifdef DEBUG_MSG
    qDebug("tag=%d\n", tag);
#endif
    AttribList *attribList = parser.args();
    switch (tag) {
    case TOK_REGION: {
        SNode *sNode;
        INode *node;
        QString classNameRegion = attribList->value("class");
	
#ifdef DEBUG_MSG
        qDebug("class is %s\n", classNameRegion.toLatin1().constData());
#endif
        if (attribList->contains("class")) {
          if (QString::compare(classname, classNameRegion,Qt::CaseInsensitive) == 0) {
            sNode = this;
          }
          else if (parent() && attributeBool("td_multiclass")) {
            sNode = parent()->findClass(classNameRegion);
          }
          else
            sNode = 0;
        }
        else
          sNode = 0;

        if (sNode) {
#ifdef DEBUG_MSG
          qDebug("SNode::evalTopDown(): found %s\n",
                 classNameRegion.toLatin1().constData());
#endif
          node = new INode(sNode);
          node->configure(*attribList);
          if (node->attribute("name").isEmpty() && attribList->contains("id"))
            node->attribute("name", 
			    sNode->name() 
			    + QString().sprintf("_%03d",
						attribList->value("id").toInt()));
          Q_ASSERT(inode->parent());
          double gN = inode->parent()->attributeFloat("geoNorth");
          double gS = inode->parent()->attributeFloat("geoSouth");
          double gW = inode->parent()->attributeFloat("geoWest");
          double gE = inode->parent()->attributeFloat("geoEast");
          node->setGeoRegion(gW, gN, gE, gS);
          cleanUp_.append(node->attribute("file")); //GILSON: maybe this is not necessary
          list.append(node);
        }
        break;
      }
    }
    delete attribList;
  } while (tag != MLParser::END_OF_FILE);
  return list;
}

/** Evaluates the result of temporalTopDownOperator    */
QList <INode*> *SNode::evalTemporalTopDown(INode * inode)
{
#ifdef DEBUG_MSG
  qDebug("#* SNode::evalTemporalTopDown(%s)", name_.toLatin1().constData());
#endif
  tdCounter_--;
  stateChanged(guiPtr_);
  Q_ASSERT(inode);
  QList <INode*> *list = new QList <INode*>;

  cleanUp_.append(inode->attribute("input"));
  inode->attributeRemove("input");
  cleanUp_.append(inode->output());
  QString fileName = inode->output();

  QFile fp(fileName);
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("SNode::evalTemporalTopDown(%s): file not found\n",
           (inode->output().toLatin1().constData()));
    delete list;
    return 0;
  }
  QString classname = this->attribute("class");
  MLParser parser(&fp); 
#ifdef DEBUG_MSG
  qDebug("SNode::evalTemporalTopDown: Searching for %s\n", classname.toLatin1().constData());
#endif
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
#ifdef DEBUG_MSG
    qDebug("tag=%d\n", tag);
#endif
    AttribList *attribList = parser.args();
#ifdef DEBUG_MSG     
    qDebug("tag=%d\n", tag);
#endif
    switch (tag) {
    case TOK_REGION: {
        SNode *sNode;
        INode *node;
        QString classNameRegion = attribList->value("class");
#ifdef DEBUG_MSG
        qDebug("class is %s\n", classNameRegion.toLatin1().constData());
#endif
        if (!classNameRegion.isEmpty()) {
          sNode = this->findClass(classNameRegion);
        }
        else
          sNode = 0;

        if (sNode) {
#ifdef DEBUG_MSG
          qDebug("SNode::evalTemporalTopDown(): found %s\n",
                 classNameRegion.toLatin1().constData());
#endif
          node = new INode(sNode);
          node->configure(*attribList);
          double gN = node->attributeFloat("geoNorth");
          double gS = node->attributeFloat("geoSouth");
          double gW = node->attributeFloat("geoWest");
          double gE = node->attributeFloat("geoEast");
          node->setGeoRegion(gW, gN, gE, gS);
          list->append(node);
        }
        break;
      }
    }
    delete attribList;
#ifdef DEBUG_MSG     
    qDebug("tag=%d\n", tag);
#endif
  } while (tag != MLParser::END_OF_FILE); 
  return list;
}

/** Evaluates the result of the temporal bottom-up-operator   */
QList <INode*> &SNode::evalTemporalBottomUp(INode * iNode)
{
//QFile fDebug("debug_evalTemporalBottomUp.txt");
//fDebug.open(IO_WriteOnly);
//Q3TextStream strDebug(&fDebug);
//strDebug << ".1" << endl;

#ifdef DEBUG_MSG
  qDebug("#* SNode::evalTemporalBottomUp(%s)", name_.toLatin1().constData());
  //cout << "#* SNode::evalTemporalBottomUp(%s)" << name_.toLatin1().constData() << endl;
#endif

  //decrement buCounter_ of all tempral siblings
  
  QList<SNode*> auxList = ((iNode->sNode())->parent())->children();
  
  QList<SNode*>::iterator itSSibling;
  for (itSSibling = auxList.begin(); itSSibling != auxList.end(); ++itSSibling) {	
	  SNode *sNode = *itSSibling;
	  (sNode->buCounter_)--;
	  stateChanged(sNode->guiPtr_);
  }

  cleanUp_.append(iNode->attribute("input"));
  iNode->attributeRemove("input");
  
  QList <INode*> *groupList = new QList <INode*>;
  //groupList->setAutoDelete(false);

  INode *list = 0;
//strDebug << ".2" << endl;
  cleanUp_.append(iNode->output());
  QFile fp(iNode->output());
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("SNode::evalTemporalBottomUp(%s): file not found\n",
           iNode->output().toLatin1().constData());
    return *groupList;
  }
  MLParser parser(&fp);
 
//strDebug << ".3" << endl;
  
#ifdef DEBUG_MSG
  qDebug("SNode::evalTemporalBottomUp: Parsing %s\n", iNode->output().toLatin1().constData());
#endif
  bool foundSNode = false;
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
//strDebug << ".3 tag: " << tag << endl;
#ifdef DEBUG_MSG
    qDebug("tag=%d\n", tag);
    //cout << "tag = " << tag << endl;
#endif
    AttribList *attribList = parser.args();
    INode *node;
    switch (tag) {
    case TOK_GROUP:
	{
      // list = new INode(this);
      // must find SNode to which this group belongs to
      SNode *sNode;
      QString classNameRegion = attribList->value("class");

//strDebug << ".4 TOK_GROUP: *classNameRegion: " << *classNameRegion << endl;

      if (!classNameRegion.isEmpty()) {
        sNode = ((iNode->sNode())->parent())->findClass(classNameRegion);
      }
      else
        sNode = 0;
//strDebug << ".5 TOK_GROUP: sNode: " << sNode << endl;

      if (sNode) {
#ifdef DEBUG_MSG
        qDebug("SNode::evalTemporalBottomUp(): found %s\n",
               classNameRegion.toLatin1().constData());
#endif
    	foundSNode = true;
        list = new INode(sNode);
#ifdef WIN32
        if (list == 0){
          //cout << "Out of Memory..16";
          exit(1);
        }
#endif
        list->update(iNode->attribList());
        list->attributeRemove("llx");
        list->attributeRemove("urx");
        list->attributeRemove("lly");
        list->attributeRemove("ury");
        list->attributeRemove("file_geoWest");
        list->attributeRemove("file_geoNorth");
        list->attributeRemove("file_geoEast");
        list->attributeRemove("file_geoSouth");
        list->update(*attribList);
        if (!attribList->contains("name") && attribList->contains("id"))
          list->attribute("name", sNode->name() + QString().sprintf("_t%03d",(attribList->value("id").toInt())));
		  
//strDebug << ".6 TOK_GROUP: foundSNode: " << foundSNode << endl;
      }
      else {
    	foundSNode = false;
      } 
	}
    break;

    case -TOK_GROUP:
      if (foundSNode) {
//strDebug << ".7 -TOK_GROUP: list->children().count(): " << list->children().count() << endl;
        if (list->children().count() != 0) {
          list->setGeoRegion(list->attributeFloat("file_geoWest"),
                             list->attributeFloat("file_geoNorth"),
                             list->attributeFloat("file_geoEast"),
                             list->attributeFloat("file_geoSouth"));
          cleanUp_.append(list->attribute("file"));
          groupList->append(list);
        }
        else
          delete list;
      }
#ifdef DEBUG_MSG
      qDebug("End of group\n");
      //cout << "end of group " << endl;
#endif
      list = 0;
      break;
    case TOK_NODE:
      if (foundSNode) {
//strDebug << ".8 TOK_NODE: (*attribList)[addr]: " << *((*attribList)["addr"]) << endl;
        QString s = attribList->value("addr");
        if (s.isEmpty())
			break;
        node = 0;
		if (sscanf(s.toLatin1().constData(), "%p", &node) != 1)
			break;  
//strDebug << ".9 TOK_NODE: node.attribute(class): " << node->attribute("class") << endl;
//Q3DictIterator < QString > it(*attribList);
//for (; it.current(); ++it)
//strDebug << ".10 TOK_NODE:" << it.currentKey() << ": " << it.current()->latin1() << endl;
		node->update(*attribList);
//strDebug << ".11 TOK_NODE" << endl;
        node->attributeRemove("addr");
//strDebug << ".12 TOK_NODE" << endl;
        node->setGeoRegion(node->attributeFloat("file_geoWest"),
                           node->attributeFloat("file_geoNorth"),
                           node->attributeFloat("file_geoEast"),
                           node->attributeFloat("file_geoSouth"));
//strDebug << ".13 TOK_NODE" << endl;
        cleanUp_.append(node->attribute("file"));  
//strDebug << ".14 TOK_NODE: list: " << list << endl;
        list->childLink(node);
//strDebug << ".15 TOK_NODE: node.attribute(file): " << node->attribute("file") << endl;
      }
      break;
    case -TOK_NODE:
      break;
    }
    delete attribList;
  } while (tag != MLParser::END_OF_FILE);

  if (list)
    delete list;
//fDebug.close();
  QList<INode*>::iterator it = groupList->begin();
  for (; it != groupList->end(); ++it) {
#ifdef DEBUG_MSG
    qDebug("count=%d\n", (*it)->children().count());
    //cout << "count= " << it.current()->children().count() << endl;
#endif
  }
  return *groupList;
}


/** Evaluates the result of the bottom-up-operator   */
QList < INode* > &SNode::evalBottomUp(INode * iNode)
{
#ifdef DEBUG_MSG
  qDebug("#* SNode::evalBottomUp(%s)", name_.toLatin1().constData());
#endif
 
  buCounter_--;
  stateChanged(guiPtr_);
   
  cleanUp_.append(iNode->attribute("input"));
  iNode->attributeRemove("input");

  QList < INode* > *groupList = new QList < INode* >;

  INode *list = 0;

  cleanUp_.append(iNode->output());
  QFile fp(iNode->output());
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("SNode::evalBottomUp(%s): file not found\n",
           iNode->output().toLatin1().constData());
    return *groupList;
  }
  MLParser parser(&fp);
 
  
#ifdef DEBUG_MSG
  qDebug("SNode::evalBottomUp: Parsing %s\n", iNode->output().toLatin1().constData());
#endif
  int tag;
 
  do {
    tag = parser.tag(nodeTagTable);
#ifdef DEBUG_MSG
    qDebug("tag=%d\n", tag);
    //cout << "tag = " << tag << endl;
#endif
    AttribList *attribList = parser.args();
    INode *node;
    switch (tag) {
    case TOK_GROUP:
      list = new INode(this);
      Q_ASSERT(list);
      list->update(iNode->attribList());
      list->attributeRemove("llx");
      list->attributeRemove("urx");
      list->attributeRemove("lly");
      list->attributeRemove("ury");
      list->attributeRemove("file_geoWest");
      list->attributeRemove("file_geoNorth");
      list->attributeRemove("file_geoEast");
      list->attributeRemove("file_geoSouth");
      list->update(*attribList);
      if (!attribList->contains("name") && attribList->contains("id"))
        list->attribute("name", 
			iNode->name() 
			+ QString().sprintf("_%03d",(attribList->value("id").toInt())));
      break;
    case -TOK_GROUP:
      if (list->children().count() != 0) {
        list->setGeoRegion(list->attributeFloat("file_geoWest"),
                           list->attributeFloat("file_geoNorth"),
                           list->attributeFloat("file_geoEast"),
                           list->attributeFloat("file_geoSouth"));
        cleanUp_.append(list->attribute("file"));
        groupList->append(list);
      }
      else
        delete list;
#ifdef DEBUG_MSG
      qDebug("End of group\n");
      //cout << "end of group " << endl;
#endif
      list = 0;
      break;
    case TOK_NODE:
      {
        QString s = attribList->value("addr");
        if (s.isEmpty())
          break;
        node = 0;
        if (sscanf(s.toLatin1().constData(), "%p", &node) != 1)
          break;
#ifdef DEBUG_MSG
        qDebug("Found %s\n", node->name().toLatin1().constData());
#endif
        node->update(*attribList);
        node->attributeRemove("addr");
        node->setGeoRegion(node->attributeFloat("file_geoWest"),
                           node->attributeFloat("file_geoNorth"),
                           node->attributeFloat("file_geoEast"),
                           node->attributeFloat("file_geoSouth"));
        
        cleanUp_.append(node->attribute("file"));
        
        list->childLink(node);
        //! Set attributes added by BottomUp-Operator
        //GILSON: doesn't that happens with node->update(*attribList); ?????
         
      }
      break;
    case -TOK_NODE:
      break;
    }
    delete attribList;
  } while (tag != MLParser::END_OF_FILE);
//!     fp.remove();
 

  if (list)
    delete list;
  if (!iNode->parent()) {
#ifdef DEBUG_MSG
    qDebug
      ("#  (Warning) %s(%p) has no parent -> write result './new_result1.net'\n",
       iNode->attribute("name").toLatin1().constData(), iNode);
#endif
    QFile fp("new_result1.net");
    fp.open(QIODevice::WriteOnly);
    QXmlStreamWriter str(&fp);
    str.setAutoFormatting(true);
    str.writeStartDocument();
    str.writeStartElement("nodelist");
    iNode->write(str);
    str.writeEndElement();
    str.writeEndDocument();
    fp.close();
  }

  {                             // Debug
    for (QList < INode* >::const_iterator it = groupList->constBegin(); 
	 it!=groupList->constEnd(); 
	 ++it) {
#ifdef DEBUG_MSG
      qDebug("count=%d\n", (*it)->children().count());
#endif
    }
  }
  return *groupList;
}

/** indicates whether or not the node is a temporal one */
bool SNode::temporal(void)
{
	return attributeBool("temporal_node");
}

/** says whether the node is hollistisch */
bool SNode::holistic(void)
{
  if (topDown_)
    return TRUE;
  else
    return FALSE;
}

/** No descriptions */
uint SNode::color()
{
  return color_;
}

bool SNode::synchron_ = FALSE;

/** find a snode with the given classname. Only direct children of this snode are checked. */
SNode *SNode::findClass(QString classname)
{
  for (QList < SNode* >::const_iterator it= children().constBegin();
       it!=children().constEnd(); 
       ++it) {
    SNode *node = (*it);
    if (QString::compare(classname, node->attribute("class"),Qt::CaseInsensitive) == 0)
      return node;
  }
  return 0;
}

void SNode::setGuiPtr(TreeItem * ptr)
{
  GNode::setGuiPtr(ptr);
  tdCounter_ = 0;
  buCounter_ = 0;
}

/** change an operator and change the corresponding attributes */
void SNode::changeOperator(Operator * &op, QString opName)
{
  Operator *newOp = operatorList_[opName];
  if (op == newOp)
    return;
  if (op) {
    AttribListConstIterator it = attribList().constBegin();
    while (it!=attribList().constEnd()) {
      if (it.key().contains(op->type().toLower() + ".")) {
        attribList().remove(it.key());
      }
      else
        ++it;

    }
  }
  op = newOp;
  if (!op)
    return;
  AttributeDictionary *attribs = op->attributeDesc();
  if (attribs) {
    for (AttributeDictionary::const_iterator it = attribs->constBegin(); 
	 it!=attribs->constEnd(); 
	 ++it) {
      QString name = (*it)->fullname();
      QString val = (*it)->value();
      if (!val.isEmpty())
        attribList().replace(name, val);
    }
  }

}
/** increase the reference counter */
void SNode::link()
{
  refCounter_++;
}

/** decrease the reference counter */
void SNode::unlink()
{
  refCounter_--;
  Q_ASSERT(refCounter_>=0);
}

void SNode::stateChanged(TreeItem*)
{
  qWarning("SNode::stateChanged function not implemented");
}
