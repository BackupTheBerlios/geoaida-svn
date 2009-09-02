/***************************************************************************
                          stackelemnodelist.cpp  -  description
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

#include "stackelementnodelist.h"

using namespace BottomUpLib;

/** constructs a stack element containing the given nodelist */
StackElementNodeList::StackElementNodeList(const NodeList& list)
{
  data_=list;
}

StackElementNodeList::StackElementNodeList(const StackElementNodeList& elem)
{
 data_=elem.data_;
}


StackElementNodeList::~StackElementNodeList()
{
}

/** returns the type (NODELIST) of this stack element  */
int StackElementNodeList::type()
{
  return NODELIST;
}

const char* StackElementNodeList::typeName()
{
  return "nodelist";
}

/** Returns a copy of this stack element */
StackElement* StackElementNodeList::copy()
{
  NodeList nl=((StackElementNodeList*)this)->data(); //liste holen
  return (new StackElementNodeList(nl));
}

/** return the data in the stack element */
NodeList& StackElementNodeList::data()
{
  return data_;
}
