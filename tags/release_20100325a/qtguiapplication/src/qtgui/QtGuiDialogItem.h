// QtGui - Library 
// Copyright (C) 1999 Martin Pahl, Universitšt Hannover
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
 * $Source: /data/cvs/qtguiapplication/qtgui/qtgui/QtGuiDialogItem.h,v $
 * $Revision: 1.1 $
 * $Date: 2001/07/31 11:57:04 $
 * $Author: pahl $
 * $Locker:  $
 */

#ifndef _QtGuiDialogItem_h
#define _QtGuiDialogItem_h
#include "QtGui.h"

class QtGuiDialogItemPrivate;

class QtGuiDialogItem : public QtGui {
  Q_OBJECT
 public:
//  QtGuiDialogItem(MLParser *parser=0);
//  ~QtGuiDialogItem();
//  virtual QWidget *create(QWidget *parent=0, QLayout *layout=0);
//  virtual void open();
//  virtual void close();
//  virtual QString &name();
//  virtual void valueSetInt(int);
  virtual void variableSet();
  virtual void variableGet();
 public slots:
  virtual void enable(int);
 protected:  
  QtGuiDialogItemPrivate* idata_;
};

#endif
