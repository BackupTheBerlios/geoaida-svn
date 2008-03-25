/*!*************************************************************************
 * \file attributemodel.h 
 * \brief Prototype for class "AttributeModel"
 *
 * \date
 *  begin                : Mon Sep 4 2000
 * \author 
 *  copyright            : (C) 2000 by Martin Pahl
 *  email                : pahl@tnt.uni-hannover.de
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
 * $Source: /data/cvs/gda/gda/core/attributemodel.h,v $
 * $Revision: 1.3 $
 * $Date: 2001/11/27 16:35:30 $
 * $Author: pahl $
 * $Locker:  $
 */

#ifndef ATTRIBUTEMODEL_H
#define ATTRIBUTEMODEL_H

#include <QTextStream>
#include <QAbstractItemModel>
#include <QHash>
#include <QList>
#include <QStringList>

class GNode;
class Attribute;

/*!
 * \brief Class for Semantic Net of type SNode
 *
 */

class AttributeModel : public QAbstractItemModel
{
Q_OBJECT
public:
  AttributeModel();
  ~AttributeModel();

  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex &child) const;
  virtual QVariant headerData ( int section, 
				Qt::Orientation orientation, 
				int role = Qt::DisplayRole ) const;
 private:
  //  GNode* nodeFromIndex(const QModelIndex& index) const;

 public slots:
  void setNode(GNode* node);
  void setNode(const QModelIndex & index);
protected:     
  /** node containing the attributes */
  GNode * node_;
  QStringList section_;
  QHash < QString, QList <Attribute*> > attributesDict_;
};
#endif
