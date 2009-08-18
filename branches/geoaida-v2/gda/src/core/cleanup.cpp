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

bool RemoveDirectory(QDir aDir)
{
	bool has_err = false;
	if (aDir.exists())//QDir::NoDotAndDotDot
	{
	
		QFileInfoList entries = aDir.entryInfoList(QDir::NoDotAndDotDot |
		QDir::Dirs | QDir::Files);
		int count = entries.size();
	
		//RBCFS_OK == has_err
		for (int idx = 0; ((idx < count) && (!has_err)); idx++)
		{
			QFileInfo entryInfo = entries[idx];
			QString path = entryInfo.absoluteFilePath();
			if (entryInfo.isDir())
			{
			has_err = RemoveDirectory(QDir(path));
			}
			else
			{
				QFile file(path);
				if (!file.remove())
					has_err = true;
			}
		}
	
		if (!aDir.rmdir(aDir.absolutePath()))
			has_err = true;
	}
	
	return(has_err);
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

/*
#ifdef DEBUG_MSG 	
  qDebug("Cleanup::deleteFiles");
#endif
  Iterator it;
  for (it = this->begin(); it != this->end(); ++it) {
#ifdef DEBUG_MSG
#ifdef WIN32
	  qDebug("     del /f %s", (const char *) it.key());
#else
	  qDebug("     rm %s", (const char *) it.key());
#endif
#endif
    QFile::remove(it.key());
  }
  clear();
  QString cmd;
#ifdef WIN32
  	  cmd.sprintf("rd /s/q %s",getTmpDirPID().latin1());
#else
	  cmd.sprintf("rm -rf %s",getTmpDirPID().latin1());
#endif  
  
#ifdef DEBUG_MSG   
  qDebug(cmd);
#endif
  system(cmd.latin1());
*/

  QDir tempDir = QDir(getTmpDirPID());

  RemoveDirectory(tempDir);
  
}

/** Delete all temp files */
void CleanUp::deleteAllFiles()
{

  QDir aDir = QDir(getTmpDir());

  bool has_err = false;
	if (aDir.exists())//QDir::NoDotAndDotDot
	{
	
		QFileInfoList entries = aDir.entryInfoList(QDir::NoDotAndDotDot |
		QDir::Dirs | QDir::Files);
		int count = entries.size();
	
		//RBCFS_OK == has_err
		for (int idx = 0; ((idx < count) && (!has_err)); idx++)
		{
			QFileInfo entryInfo = entries[idx];
			QString path = entryInfo.absoluteFilePath();
			if (entryInfo.isDir())
			{
			has_err = RemoveDirectory(QDir(path));
			}
			else
			{
				QFile file(path);
				if (!file.remove())
					has_err = true;
			}
		}
			
	}

}

QString CleanUp::getTmpDir()
{
  static QString dir;
  if (dir.isEmpty()) {
    QDir tmpdir;
    dir = getenv("GA_DATADIR");
    if (dir.isEmpty()) 
      tmpdir.setPath(QDir::homePath()+"/.gda/tmp");
    else tmpdir.setPath(dir);
    if (!tmpdir.exists()) {
      qDebug("Creating tmpdir %s", tmpdir.path().toLatin1().constData());
      tmpdir.mkpath(tmpdir.path());
    }
    dir=tmpdir.path();
  }

  return dir;
}

QString CleanUp::getTmpDirPID()
{
  static QString dir;
  if (dir.isEmpty()) {
    dir = getTmpDir();
    
    dir = QString().sprintf("%s/%d", dir.toLatin1().constData(), getpid());
  }

  QDir tmpdir(dir);
  if (!tmpdir.exists()) {
    qDebug("Creating tmpdir %s", tmpdir.path().toLatin1().constData());
    tmpdir.mkdir(tmpdir.path());
  }
  return dir;
}

unsigned int CleanUp::getPID()
{
	return getpid();
}

/** creates the relDir in absDir and returns the new absDir */
QString CleanUp::mkdir(QString absDir, QString relDir)
{
  QDir dir(absDir);
  if (!dir.exists()) return "";
  dir.mkpath(relDir); //! throw exception
  dir.setPath(relDir);  //! throw exception
  return dir.path();

}

void CleanUp::setPrefix(QString prefix)
{
   prefix_=prefix;
}

QString& CleanUp::prefix(QString prefix)
{
  if (prefix!="")
    prefix_=prefix;
  return prefix_;
}

QString CleanUp::prefix_;
CleanUp cleanUp_;
