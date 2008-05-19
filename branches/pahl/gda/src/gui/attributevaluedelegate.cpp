/***************************************************************************
                          attributevaluedelegate.cpp  -  description
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


#include "attributevaluedelegate.h"
#include <QModelIndex>
#include <QVariant>
#include "StringEditor"
#include "OperatorListEditor"
#include "Attribute"

AttributeValueDelegate::AttributeValueDelegate(QWidget *parent) 
  : QItemDelegate(parent)
{
}

QSize AttributeValueDelegate::sizeHint(const QStyleOptionViewItem &option,
				       const QModelIndex &index) const
{
  qDebug("AttributeValueDelegate::sizeHint()");
  QSize size=QItemDelegate::sizeHint(option, index);
  size.setHeight(size.height()*1.3);
  return size;
}


QWidget *AttributeValueDelegate::createEditor(QWidget *parent,
					    const QStyleOptionViewItem &option,
					    const QModelIndex &index) const
  
{
  if (qVariantCanConvert<AttributeValue>(index.data(Qt::EditRole))) {
    AttributeValue attribValue=qVariantValue<AttributeValue>(index.data(Qt::EditRole));
    Attribute* attrib=attribValue.attribute();
    if (!attrib) return 0;
    switch (attrib->type()) {
#if 0
    case Attribute::INT:
      return value.toInt();
    case Attribute::DOUBLE:
      return value.toDouble();
    case Attribute::BOOL:
      return value=="true";
#endif
    case Attribute::OPERATOR:{
      OperatorListEditor* editor=new OperatorListEditor(parent, attrib);
      connect(editor, SIGNAL(editingFinished()),
	      this, SLOT(commitAndCloseEditor()));
      return editor;
    }
    default: {
      StringEditor* editor=new StringEditor(parent, attrib);
      connect(editor, SIGNAL(editingFinished()),
	      this, SLOT(commitAndCloseEditor()));
      return editor;
    }
    }

  } else {
    return QItemDelegate::createEditor(parent, option, index);
  }
}

void AttributeValueDelegate::commitAndCloseEditor()
{
  QWidget *editor = qobject_cast<QWidget *>(sender());
  emit commitData(editor);
  emit closeEditor(editor);
}

void AttributeValueDelegate::setEditorData(QWidget *editor,
					   const QModelIndex &index) const
{
  if (qVariantCanConvert<AttributeValue>(index.data(Qt::EditRole))) {
    AttributeValue attribValue = qVariantValue<AttributeValue>(index.data(Qt::EditRole));
    Attribute* attrib=attribValue.attribute();
    switch (attrib->type()) {
#if 0
    case Attribute::INT:
      return value.toInt();
    case Attribute::DOUBLE:
      return value.toDouble();
    case Attribute::BOOL:
      return value=="true";
#endif
    case Attribute::OPERATOR:{
      OperatorListEditor *attribeditor = qobject_cast<OperatorListEditor *>(editor);
      attribeditor->setValue(attribValue.value());
      return;
    }
    default: {
      StringEditor *attribeditor = qobject_cast<StringEditor *>(editor);
      attribeditor->setValue(attribValue.value());
      return;
    }
    }
  } 
  else 
    QItemDelegate::setEditorData(editor, index);
  
}

void AttributeValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const
{
  if (qVariantCanConvert<AttributeValue>(index.data(Qt::EditRole))) {
    AttributeValue attribValue = qVariantValue<AttributeValue>(index.data(Qt::EditRole));
    Attribute* attrib=attribValue.attribute();
    switch (attrib->type()) {
    case Attribute::OPERATOR:{
      OperatorListEditor *attribeditor = qobject_cast<OperatorListEditor *>(editor);
      model->setData(index, attribeditor->value(), Qt::EditRole);
      return;
    }
    default: {
      StringEditor *attribeditor = static_cast<StringEditor*>(editor);
      model->setData(index, attribeditor->value(), Qt::EditRole);
      return;
    }
    }
  }
  else
    QItemDelegate::setModelData(editor,model,index);
}
