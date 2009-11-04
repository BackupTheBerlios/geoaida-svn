/***************************************************************************
                          gnode.cpp  -  description
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

#include "gnode.h"


// #define DEBUGMSG

const static QString nullString("");

static QString keywords[] = { "node", "region", "group", "" };
const MLTagTable nodeTagTable(keywords);

GNode::GNode()
{
  init();
}

GNode::~GNode()
{
  qDeleteAll(children()); // while (!children().isEmpty()) delete children().takeFirst();
#if defined WITH_GUI_SUPPORT || WIN32
  if (guiPtr_) delete guiPtr_;
#endif
}

/** Copy constructor */
GNode::GNode(GNode & node)
{
  init();
  attribList_=node.attribList_;
  name_ = node.name_;
  {
    QList<GNode*>::const_iterator it = node.children().constBegin();
    for (; it!=node.children().constEnd(); ++it) {
      childLink((*it)->copy());
    }
  }
}

/** Construct a GNode by parsing a file */
GNode::GNode(MLParser & parser)
{
  init();
  read(parser);
}

/** Init member variables */
void GNode::init()
{
  name_ = ""; 
  parent_ = 0;
//  qDebug ("GNode generiert");

}

/** Read GNode by parsing a file */
void GNode::read(MLParser & parser)
{
  parser.args(&attribList_,false);
  setVars();

  int tag = parser.tag(nodeTagTable);
  while (tag == TOK_NODE) {
    GNode *node = this->newNode();
    node->read(parser);
    childLink(node);
    tag = parser.tag(nodeTagTable);
  }
  if (tag != -TOK_NODE) {
    qDebug("Warning: /node missing\n");
  }
}

/** Write the node-Date to a file */
void GNode::write(QXmlStreamWriter & fp, bool recursive)
{
  if (children().count() && recursive ) 
    fp.writeStartElement("node");
  else
    fp.writeEmptyElement("node");
  fp << attribList_;
  if (children().count() && recursive ) {
    QList<GNode*>::const_iterator it = children().constBegin();
    for (; it!=children().constEnd(); ++it) {
      (*it)->write(fp);
    }
    fp.writeEndElement();
  }
}

/** Set the name_ */
void GNode::name(QString name)
{
  attributeRemove("name");
  attribute("name", name);
}

/** Set the attribute key to val */
void GNode::attribute(QString key, QString val)
{
  attributeSet(key.toLower(), val);
}

/** returns the name of this node */
const QString & GNode::name() const
{
  return name_;
}

/** Set the attribute key to val */
void GNode::attribute(QString key, int val)
{
  attributeSet(key, QString().setNum(val));
}

/** Set the attribute key to val */
void GNode::attribute(QString key, double val)
{
  attributeSet(key, QString().sprintf("%f", val));
}

/** Set the attribute key to val */
void GNode::attribute(QString key, const char* val)
{
  attributeSet(key, QString(val));
}

/** Get the value of attribute key */
const QString & GNode::attribute(QString key)
{
  key=key.toLower();
  if (attribList_.contains(key))
    return attribList_[key];
  else
    return nullString;
}

/** Get the value of attribute key as float*/
const float GNode::attributeFloat(QString key)
{
#ifdef DEBUGMSG
  qDebug("GNode::attributeFloat(%s)=%f (%s)",
         key.latin1(), attribute(key).toFloat(), attribute(key).latin1());
#endif
  return attribute(key).toFloat();
}

/** Get the value of attribute key as int*/
const int GNode::attributeInt(QString key)
{
  return attribute(key).toInt();
}

/** Get the value of attribute key as bool */
const bool GNode::attributeBool(QString key)
{
  bool val = false;
  MLParser::setBool(val, &attribList_, key);
  return val;
}

/** Remove the attribute key */
bool GNode::attributeRemove(QString key)
{
  return attribList_.remove(key);
}

/** Get the attribute list */
ArgDict & GNode::attribList()
{
  return attribList_;
}

/** Set the attribute key to val */
void GNode::attributeSet(QString key, QString val)
{
  attribList_.replace(key, val);
  if (key == "name")
    name_ = val;
}

/** Set variables of this node using the attributes */
void GNode::setVars()
{
  MLParser::setString(name_, &attribList_, "name");
}

/** returns true if this class is a cname */
bool GNode::isA(QString cname)
{
  return (cname == "GNode");
}


/** Configure this node using attribList    */
void GNode::configure(ArgDict & attribList)
{
  attribList_=attribList;
}

/** Update attributes of this node with the given attribute list  */
void GNode::update(ArgDict & attribList)
{
  ArgDictConstIterator it = attribList.constBegin();
  for (; it!=attribList.constEnd(); ++it) {
    attributeSet(it.key(), it.value());
  }
}


