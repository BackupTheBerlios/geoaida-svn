/*!*************************************************************************
 * \file semanticnet.h 
 * \brief Prototype for class "SemanticNet"
 *
 * \date
 *  begin                : 2008-04-09
 * \author 
 *  copyright            : (C) 2008 by Martin Pahl
 *  email                : pahl@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INSTANCENET_H
#define INSTANCENET_H

#include "NetModel"
#include "INode"

/*!
 * \brief Class for Instance Net of type INode
 *
 */
class InstanceNet : public NetModel
{
 public:
  INode *rootNode() {
    return static_cast<INode*>(rootNode_);
  }
  void readfile(MLParser& parser);
};

#endif
