/***************************************************************************
                          file.cpp  -  description
                             -------------------
    begin                : Thu Oct 19 2000
    copyright            : (C) 2000 by Jürgen Bückner
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



#include "filelist.h"
#include "qfile.h"

// #define DEBUGMSG

/** default constructor */
FileList::FileList()
{
}

/** constructor resd from the provided filename*/
FileList::FileList(QString filename)
{
  read(filename);
}

/** denstructor */
FileList::~FileList()
{
  clear();
}

void
FileList::clear()
{
  qDeleteAll(*this);
  QHash< QString, ArgDict* >::clear();
}

/** read a list of File and the attributes from the provided filename */
void
FileList::read(QString filename)
{
  QFile fp(filename);
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("FileList::read(%s): file not founed\n",
           filename.toLatin1().constData());
    return;
  }
  read(fp);
  fp.close();
}

/** read a list of File and the attributes from the provided filepointer */
void
FileList::read(QIODevice & fp)
{
  MLParser parser(&fp);
  read(parser);
}

/** read a list of File and the attributes through parser */
void
FileList::read(MLParser & parser)
{
  qDebug("FileList::read(parser)");
  list_.clear();
  QString keywords[] = { "file", "filelist", "" };
  const MLTagTable nodeTagTable(keywords);
  const int TOK_FILE = 1;
  const int TOK_FILE_LIST = 2;
  int tag;
  do {
    tag = parser.tag(nodeTagTable);
    ArgDict *args = parser.args();
    switch (tag) {
    case TOK_FILE:
      insert(args->value("key"), args);   // insert read geoimage
      list_ += args->value("key");        // fill additional list of file names
      break;
    default:
      delete args;
      break;
    }
  } while ((tag != MLParser::END_OF_FILE) && (tag != -TOK_FILE_LIST));
}


/** return the filename corresponding to the given key */
QString 
FileList::filename(QString fileKey)
{
  ArgDict* args=value(fileKey);
  if (args) {
    return args->value("filename");
  }
  return "";
}

/** return Info about the file*/
QString
FileList::info(QString fileKey)
{
  ArgDict* args=value(fileKey);
  if (args) {
    return args->value("info");
  }
  return "";
}

/** return a list of included files*/
QStringList FileList::list(QString type)
{
  if (type.isEmpty())
    return list_;
  QStringList list;
  QStringList::ConstIterator it;
  for (it = list_.begin(); it != list_.end(); ++it) {
    ArgDict *args = value(*it);
    QString filetype=args->value("type");
    if (QString::compare(filetype, type, Qt::CaseInsensitive) == 0)
      list += (*it);
    qDebug("FileList::list(item=%s)", it->toLatin1().constData());
  }
  return list;
}

FileList fileList_;


