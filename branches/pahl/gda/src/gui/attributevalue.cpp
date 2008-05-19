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

#include "attributevalue.h"

AttributeValue::AttributeValue()
  : attribute_(0), value_("")
{
}

AttributeValue::~AttributeValue()
{
}

AttributeValue::AttributeValue(const AttributeValue& attribValue)
  :   attribute_(attribValue.attribute_), value_(attribValue.value_)

{
}


AttributeValue::AttributeValue(Attribute* attribute, const QString& value)
  : attribute_(attribute), value_(value)
{
}

Attribute* AttributeValue::attribute()
{
  return attribute_;
}
  
const QString& AttributeValue::value()
{
  return value_;
}

