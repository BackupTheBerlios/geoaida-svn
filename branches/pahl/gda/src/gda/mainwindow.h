/***************************************************************************
                          maingui.h  -  description
                             -------------------
    begin                : Tue Sep 12 2000
    copyright            : (C) 2000 by Martin Pahl
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
 * $Source: /data/cvs/gda/gda/maingui.h,v $
 * $Revision: 1.28 $
 * $Date: 2005/10/14 07:38:31 $
 * $Author: mueller $
 * $Locker:  $
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


/**Contains the main class
  *@author Martin Pahl
  */

#include <ui_mainwindow.h>
#include <QMainWindow>
#include "SemanticNet"
#include "AttributeModel"
#include "AttributeValueDelegate"
#include "Project"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
 public:
  MainWindow(int argc, char **argv, QWidget* parent =0);
  virtual ~MainWindow();
 public slots:
  void on_actionQuit_triggered();
  void on_actionNetLoad_triggered();
  void on_actionProjectLoad_triggered();
 private:
  bool dirty_;	
  Project project_;
  SemanticNet semNet_;
  AttributeModel attributeModel_;
  AttributeValueDelegate attributeValueDelegate_;
  /** filename for the semantic net */
  QString fileSemanticNet_;
};

#endif
