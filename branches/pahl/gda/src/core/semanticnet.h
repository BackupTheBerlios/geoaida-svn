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

#include <QTextStream>
#include <QAbstractItemModel>
#include <QHash>
class SNode;

/*!
 * \brief Class for Semantic Net of type SNode
 *
 */

class SemanticNet : public QAbstractItemModel
{
public:
  SemanticNet();
  ~SemanticNet();

  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex parent(const QModelIndex &child) const;
  virtual QVariant headerData ( int section, 
				Qt::Orientation orientation, 
				int role = Qt::DisplayRole ) const;
 private:
  SNode* nodeFromIndex(const QModelIndex& index) const;

 public:
  void setFilename(QString filename);
  QString filename();
  void read();
  void read(const QString & fname);
  void read(QIODevice & fp);
  SNode *rootNode(void);
  void write();
  void write(QTextStream & fp);
  void write(const QString & fname);
 protected:     
  /** root node of the semantic net */
  SNode * rootNode_;
  mutable QHash<uint,QPixmap> pixmapHash_;
 private:
  QString filename_;
};
#endif
