/***************************************************************************
                          stackelemstring.cpp  -  description
                             -------------------
    begin                : Wed Aug 1 2001
    copyright            : (C) 2001 by Martin Pahl
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

#include "stackelemstring.h"

using BottomUp;

StackElementString::StackElementString(QString s)
{
  data_=s;
}

StackElementString::StackElementString(const StackElementString& elem)
{
  data_=elem.data_;
}

StackElementString::~StackElementString()
{
}

/** returns the type (STRING) of this stack element  */
int StackElementString::type()
{
  return STRING;
}

const char* StackElementString::typeName()
{
  return "string";
}

/** Returns a copy of this stack element */
StackElement* StackElementString::copy()
{
  QString val=((StackElementString*)this)->data(); //liste holen
  return (new StackElementString(val));
}

/** Returns the contained string from this stack element */
QString StackElementString::data()
{
  return data_;
}
