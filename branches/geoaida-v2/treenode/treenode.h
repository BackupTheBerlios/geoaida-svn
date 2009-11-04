/***************************************************************************
                          tree.h  -  description
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

#ifndef TREENODE_H
#define TREENODE_H

#include <QList>

/**Template class for a tree
  *@author Martin Pahl
  */

template <class T, class Base>
class TreeNode : public Base
{
public: 
  TreeNode();
  virtual ~TreeNode();
  T* parent();
  QList<T*>& children();
  QList<T*>& child(int row);
};

#include "treenode.hpp"
#endif
