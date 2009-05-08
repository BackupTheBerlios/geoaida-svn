/***************************************************************************
                          stackelemstack.cpp  -  description
                             -------------------
    begin                : Tue Nov 13 2001
    copyright            : (C) 2001 by Martin Pahl
    email                : bueckner@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "StackElement"

using BottomUp;

StackElementStack::StackElementStack(const Stack& stack){
  //stack_=stack;
  QPtrStackIterator<StackElement> it(stack);
  for (;it.current();++it) {
    stack_.push((*it)->copy());
  }
}

StackElementStack::StackElementStack(const StackElementStack& elem){
  //stack_=elem.stack_;
  QPtrStackIterator<StackElement> it(elem.stack_);
  for (;it.current();++it) {
    stack_.push((*it)->copy());
  }
}

StackElementStack::~StackElementStack(){
}

Stack& StackElementStack::data(){
  return stack_;
}

int StackElementStack::type(){
  return STACK;
}

const char* StackElementStack::typeName()
{
  return "stack";
}

StackElement* StackElementStack::copy() {
  Stack val=((StackElementStack*)this)->data(); //liste holen
  return (new StackElementStack(val));
}
