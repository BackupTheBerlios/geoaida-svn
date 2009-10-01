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
#include <QCoreApplication>
#include <QDir>
#include "OperatorList"
#include "CleanUp"
#include <iostream>
#include <getopt.h>
#include "Project"

using namespace std;

void Usage(const char* prgname)
{
  std::cout << "Usage:" << endl;
  std::cout << "  " << prgname << " <project-file>" << endl;
  
}

int main(int argc, char **argv)
{
  if (argc<=1) {
    Usage(argv[0]);
    return(1);
  }
  QCoreApplication app(argc, argv);
  //  Q_INIT_RESOURCE(gda);
  cleanUp_.prefix(PRGDIR);
  QDir d(PRGDIR);
  d.cd("share/data/operators");
  operatorList_.readDir(d.path());	
  Project project;
  project.read(argv[1]);
//   MainWindow *mainWindow = new MainWindow(argc,argv);
//   mainWindow->show();
  project.analyze();
  project.saveResults();
  //  app.exec();

  //  int ret_val=app.exec();
  return 0;

}
