/***************************************************************************
                          attributevaluedelegate.h  -  description
                             -------------------
    begin                : 2008-05-05
    copyright            : (C) 2008 by Martin Pahl
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

#ifndef ATTRIBUTEVALUEDELEGATE_H
#define ATTRIBUTEVALUEDELEGATE_H

#include <QItemDelegate>
#include "AttributeValue"

class Attribute;

class AttributeValueDelegate : public QItemDelegate
{
  Q_OBJECT
    
public:
  AttributeValueDelegate(QWidget *parent = 0);

//      void paint(QPainter *painter, const QStyleOptionViewItem &option,
//                 const QModelIndex &index) const;
  QSize sizeHint(const QStyleOptionViewItem &option,
		 const QModelIndex &index) const;
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
		    const QModelIndex &index) const;
  
private slots:
  void commitAndCloseEditor();
 };

#endif
