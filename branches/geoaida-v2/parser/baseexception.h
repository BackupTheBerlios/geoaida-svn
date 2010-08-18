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
#ifndef BASEEXCEPTION_H
#define BASEEXCEPTION_H

#include <QString>

class BaseException
{
 public:
  BaseException(const char* sourcefile, int sourceline);
  virtual QString what() const;
  virtual QString debug() const;
  virtual const char* classname() const { return "BaseException"; }
 private:
  const char* sourcefile_;
  int sourceline_;
};

class GeneralException : public BaseException
{
 public:
  GeneralException(QString errorMessage, const char* sourcefile=0, int sourceline=0);
  virtual QString what() const;
  const char* classname() const { return "GeneralException"; }
 private:
  QString message_;
};


#endif
