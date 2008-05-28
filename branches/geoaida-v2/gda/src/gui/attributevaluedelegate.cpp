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
#include "FormulaEditor"
#include "OperatorListEditor"
#include "ImageListEditor"
#include "EnumEditor"
#include "BoolEditor"
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
#define CreateEditor(EditorType) \
      EditorType* editor=new EditorType(parent, attrib); \
      connect(editor, SIGNAL(editingFinished()),         \
	      this, SLOT(commitAndCloseEditor()));       

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
#endif
    case Attribute::BOOL:{
      CreateEditor(BoolEditor);
      return editor;
    }
    case Attribute::FORMULA:{
      CreateEditor(FormulaEditor);
      return editor;
    }
    case Attribute::ENUM:{
      CreateEditor(EnumEditor);
      return editor;
    }
    case Attribute::OPERATOR:{
      CreateEditor(OperatorListEditor);
      return editor;
    }
    case Attribute::IMAGE:{
      CreateEditor(ImageListEditor);
      return editor;
    }
    default: {
      CreateEditor(StringEditor);
      return editor;
    }
    }

  } else {
    return QItemDelegate::createEditor(parent, option, index);
  }

#undef CreateEditor
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
#define SetEditorData(EditorType) \
      EditorType *attribeditor = qobject_cast<EditorType *>(editor); \
      attribeditor->setValue(attribValue.value()); \

  if (qVariantCanConvert<AttributeValue>(index.data(Qt::EditRole))) {
    AttributeValue attribValue = qVariantValue<AttributeValue>(index.data(Qt::EditRole));
    Attribute* attrib=attribValue.attribute();
    switch (attrib->type()) {
#if 0
    case Attribute::INT:
      return value.toInt();
    case Attribute::DOUBLE:
      return value.toDouble();
#endif
    case Attribute::BOOL:{
      SetEditorData(BoolEditor);
      return;
    }
    case Attribute::FORMULA:{
      SetEditorData(FormulaEditor);
      return;
    }
    case Attribute::ENUM:{
      SetEditorData(EnumEditor);
      return;
    }
    case Attribute::OPERATOR:{
      SetEditorData(OperatorListEditor);
      return;
    }
    case Attribute::IMAGE:{
      SetEditorData(ImageListEditor);
      return;
    }
    default: {
      SetEditorData(StringEditor);
      return;
    }
    }
  } 
  else 
    QItemDelegate::setEditorData(editor, index);

#undef SetEditorData
}

void AttributeValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const
{
#define SetModelData(EditorType) \
      EditorType *attribeditor = qobject_cast<EditorType *>(editor); \
      model->setData(index, attribeditor->value(), Qt::EditRole);

  if (qVariantCanConvert<AttributeValue>(index.data(Qt::EditRole))) {
    AttributeValue attribValue = qVariantValue<AttributeValue>(index.data(Qt::EditRole));
    Attribute* attrib=attribValue.attribute();
    switch (attrib->type()) {
    case Attribute::BOOL:{
      SetModelData(BoolEditor);
      return;
    }
    case Attribute::ENUM:{
      SetModelData(EnumEditor);
      return;
    }
    case Attribute::FORMULA:{
      SetModelData(FormulaEditor);
      return;
    }
    case Attribute::OPERATOR:{
      SetModelData(OperatorListEditor);
      return;
    }
    case Attribute::IMAGE:{
      SetModelData(ImageListEditor);
      return;
    }
    default: {
      SetModelData(StringEditor);
      return;
    }
    }
  }
  else
    QItemDelegate::setModelData(editor,model,index);

#undef SetModleData
}
