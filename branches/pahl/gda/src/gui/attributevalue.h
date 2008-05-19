/***************************************************************************
                          attributevalue.h  -  description
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

#ifndef ATTRIBUTEVALUE_H
#define ATTRIBUTEVALUE_H

#include <QString>
#include <QVariant>

class Attribute;

class AttributeValue {
public:
  AttributeValue();
  ~AttributeValue();
  AttributeValue(const AttributeValue& attribValue);
  AttributeValue(Attribute* attribute, const QString& value);
  Attribute* attribute();
  const QString& value();
private:
  Attribute* attribute_;
  QString value_;
};

Q_DECLARE_METATYPE(AttributeValue)

#endif
