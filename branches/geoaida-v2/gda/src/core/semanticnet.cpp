/***************************************************************************
                          semanticnet.cpp  -  description
                             -------------------
    begin                : Tue Feb 12 2008
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

#include "semanticnet.h"
#include "SNode"
#include "NodeException"

/** Read a  net  */
void SemanticNet::readfile(MLParser& parser)
{
  if (rootNode_) {
    delete rootNode_;
    rootNode_=0;
  }
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
  } while ((tag != TOK_NODE) && (tag != MLParser::END_OF_FILE));
  if (tag == TOK_NODE) {
    rootNode_ = new SNode(parser);
    Q_ASSERT(rootNode_!=0);
    qDebug("NetModel: Root=%s\n", rootNode_->name().toLatin1().constData());
  }
  else throw NodeException(NodeException::NOT_FOUND, 
			   __FILE__":SemanticNet::readFile", __LINE__);
}

