/***************************************************************************
                          tree.cpp  -  description
                             -------------------
    begin                : Wed Sep 27 2000
    copyright            : (C) 2000 by Martin Pahl
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


template <class T, class Base>
TreeNode<T, Base>::TreeNode(){
}

template <class T, class Base>
TreeNode<T, Base>::~TreeNode(){
}

/** Get the parent of this node */
template <class T, class Base>
T* TreeNode<T, Base>::parent()
{
  return (T*)Base::parent_;
}


/** Get the List of children */
template <class T, class Base>
QList<T*>& TreeNode<T, Base>::children()
{
  return (QList<T*>&)Base::children_;
}

/** Get the child in the given row */
template <class T, class Base>
QList<T*>& TreeNode<T, Base>::child(int row)
{
  return (QList<T*>&)Base::children_.value(row);
}

