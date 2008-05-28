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
/******************************************************************************/
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


/******************************************************************************/
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

/******************************************************************************/
#include <QStringList>
#include "GeoImageList"

ImageListEditor::ImageListEditor(QWidget *parent, Attribute* attribute)
  : QComboBox(parent)
{
  const QStringList& options=geoImageList_.list(attribute->typeOfImage());
  addItems(options);
}

void ImageListEditor::setValue(const QString& value)
{
  setCurrentIndex(findText(value));
}

QString ImageListEditor::value()
{
  return currentText();
}

QSize ImageListEditor::sizeHint() const
{
  qDebug("ImageListEditor::sizeHint()");
  QSize size=QComboBox::sizeHint();
  size.setHeight(size.height()*1.3);
  return size;
}

/******************************************************************************/
#include <QStringList>

EnumEditor::EnumEditor(QWidget *parent, Attribute* attribute)
  : QComboBox(parent)
{
  const QStringList& options=attribute->options();
  addItems(options);
}

void EnumEditor::setValue(const QString& value)
{
  setCurrentIndex(findText(value));
}

QString EnumEditor::value()
{
  return currentText();
}

QSize EnumEditor::sizeHint() const
{
  qDebug("EnumEditor::sizeHint()");
  QSize size=QComboBox::sizeHint();
  size.setHeight(size.height()*1.3);
  return size;
}

/******************************************************************************/
#include <QStringList>
#include <QCheckBox>

BoolEditor::BoolEditor(QWidget *parent, Attribute* attribute)
  : QCheckBox(parent)
{
}

void BoolEditor::setValue(const QString& value)
{
  setChecked(value=="true");
}

QString BoolEditor::value()
{
  return (isChecked() ? "true" : "false");
}

QSize BoolEditor::sizeHint() const
{
  qDebug("BoolEditor::sizeHint()");
  QSize size=QCheckBox::sizeHint();
  size.setHeight(size.height()*1.3);
  return size;
}


/******************************************************************************/
FormulaEditor::FormulaEditor(QWidget *parent, Attribute* attribute)
  : QTextEdit(parent)
{
}

void FormulaEditor::setValue(const QString& value)
{
  setPlainText(value);
}

QString FormulaEditor::value()
{
  return toPlainText();
}

QSize FormulaEditor::sizeHint() const
{
  qDebug("FormulaEditor::sizeHint()");
  QSize size=QTextEdit::sizeHint();
  size.setHeight(size.height()*1.3);
  return size;
}









