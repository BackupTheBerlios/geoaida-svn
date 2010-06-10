/***************************************************************************
                          exception.h  -  description
                             -------------------
    begin                : Wed Apr 09 2008
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
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>
#include "BaseException"

class FileIOException : public BaseException
{
 public:
  enum ExceptionType { FILE_NOT_EXISTS, OPEN_FAILED };
  FileIOException(ExceptionType type, QString filename, 
		  const char* sourcefile=0, int sourceline=0);
  QString what() const;
  const char* classname() const { return "FileIOException"; }
 private:
  ExceptionType type_;
  QString filename_;
};

class ImageException : public BaseException
{
 public:
  enum ExceptionType { Dimension, UnknownType };
  ImageException(ExceptionType type, int x1, int x2, int dx, int y1, int y2, int dy, 
		 const char* sourcefile=0, int sourceline=0);
  ImageException(ExceptionType type, QString filename,
		 const char* sourcefile=0, int sourceline=0);
  QString what() const;
  const char* classname() const { return "ImageException"; }
 private:
  ExceptionType type_;
  int x1_;
  int x2_;
  int dx_;
  int y1_;
  int y2_;
  int dy_;
  QString filename_;
};

class NodeException : public BaseException
{
 public:
  enum ExceptionType { ALLOC_FAILED, NOT_FOUND };
  NodeException(ExceptionType type, 
		const char* sourcefile=0, int sourceline=0);
  QString what() const;
  const char* classname() const { return "NodeException"; }
 private:
  ExceptionType type_;
};

class ProcessException : public BaseException
{
 public:
  ProcessException(int error, QString cmd,
		   const char* sourcefile=0, int sourceline=0);
  QString what() const;
  const char* classname() const { return "NodeException"; }
 private:
  int error_;
  QString cmd_;
};

#endif
