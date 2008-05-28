/***************************************************************************
                          cleanup.cpp  -  description
                             -------------------
    begin                : Tue Jul 24 2001
    copyright            : (C) 2001 by Martin pahl
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

/*
 * $Source: /data/cvs/gda/gda/core/cleanup.cpp,v $
 * $Revision: 1.9 $
 * $Date: 2003/06/04 11:49:25 $
 * $Author: pahl $
 * $Locker:  $
 */

#include "cleanup.h"
#include <qfile.h>
#include <qdir.h>
#include <unistd.h>
#include <qstringlist.h>
#include <stdlib.h> //für getenv
#include <stdlib.h>

CleanUp::CleanUp()
{
//  setAutoDelete(FALSE);
  prefix_="";
}

CleanUp::~CleanUp()
{
}

/** Append filename to the list of temporary files */
void CleanUp::append(QString filename)
{
  if (filename.isEmpty())
    return;
  QStringList::append(filename);
}

/** Delete the stored files */
void CleanUp::deleteFiles()
{
  qDebug("Cleanup::deleteFiles");
  const_iterator it;
  for (it = constBegin(); it != constEnd(); ++it) {
    qDebug("     rm %s", (*it).toLatin1().constData());
    QFile::remove(*it);
  }
  clear();
  QString cmd;
  cmd.sprintf("rm -rf %s",getTmpDirPID().toLatin1().constData());
  qDebug(cmd.toLatin1().constData());
  system(cmd.toLatin1().constData());

}

QString CleanUp::getTmpDir()
{
  static QString dir;
  if (dir.isEmpty()) {
    dir = getenv("GA_DATADIR");
      if (dir.isEmpty()) {
      dir=getenv("HOME");
      if (!dir
      .isEmpty()) {
        dir+="/.gda";
        QDir tmpdir(dir);
        if (!tmpdir.exists()) {
	  qDebug("Creating tmpdir %s", dir.toLatin1().constData());
	  tmpdir.mkdir(dir);
        }
        dir+="/tmp";
      }
      else {
        if (prefix_=="")
#ifdef WIN32
          dir = getenv("TEMP");
#else
          dir = TMPDIR;
#endif
        else
          dir=prefix_+"/tmp";
      }
    }
	  QDir tmpdir(dir);
	  if (!tmpdir.exists()) {
	    qDebug("Creating tmpdir %s", dir.toLatin1().constData());
	    tmpdir.mkdir(dir);
	  }
  }

  return dir;
}

QString CleanUp::getTmpDirPID()
{
  static QString dir;
  if (dir.isEmpty()) {
    dir = getTmpDir();
#ifdef WIN32
    dir = QString().sprintf("%s\\%d", dir.toLatin1().constData(), getpid());
#else
    dir = QString().sprintf("%s/%d", dir.toLatin1().constData(), getpid());
#endif
  }

  QDir tmpdir(dir);
  if (!tmpdir.exists()) {
    qDebug("Creating tmpdir %s", dir.toLatin1().constData());
    tmpdir.mkdir(dir);
  }
  return dir;
}

/** creates the relDir in absDir and returns the new absDir */
QString CleanUp::mkdir(QString absDir, QString relDir)
{
  QDir dir(absDir);
  if (!dir.exists()) return "";
#ifdef WIN32
  QStringList dirElements=relDir.split("\\");
#else
  QStringList dirElements=relDir.split("/");
#endif
  for ( QStringList::Iterator it = dirElements.begin(); it != dirElements.end(); ++it ) {
    if (!dir.exists(*it)) {
      if (!dir.mkdir(*it)) return "";
    }
    if (!dir.cd(*it)) return "";
  }
  return dir.path();

}

QString& CleanUp::prefix(QString prefix)
{
  if (prefix!="")
    prefix_=prefix;
  return prefix_;
}

QString CleanUp::prefix_;
CleanUp cleanUp_;
