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

class Exception
{
 public:
  virtual QString what() const;
};

class GeneralException : public Exception
{
 public:
  GeneralException(QString errorMessage);
  QString what() const;
 private:
  QString message_;
};

class FileIOException : public Exception
{
 public:
  enum ExceptionType { FILE_NOT_EXISTS, OPEN_FAILED };
  FileIOException(ExceptionType type, QString filename);
  QString what() const;
 private:
  ExceptionType type_;
  QString filename_;
};

class ImageException : public Exception
{
 public:
  enum ExceptionType {
    Dimension
  };
  ImageException(ExceptionType type, int x1, int x2, int dx, int y1, int y2, int dy);
  QString what() const;
 private:
  ExceptionType type_;
  int x1_;
  int x2_;
  int dx_;
  int y1_;
  int y2_;
  int dy_;
};

class NodeException : public Exception
{
 public:
  enum ExceptionType { ALLOC_FAILED };
  NodeException(ExceptionType type);
  QString what() const;
 private:
  ExceptionType type_;
};

#endif
