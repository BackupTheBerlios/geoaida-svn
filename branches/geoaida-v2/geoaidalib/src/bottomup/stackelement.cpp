/***************************************************************************
                          stackelem.cpp  -  description
                             -------------------
    begin                : Tue Jul 31 2001
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

#include "stackelement.h"

using namespace BottomUpLib;

StackElement::StackElement(){
}

StackElement::~StackElement(){
}

/** Type of stack element */
int StackElement::type()
{
  return UNKNOWN;
}
