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
  /** Copy constructor */
  GNode(GNode & node);
  /** Construct a GNode by parsing a file */
  GNode(MLParser & parser);
  virtual ~ GNode();
  /** Read a GNode by parsing a file */
  void read(MLParser & parser);
  /** Write the node-Date to a file */
  void write(QXmlStreamWriter & fp, bool recursive = true);
  /** returns the name of this node */
  virtual const QString & name() const;
  /** Set the name_ */
  void name(QString name);
  /** Get the value of attribute key */
  const QString & attribute(QString key);
  /** Get the value of attribute key as float*/
  const float attributeFloat(QString key);
  /** Get the value of attribute key as int*/
  const int attributeInt(QString key);
  /** Get the value of attribute key as bool*/
  const bool attributeBool(QString key);
  /** Set the attribute key to val */
  void attribute(QString key, const char* val);
  /** Set the attribute key to val */
  void attribute(QString key, double val);
  /** Set the attribute key to val */
  void attribute(QString key, int val);
  /** Set the attribute key to val */
  void attribute(QString key, QString val);
  /** Remove the attribute key */
  bool attributeRemove(QString key);
  /** Get the attribute list */
    ArgDict & attribList();
  /** Get the name of the node */
  void isCompound(bool val);
  /** This node is a compound of its subnodes. */
  bool isCompound();
  /**  Makes a copy of this node */
  virtual GNode* copy() =0;
  /** return a new Node */
  virtual GNode *newNode() =0;
  /** Set variables of this node using the attributes */
  virtual void setVars();
#if 0
  virtual QStringList & predefinedAttribs();
#endif
  /** returns true if this class is a cname */
  virtual bool isA(QString cname);
  /** Configure this node using attribList    */
  void configure(ArgDict & attribList);
  /** Update attributes of this node with the given attribute list  */
  void update(ArgDict & attribList);

protected:                     // Protected methods
  /** Init member variables */
    virtual void init();
  /** Set the attribute key to val */
  virtual void attributeSet(QString key, QString val);
protected:                     // Protected attributes
  /** name of the node */
  QString name_;
  /** list of attributes */
  ArgDict attribList_;
public:                        // Public attributes
  enum
  {
    TOK_NODE = 1,
    TOK_REGION,
    TOK_GROUP
  };
  /** Predefined Attributes */
//  static QStringList predefinedAttribs_;
};


#endif
