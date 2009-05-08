/*!*************************************************************************
 * \file semanticnet.h 
 * \brief Prototype for class "SemanticNet"
 *
 * \date
 *  begin                : 2008-02-12
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

#ifndef SEMANTICNET_H
#define SEMANTICNET_H

#include "NetModel"
#include "SNode"

/*!
 * \brief Class for Semantic Net of type SNode
 *
 */
class SemanticNet : public NetModel
{
public:
  SNode *rootNode() {
    return static_cast<SNode*>(rootNode_);
  }
  void readfile(MLParser& parser);
};

#endif
