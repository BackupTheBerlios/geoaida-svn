/***************************************************************************
                          netmodel.cpp  -  description
                             -------------------
    begin                : Tue Feb 12 2008
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

#include "netmodel.h"
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include "GNode"
#include "SNode"
#include "INode"
#include "NodeException"

/*!
 * \brief constructor
 */
NetModel::NetModel()
   : rootNode_(0)
{
#if 0
  rootNode_ = new GNode();
#ifdef WIN32
        if (rootNode_ == 0){
          //cout << "Out of Memory..14";
          exit(1);
        }
#endif
  rootNode_->name("Scene");
//  rootNode_->xxxmin(1);
//  rootNode_->xxxmax(1);
#endif
}

/*!
 * \brief destructor
 */
NetModel::~NetModel()
{
  if (rootNode_)
    delete rootNode_;
}

/*!
 * \name Model interface for read-only access 
*/ 
//@{

/*!
 * \brief return the number of columns for the given object
 *
 * In the Net-model columns represent the attributes of one node.
 *
 * \param parent - the node for which to return number of columns.
 * \return number of columns
 */
int NetModel::columnCount ( const QModelIndex & parent ) const
{
  return 1;
}

/*!
 * \brief return the number of rows for the given object
 *
 * In the Net-model rows represent the children of one node.
 *
 * \param parent - the node for which to return number of rows.
 * \return number of rows
 */
int NetModel::rowCount ( const QModelIndex & parent ) const
{
  if (!parent.isValid()) return 1;
  GNode *parentNode = nodeFromIndex(parent);
  if (!parentNode)
    return 0;
  return parentNode->children().count();
}

/*!
 * \brief return data from the given object
 *
 * \param index - the node for which to return some data.
 * \param role - the role of the data which should be returned
 * \return number of rows
 */
QVariant NetModel::data ( const QModelIndex & index, int role ) const
{
  if (role != Qt::DisplayRole && role != Qt::DecorationRole )
    return QVariant();
  GNode *node = nodeFromIndex(index);
  if (!node)
    return QVariant();
  if (index.column() == 0) {
    switch (role) {
    case Qt::DisplayRole:
      return node->name();
    case Qt::DecorationRole:
      if (node->isA("SNode")) {
	uint color=static_cast<SNode*>(node)->color();
	if (pixmapHash_.contains(color)) {
	  return QIcon(pixmapHash_.value(color));
	}
	else {
	  QPixmap p(20,10);
	  p.fill(color);
	  pixmapHash_.insert(color,p);
	  return QIcon(p);
	}
	return QVariant();
      }
      else return QVariant();
      //!    case NodeRole:
      //      return QVariant::fromValue(node);
    }
  }
  return QVariant();
}


/*!
 * \brief return an index pointing to the requested object
 *
 * \param row - the position of the children to be returned
 * \param column - the role of the data which should be returned
 * \return number of rows
 */
QModelIndex NetModel::index (int row, int column, 
			   const QModelIndex & parent ) const
{
    if (!rootNode_)
        return QModelIndex();
    if (parent.isValid()) {
      GNode *parentNode = nodeFromIndex(parent);
      return createIndex(row, column, parentNode->children()[row]);
    }
    else
      return createIndex(row, column, rootNode_);
      
}

/*!
 * \brief return an index pointing to parent of the given index
 *
 * \param child - index of child
 * \return index of parent
 */
QModelIndex NetModel::parent(const QModelIndex &child) const
 {
    GNode *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();
    GNode *parentNode = node->parent();
    if (!parentNode)
        return QModelIndex();
    GNode *grandparentNode = parentNode->parent();
    if (!grandparentNode)
      return createIndex(0,0,parentNode);
    int row = grandparentNode->children().indexOf(parentNode);
    return createIndex(row, child.column(), parentNode);
}

QVariant NetModel::headerData ( int section, 
			      Qt::Orientation orientation, 
			      int role ) const 
{
  if (role!=Qt::DisplayRole) return QVariant();
  return QString(tr("Node"));
}

/*!
 * \brief convert internal pointer of a QModelIndex to GNode
 *
 * \param index
 * \return pointer to GNode
 */
GNode* NetModel::nodeFromIndex(const QModelIndex& index) const
{
  if (index.isValid()) {
    return static_cast<GNode *>(index.internalPointer());
  } else {
    return rootNode_;
  }
}


//@}


/*!
 * \name Net-specific interface 
 */
//@{

/** Set the filename */
void NetModel::setFilename(QString filename)
{
  filename_=filename;
}

/** Get the filename */
QString NetModel::filename()
{
  return filename_;
}

/** Read a  net. The filename must be set before */
void NetModel::read()
{
  read(filename_);
}

/** Read a  net by using a filename */
void NetModel::read(const QString & fname)
{
  QFile fp(fname);
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("NetModel::read(%s): file not found", fname.toLatin1().constData());
    throw FileIOException(FileIOException::FILE_NOT_EXISTS,fname);
  }
  read(fp);
  fp.close();
  setFilename(fname);
}

/** Read a  net  */
void NetModel::read(QIODevice & fp)
{
  MLParser parser(&fp);
  readfile(parser);
}

/** Read a  net  */
void NetModel::readfile(MLParser& parser)
{
  if (rootNode_) {
    delete rootNode_;
    rootNode_=0;
  }
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
  } while ((tag != TOK_NODE) && (tag != MLParser::END_OF_FILE));
  if (tag == TOK_NODE)
    rootNode_ = new GNode(parser);
  if (rootNode_)
    qDebug("NetModel: Root=%s\n", rootNode_->name().toLatin1().constData());
  else
    throw NodeException(NodeException::ALLOC_FAILED);
}

/** Write a  net. The filename must be set before */
void NetModel::write()
{
  write(filename_);
}

/** Write the  net to the file fname */
//void NetModel::write(const QString & fname)
//{
//  QFile fp(fname);
//  if (!fp.open(QIODevice::WriteOnly)) {
//    qDebug("NetModel::write(%s): file not accessable", fname.toLatin1().constData());
//    throw FileIOException(FileIOException::OPEN_FAILED, fname);
//  }
//  QTextStream str(&fp);
//  write(str);
//  fp.close();
//  setFilename(fname);
//}

//Method changed by Rodrigo - 04/06/09

/** write the  net to the file fp */
//void NetModel::write(QTextStream & fp)
//{
//  if (rootNode_)
//    rootNode_->write(fp, QString(""));
//}

/** write the  net to the file fp */
void NetModel::write(const QString & fname)
{
  QFile fp(fname);
  if (!fp.open(QIODevice::WriteOnly)) {
    qDebug("NetModel::write(%s): file not accessable", fname.toLatin1().constData());
    throw FileIOException(FileIOException::OPEN_FAILED, fname);
  }
  
  if (!rootNode_) return;
  
  QXmlStreamWriter writer(&fp);
  
  writer.setAutoFormatting(true);
  writer.setCodec("UTF-8");
  writer.writeStartDocument();
  writer.writeStartElement("node");
  
  ArgDictConstIterator it=rootNode_->attribList().constBegin();
  for (;it!=rootNode_->attribList().constEnd(); ++it) {
    writer.writeAttribute(it.key(), it.value());
  }
  
  //Writes child elements recursively
  
  QList<GNode*>::const_iterator ait = rootNode_->children().constBegin();
  for (; ait!=rootNode_->children().constEnd(); ++ait) {
    writeChild(*ait,writer);
  }
  
  writer.writeEndElement();
  writer.writeEndDocument();
  fp.close();
  
  setFilename(fname);
}

/** write child element to the net file */

void NetModel::writeChild(GNode* node, QXmlStreamWriter & writer)
{

  writer.writeStartElement("node");
  
  ArgDictConstIterator it=node->attribList().constBegin();
  for (;it!=node->attribList().constEnd(); ++it) {
    
    writer.writeAttribute(it.key(), it.value());
    
  }
  
  //Writes child elements recursively
  
  QList<GNode*>::const_iterator ait = node->children().constBegin();
  for (; ait!=node->children().constEnd(); ++ait) {
    writeChild(*ait,writer);
  }
  
  writer.writeEndElement();
  
}

/** Get the rootNode_ of the  net */
GNode *NetModel::rootNode(void)
{
  return rootNode_;
}

/** Set the rootNode_ of the  net */
void NetModel::setRootNode(GNode *node)
{
  if (rootNode_) delete rootNode_;
  rootNode_=node;
}

//@}
