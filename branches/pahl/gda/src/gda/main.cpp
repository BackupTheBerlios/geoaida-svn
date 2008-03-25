/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Wed Jul 12 2000
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
#include <QApplication>
#include "MainWindow"
#include <getopt.h>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  
  MainWindow *mainWindow = new MainWindow(argc,argv);
  mainWindow->show();
  
  int ret_val=app.exec();
  return 0;

}
