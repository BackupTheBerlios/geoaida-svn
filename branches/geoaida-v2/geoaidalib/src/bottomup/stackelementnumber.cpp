/***************************************************************************
                          stackelemnumber.cpp  -  description
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

#include "stackelementnumber.h"

using BottomUp;
StackElementNumber::StackElementNumber(double v) {
  data_=v;
}

StackElementNumber::StackElementNumber(const StackElementNumber& elem) {
  data_=elem.data_;
}

StackElementNumber::~StackElementNumber() {
}

/** returns the type (NUMBER) of this stack element  */
int StackElementNumber::type() {
  return NUMBER;
}

const char* StackElementNumber::typeName()
{
  return "number";
}

/** Returns a copy of this stack element */
StackElement* StackElementNumber::copy() {
  double val=((StackElementNumber*)this)->data(); //liste holen
  return (new StackElementNumber(val));
}

/** Returns the contained numeric value from this stack element */
double StackElementNumber::data() {
  return data_;
}
