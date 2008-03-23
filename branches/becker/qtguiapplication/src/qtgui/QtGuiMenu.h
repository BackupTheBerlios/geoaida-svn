// QtGui - Library 
// Copyright (C) 1999 Martin Pahl, Universit�t Hannover
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

/*
 * $Source: /data/cvs/qtguiapplication/qtgui/qtgui/QtGuiMenu.h,v $
 * $Revision: 1.1 $
 * $Date: 2001/07/31 11:57:04 $
 * $Author: pahl $
 * $Locker:  $
 */

#ifndef _QtGuiMenu_h
#define _QtGuiMenu_h
#include "QtGui.h"

class QtGuiMenuPrivate;

class QtGuiMenu : public QtGui {
 public:
  QtGuiMenu(MLParser *parser=0, ArgDict* argdict=0);
  ~QtGuiMenu();
  virtual QWidget *create(QWidget *parent=0, QLayout *layout=0);
  virtual void connectObject();
//  virtual void show();
//  virtual void hide();
//  virtual const QString &name();
 private:  
  QtGuiMenuPrivate* data_;
};

QtGuiMenu *newQtGuiMenu(MLParser *parser, ArgDict* argdict=0);
#endif