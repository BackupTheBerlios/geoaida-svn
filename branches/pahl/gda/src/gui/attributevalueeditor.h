/***************************************************************************
                          attributevalueeditor.h  -  description
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

#ifndef ATTRIBUTEVALUEEDITOR_H
#define ATTRIBUTEVALUEEDITOR_H


#include <QItemDelegate>
#include <QWidget>

class Attribute;

#if 0
class AttributeValueEditor : public QWidget
{
  Q_OBJECT
public:
  //  QSize sizeHint() const =0;
  virtual void setValue(QString value) =0;
  virtual QString value() =0;
  
 signals:
  void editingFinished();
  
 protected:
  //  void paintEvent(QPaintEvent *event) =0;
  //  void mouseMoveEvent(QMouseEvent *event) =0;
  //  void mouseReleaseEvent(QMouseEvent *event) =0;
 protected:
  Attribute* attribute_;
};

#endif

#include <QLineEdit>

class StringEditor : public QLineEdit
{
  Q_OBJECT
    
public:
  StringEditor(QWidget *parent, Attribute *attribute);
  QSize sizeHint() const;
  
  void setValue(const QString& value);
  QString value();
};


#include <QComboBox>

class OperatorListEditor : public QComboBox
{
  Q_OBJECT
    
public:
  OperatorListEditor(QWidget *parent, Attribute *attribute);
  QSize sizeHint() const;
  
  void setValue(const QString& value);
  QString value();
};


#endif
