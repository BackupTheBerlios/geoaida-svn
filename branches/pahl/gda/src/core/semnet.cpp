/***************************************************************************
                          semnet.cpp  -  description
                             -------------------
    begin                : Mon Sep 4 2000
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
 * $Source: /data/cvs/gda/gda/core/semnet.cpp,v $
 * $Revision: 1.8 $
 * $Date: 2002/06/28 07:27:25 $
 * $Author: pahl $
 * $Locker:  $
 */

#include "semnet.h"
#include "SNode"
#include <QFile>
#include <QIcon>
#include <QPixmap>
#ifdef WIN32
#include <stdlib.h> // für exit
#endif

/*!
 * \brief constructor
 */
SemNet::SemNet()
{
  rootNode_ = new SNode();
#ifdef WIN32
        if (rootNode_ == 0){
          cout << "Out of Memory..14";
          exit(1);
        }
#endif
  rootNode_->name("Szene");
//  rootNode_->xxxmin(1);
//  rootNode_->xxxmax(1);
}

/*!
 * \brief destructor
 */
SemNet::~SemNet()
{
  if (rootNode_)
    delete rootNode_;
}

/*!
 * \name Model interface for read-only access 
*/ 
//@{

/*!
 * \brief return the number of columns for the given object
 *
 * In the SemNet-model columns represent the attributes of one node.
 *
 * \param parent - the node for which to return number of columns.
 * \return number of columns
 */
int SemNet::columnCount ( const QModelIndex & parent ) const
{
  return 1;
}

/*!
 * \brief return the number of rows for the given object
 *
 * In the SemNet-model rows represent the children of one node.
 *
 * \param parent - the node for which to return number of rows.
 * \return number of rows
 */
int SemNet::rowCount ( const QModelIndex & parent ) const
{
  SNode *parentNode = nodeFromIndex(parent);
  if (!parentNode)
    return 0;
  return parentNode->children().count();
}

/*!
 * \brief return data from the given object
 *
 * \param index - the node for which to return some data.
 * \param role - the role of the data which should be returned
 * \return number of rows
 */
QVariant SemNet::data ( const QModelIndex & index, int role ) const
{
  if (role != Qt::DisplayRole && role != Qt::DecorationRole )
    return QVariant();
  SNode *node = nodeFromIndex(index);
  if (!node)
    return QVariant();
  if (index.column() == 0) {
    switch (role) {
    case Qt::DisplayRole:
      return node->name();
    case Qt::DecorationRole:
      if (node->isA("SNode")) {
	uint color=node->color();
	if (pixmapHash_.contains(color)) {
	  return QIcon(pixmapHash_.value(color));
	}
	else {
	  QPixmap p(20,10);
	  p.fill(color);
	  pixmapHash_.insert(color,p);
	  return QIcon(p);
	}
	return QVariant();
      }
      else return QVariant();
    }
  }
  return QVariant();
}


/*!
 * \brief return an index pointing to the requested object
 *
 * \param row - the position of the children to be returned
 * \param column - the role of the data which should be returned
 * \return number of rows
 */
QModelIndex SemNet::index (int row, int column, 
			   const QModelIndex & parent ) const
{
    if (!rootNode_)
        return QModelIndex();
    SNode *parentNode = nodeFromIndex(parent);
    return createIndex(row, column, parentNode->children()[row]);
}

/*!
 * \brief return an index pointing to parent of the given index
 *
 * \param child - index of child
 * \return index of parent
 */
QModelIndex SemNet::parent(const QModelIndex &child) const
 {
    SNode *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();
    SNode *parentNode = node->parent();
    if (!parentNode)
        return QModelIndex();
    SNode *grandparentNode = parentNode->parent();
    if (!grandparentNode)
        return QModelIndex();
    int row = grandparentNode->children().indexOf(parentNode);
    return createIndex(row, child.column(), parentNode);
}

QVariant SemNet::headerData ( int section, 
			      Qt::Orientation orientation, 
			      int role ) const 
{
  if (role!=Qt::DisplayRole) return QVariant();
  return QString(tr("Node"));
}

/*!
 * \brief convert internal pointer of a QModelIndex to SNode
 *
 * \param index
 * \return pointer to SNode
 */
SNode* SemNet::nodeFromIndex(const QModelIndex& index) const
{
  if (index.isValid()) {
    return static_cast<SNode *>(index.internalPointer());
  } else {
    return rootNode_;
  }
}


//@}


/*!
 * \name Semnet-specific interface 
 */
//@{

/** Read a semantic net  */
void SemNet::read(QIODevice & fp)
{
  if (rootNode_) {
    delete rootNode_;
    rootNode_=0;
  }
  MLParser parser(&fp);
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
  } while ((tag != TOK_NODE) && (tag != MLParser::END_OF_FILE));
  if (tag == TOK_NODE)
    rootNode_ = new SNode(parser);
  if (rootNode_)
    qDebug("SemNet: Root=%s\n", rootNode_->name().toLatin1().constData());
#ifdef WIN32
#warning cout ist gar nicht definiert!!!!!
  else {
    cout << "Out of Memory..14";
    exit(1);
  }
#endif
}

/** Read a semantic net by using a filename */
void SemNet::read(const QString & fname)
{
  QFile fp(fname);
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("SemNet::read(%s): file not found", fname.toLatin1().constData());
    return;
  }
  read(fp);
  fp.close();
}

/** write the semantic net to the file fp */
void SemNet::write(QTextStream & fp)
{
  if (rootNode_)
    rootNode_->write(fp, QString(""));
}

/** Write the semantic net to the file fname */
void SemNet::write(const QString & fname)
{
  QFile fp(fname);
  if (!fp.open(QIODevice::WriteOnly)) {
    qDebug("SemNet::write(%s): file not accessable", fname.toLatin1().constData());
    return;
  }
  QTextStream str(&fp);
  write(str);
  fp.close();
}

/** Get the rootNode_ of the semantic net */
SNode *SemNet::rootNode(void)
{
  return rootNode_;
}

//@}
