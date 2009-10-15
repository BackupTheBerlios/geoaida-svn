/***************************************************************************
                          instancenet.cpp  -  description
                             -------------------
    begin                : 2008-04-10
    copyright            : (C) 2008 by Martin Pahl
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

#include "instancenet.h"
#include "INode"

/** Read a  net  */
void InstanceNet::readfile(MLParser& parser)
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
    rootNode_ = new INode(parser);
    Q_ASSERT(rootNode_!=0);
    qDebug("NetModel: Root=%s\n", rootNode_->name().toLatin1().constData());
  }
}

