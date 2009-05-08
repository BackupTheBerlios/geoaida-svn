/***************************************************************************
                          main.cpp  -
                             -------------------
    begin                : Mon Jul 07 2008
    copyright            : (C) 2008 TNT, Uni Hannover
    authors              : Karsten Vogt

    email                : vogt@tnt.uni-hannover.de
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

#include "MainWindow.h"

int main(int argc, char *argv[])
{
	//Q_INIT_RESOURCE(application);

	QApplication app(argc, argv);

	QString filename = (argc > 1) ? argv[1] : QString();
	MainWindow window(filename);
	window.show();

	return app.exec();
}
