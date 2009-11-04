/***************************************************************************
                          gnode.h  -  description
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

#ifndef GNODE_H
#define GNODE_H

#include "MLParser"
#include "TreeGNode"
#include "TreeNode"
#include "ArgDict"
#include <QStringList>
#include <QList>
#include <QMultiHash>
#include <QXmlStreamWriter>
/**Base Node
  *@author Martin Pahl
  */

extern const MLTagTable nodeTagTable;

class OperatorObject;
class TreeItem;

class GNode:public TreeNode < GNode, TreeGNode >
{
public:                        // Public attributes
  GNode();
  GNode(GNode & node);
  virtual ~ GNode();

  virtual GNode* copy() =0;
  virtual GNode *newNode() =0;
  virtual bool isA(QString cname);

  void read(MLParser & parser);
  void write(QXmlStreamWriter & fp, bool recursive = true);

  virtual const QString & name() const;
  void name(QString name);

  const QString & attribute(QString key);
  const float attributeFloat(QString key);
  const int attributeInt(QString key);
  const bool attributeBool(QString key);
  void attribute(QString key, const char* val);
  void attribute(QString key, double val);
  void attribute(QString key, int val);
  void attribute(QString key, QString val);
  bool attributeRemove(QString key);
  ArgDict & attribList();

  virtual void setVars();
  void configure(ArgDict & attribList);
  void update(ArgDict & attribList);

protected:               
  virtual void attributeSet(QString key, QString val);

  QString name_;
  ArgDict attribList_;
public:                  
  enum
  {
    TOK_NODE = 1,
    TOK_REGION,
    TOK_GROUP
  };
};


#endif
