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

class Attribute;

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


/******************************************************************************/
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

/******************************************************************************/
#include <QComboBox>

class ImageListEditor : public QComboBox
{
  Q_OBJECT
    
public:
  ImageListEditor(QWidget *parent, Attribute *attribute);
  QSize sizeHint() const;
  
  void setValue(const QString& value);
  QString value();
};

/******************************************************************************/
#include <QComboBox>

class EnumEditor : public QComboBox
{
  Q_OBJECT
    
public:
  EnumEditor(QWidget *parent, Attribute *attribute);
  QSize sizeHint() const;
  
  void setValue(const QString& value);
  QString value();
};


/******************************************************************************/
#include <QCheckBox>

class BoolEditor : public QCheckBox
{
  Q_OBJECT
    
public:
  BoolEditor(QWidget *parent, Attribute *attribute);
  QSize sizeHint() const;
  
  void setValue(const QString& value);
  QString value();
};


/******************************************************************************/
#include <QTextEdit>

class FormulaEditor : public QTextEdit
{
  Q_OBJECT
    
public:
  FormulaEditor(QWidget *parent, Attribute *attribute);
  QSize sizeHint() const;
  
  void setValue(const QString& value);
  QString value();
};




#endif
