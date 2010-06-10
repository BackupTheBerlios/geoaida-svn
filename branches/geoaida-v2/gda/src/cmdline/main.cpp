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
#include <unistd.h>
#include "GeneralException"

using namespace std;

void Usage(const char* prgname)
{
  std::cout << "Usage:" << endl;
  std::cout << "  " << prgname << " -p <project-file>" << endl;
  
}

int main(int argc, char **argv)
{
  try {
    if (argc<=1) {
      Usage(argv[0]);
      return(1);
    }
    QCoreApplication app(argc, argv);
    int opt;
    const char *filename=0;
    while ((opt = getopt(argc, argv, "bmp:")) != -1) {
      switch (opt) {
      case 'm':
	break;
      case 'b':
	break;
      case 'p':
	filename=optarg;
      }
    }
    //  Q_INIT_RESOURCE(gda);
    cleanUp_.prefix(PRGDIR);
    QDir d(PRGDIR);
    d.cd("share/data/operators");
    operatorList_.readDir(d.path());	
    Project project;
    project.read(filename);
    project.analyze();
    project.saveResults();
    return 0;
  }
  catch  (const BaseException& e) {
    qCritical(("\033[0;31m"+e.what()+"\n"+e.debug()+"\33[0;22m").toLatin1().constData());
    return 1;
  }
}
