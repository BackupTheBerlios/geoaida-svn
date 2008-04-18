/***************************************************************************
                          attributemodel.cpp  -  description
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
 * $Source: /data/cvs/gda/gda/core/attributemodel.cpp,v $
 * $Revision: 1.8 $
 * $Date: 2002/06/28 07:27:25 $
 * $Author: pahl $
 * $Locker:  $
 */

#include "attributemodel.h"
#include "GNode"
#include <QFile>
#include <AttributeModel>
#ifdef WIN32
#include <stdlib.h> // für exit
#endif

/*!
 * \brief constructor
 */
AttributeModel::AttributeModel()
  : node_(0)
{
}

/*!
 * \brief destructor
 */
AttributeModel::~AttributeModel()
{
}

/*!
 * \name Model interface for read-only access 
*/ 
//@{

/*!
 * \brief return the number of columns for the given object
 *
 * In the AttributeModel-model columns represent the attributes of one node.
 *
 * \param parent - the node for which to return number of columns.
 * \return number of columns
 */
int AttributeModel::columnCount( const QModelIndex & parent ) const
{
  return 2;
}

/*!
 * \brief return the number of rows for the given object
 *
 * In the AttributeModel-model rows represent the children of one node.
 *
 * \param parent - the node for which to return number of rows.
 * \return number of rows
 */
int AttributeModel::rowCount( const QModelIndex & parent ) const
{
  if (!node_) return 0;
  if (!parent.isValid()) {
    return section_.count();
  }
  int section=parent.internalId();
  if (section!=-1) return 0;
  QString sectionName=section_[parent.row()];
  if (attributesDict_.contains(sectionName)) 
    return attributesDict_.value(sectionName).count();
  return 0;
}

/*!
 * \brief return data from the given object
 *
 * \param index - the node for which to return some data.
 * \param role - the role of the data which should be returned
 * \return number of rows
 */
QVariant AttributeModel::data( const QModelIndex & index, int role ) const
{
  if (role != Qt::DisplayRole )
    return QVariant();
  int section=index.internalId();
  if (section==-1) {
    if (index.column()==0) 
      return section_[index.row()];
    return QVariant();
  }
  QString sectionName=section_[section];
  if (attributesDict_.contains(sectionName)) {
    Attribute* attrib=attributesDict_.value(sectionName)[index.row()];
    if (index.column()==0) {
      return attrib->name();
    }
    if (index.column()==1) {
      return node_->attribute(attrib->name());
    }
  }

  return QString("Test");

#if 0
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
#endif
  return QVariant();
}


/*!
 * \brief return an index pointing to the requested object
 *
 * \param row - the position of the children to be returned
 * \param column - the role of the data which should be returned
 * \return number of rows
 */
QModelIndex AttributeModel::index (int row, int column, 
			   const QModelIndex & parent ) const
{
  if (!node_) return QModelIndex();
  if (!parent.isValid()) {
    return createIndex(row, column, -1);
  }
  else {
    int section=parent.internalId();
    if (section==-1)
      return createIndex(row, column, parent.row());
  }
  return QModelIndex();
}

/*!
 * \brief return an index pointing to parent of the given index
 *
 * \param child - index of child
 * \return index of parent
 */
QModelIndex AttributeModel::parent(const QModelIndex &child) const
{
  int section=child.internalId();
  if (section==-1) return QModelIndex();
  return createIndex(section, 0, -1);
}

QVariant AttributeModel::headerData ( int section, 
				      Qt::Orientation orientation, 
				      int role ) const 
{
  if (role!=Qt::DisplayRole) return QVariant();
  switch (section) {
  case 0:
    return QString(tr("Name"));
  case 1:
    return QString(tr("Value"));
  }
  return QVariant();
}

/*!
 * \brief return flags for the given index
 *
 * \param index
 * \return Qt::ItemFlags
 */
Qt::ItemFlags AttributeModel::flags ( const QModelIndex & index ) const
{
}


/*!
 * \brief convert internal pointer of a QModelIndex to SNode
 *
 * \param index
 * \return pointer to SNode
 */
#if 0
SNode* AttributeModel::nodeFromIndex(const QModelIndex& index) const
{
  if (index.isValid()) {
    return static_cast<SNode *>(index.internalPointer());
  } else {
    return rootNode_;
  }
}
#endif


//@}


/*!
 * \name AttributeModel-specific interface 
 */
//@{

/*! \brief set the node  */
void AttributeModel::setNode(GNode* node) 
{
  qDebug("AttributeModel::setNode");
  node_=node;
  if (!node_) return;
  section_=node_->attributeSections();
  for (int i=0; i<section_.count(); i++) {
    AttributeDictionary* attribDict=node_->attributeDesc(section_[i]);
    if (!attribDict) continue;
    QList<Attribute*> list;
    for (AttributeDictionary::const_iterator it=attribDict->constBegin();
	 it!=attribDict->constEnd();
	 ++it) {
      list.append(*it);
    }
    attributesDict_.insert(section_[i],list);
  }
  reset();
}

/*! \brief set the node  */
void AttributeModel::setNode(const QModelIndex & index) 
{
  GNode* node=0;
  if (index.isValid()) 
    node=static_cast<GNode *>(index.internalPointer());
  setNode(node);
}

//@}
