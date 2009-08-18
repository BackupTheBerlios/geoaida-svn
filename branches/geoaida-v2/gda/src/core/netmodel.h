/*!*************************************************************************
 * \file netmodel.h 
 * \brief Prototype for class "NetModel"
 *
 * \date
 *  begin                : Tue Feb 12 2008
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


#ifndef NETMODEL_H
#define NETMODEL_H

#include <QTextStream>
#include <QAbstractItemModel>
#include <QHash>
#include <QXmlStreamWriter>
#include "MLParser"

class GNode;

/*!
 * \brief Class for  NetModel of type GNode
 *
 */

class NetModel : public QAbstractItemModel
{
public:
  NetModel();
  ~NetModel();

  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex &child) const;
  virtual QVariant headerData ( int section, 
				Qt::Orientation orientation, 
				int role = Qt::DisplayRole ) const;
 private:
  GNode* nodeFromIndex(const QModelIndex& index) const;

 public:
  void setFilename(QString filename);
  QString filename();
  void read();
  void read(const QString & fname);
  void read(QIODevice& fp);
  virtual void readfile(MLParser& parser);
  GNode *rootNode(void);
  void write();
  void write(QTextStream & fp);
  void write(const QString & fname);
  
  void writeChild(GNode* node, QXmlStreamWriter & writer);
  
 protected:     
  /** root node of the net */
  GNode * rootNode_;
  mutable QHash<uint,QPixmap> pixmapHash_;
 private:
  QString filename_;
};
#endif
