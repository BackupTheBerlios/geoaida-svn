/***************************************************************************
                          attributeeditor.cpp  -  description
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

#include <QItemDelegate>
#include "AttributeValue"
#include "attributevalueeditor.h"

#if 0
class AttributeValueEditor : public QWidget
{
  Q_OBJECT
  
public:
     AttributeValueEditor(QWidget *parent = 0);

     QSize sizeHint() const;
     void setOperatorListRating(const OperatorListRating &starRating) {
         myOperatorListRating = starRating;
     }
     OperatorListRating starRating() { return myOperatorListRating; }

 signals:
     void editingFinished();

 protected:
     void paintEvent(QPaintEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);

 };
#endif

StringEditor::StringEditor(QWidget *parent, Attribute* attribute)
  : QLineEdit(parent)
{
}

void StringEditor::setValue(const QString& value)
{
  setText(value);
}

QString StringEditor::value()
{
  return text();
}

QSize StringEditor::sizeHint() const
{
  qDebug("StringEditor::sizeHint()");
  QSize size=QLineEdit::sizeHint();
  size.setHeight(size.height()*1.3);
  return size;
}


#include <QStringList>
#include "OperatorList"

OperatorListEditor::OperatorListEditor(QWidget *parent, Attribute* attribute)
  : QComboBox(parent)
{
  const QStringList& options=operatorList_.keys(attribute->typeOfOperator());
  addItems(options);
}

void OperatorListEditor::setValue(const QString& value)
{
  setCurrentIndex(findText(value));
}

QString OperatorListEditor::value()
{
  return currentText();
}

QSize OperatorListEditor::sizeHint() const
{
  qDebug("OperatorListEditor::sizeHint()");
  QSize size=QComboBox::sizeHint();
  size.setHeight(size.height()*1.3);
  return size;
}






