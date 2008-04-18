/***************************************************************************
                          operator.cpp  -  description
                             -------------------
    begin                : Thu Oct 5 2000
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

/*
 * $Source: /data/cvs/gda/gda/core/operator.cpp,v $
 * $Revision: 1.11 $
 * $Date: 2002/06/15 09:36:51 $
 * $Author: gerhards $
 * $Locker:  $
 */

#include "operator.h"
#include <QRegExp>

Operator::Operator()
{
  type_ = "topdown";
  runGlobal_ = false;
}

/** Generate a new operator using the provided parser */
Operator::Operator(MLParser & parser)
{
  type_ = "topdown";
  runGlobal_ = false;
  read(parser);
}

Operator::~Operator()
{
}

/** execute the operator returning the pid */
QString
Operator::command(AttribList & attribList)
{
  QString cmd = cmd_;
  {                             // Replace the attributes known by this operator
    QMultiHash < QString, Attribute* >::const_iterator  it = attributeDict_.constBegin();
    for (; it!=attributeDict_.constEnd(); ++it) {
      //D                     qDebug("Operator::command %s\n",(const char*)it.currentKey());
      cmd.replace(QRegExp("@" + it.key() + "@", Qt::CaseInsensitive),
                  it.value()->command(attribList));
    }
  }
  {                             // Replace all unknown attributes
    AttribListConstIterator it = attribList.constBegin();
    for (; it!=attribList.constEnd(); ++it) {
      //D                     qDebug("Operator::command %s\n",(const char*)it.currentKey());
      cmd.replace(QRegExp("@" + it.key() + "@", Qt::CaseInsensitive),
                  it.value());
    }
  }

  qDebug("Operator::command: %s\n", cmd.toLatin1().constData());

  return cmd;
}

/** returns the name of the operator */
QString
Operator::name()
{
  return name_;
}

/** read attributes for this operator through parser */
void
Operator::read(MLParser & parser)
{
  ArgDict *args = parser.args();
  MLParser::setString(name_, args, "name");
  MLParser::setString(class_, args, "class");
  MLParser::setString(cmd_, args, "cmd");
  MLParser::setString(type_, args, "type");
  MLParser::setString(tip_, args, "tip");
  MLParser::setBool(runGlobal_, args, "runglobal");
  delete args;
  qDebug("Operator (%s/%s/%s) %s\n",
         type_.toLatin1().constData(), 
	 class_.toLatin1().constData(), 
	 name_.toLatin1().constData(),
         cmd_.toLatin1().constData());

  QString keywords[] = { "operator", "attribute", "" };
  const MLTagTable nodeTagTable(keywords);
  enum
  { TOK_OPERATOR = 1, TOK_ATTRIBUTE };
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
    qDebug("Operator::read tag=%d\n", tag);
    args = parser.args();
    switch (tag) {
    case TOK_ATTRIBUTE:
      Attribute * attrib = new Attribute();
      attrib->set(*args);
#ifdef WIN32
      attrib->prefix(type()+"\\");
#else
      attrib->prefix(type()+"/");
#endif
      qDebug("Operator::read new Attribute %s\n",
             attrib->name().toLatin1().constData());
      attributeDict_.insert(attrib->name(), attrib);
      break;
    }
    delete args;
  } while ((tag != -TOK_OPERATOR) && (tag != MLParser::END_OF_FILE));

}

/** Returns the description of the attributes */
QMultiHash<QString, Attribute* > *Operator::attributeDesc()
{
//      qDebug("Operator::attributeDesc() name=%s",name_.latin1());
  return &attributeDict_;
}


/** returns the type of operator (topdown, bottomup ...)   */
QString
Operator::type()
{
  return type_;
}

/** returns a tip for this operator */
QString
Operator::tip()
{
  return tip_;
}

/** Returns true if operator works best on whole image, otherwise returns false */
bool
Operator::runGlobal()
{
  return runGlobal_;
}
