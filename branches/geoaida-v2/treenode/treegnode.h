/***************************************************************************
                          treenode.h  -  description
                             -------------------
    begin                : Thu Sep 28 2000
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

#ifndef TREEGNODE_H
#define TREEGNODE_H

#include <QList>
#include <QString>

/**TreeGNode for trees
  *@author Martin Pahl
  */

class TreeGNode {
 public:
  TreeGNode();
  virtual ~TreeGNode() = 0;
  TreeGNode* parent();
  QList<TreeGNode*>& children();
  void parent(TreeGNode*);
  void childUnlink(TreeGNode *node);
  void childLink(TreeGNode *node);
  virtual const QString& name() const = 0;
 protected: // Protected attributes
  QList<TreeGNode*> children_;
  TreeGNode* parent_;
};

#include "treegnode.hpp"

#endif
