////////////////////////////////////////////////////////////////////////////////
///
/// \file		paint.cpp
/// \brief		simple program for polyfill test purposes
///
/// \author		Torsten Büschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//--- standard header --------------------------------------------------------//
#include <iostream>

//--- project header ---------------------------------------------------------//
#include "gaimaget.h"
#include "gapainter.h"

//--- misc header ------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////////
///
/// \brief main function
///
/// this is the entrance point for program startup.
///
/// \param	argc number of given arguments
/// \param	argv array, storing the arguments
/// \return	exit code
///
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	Ga::Image img(typeid(unsigned char),500, 500);
	FILE* File;

	Ga::PointArray	points;
	Ga::Painter		painter(img);

	// white image
	img=255;

	// define the polygon
	points.push_back(Ga::Point(50,200));
	points.push_back(Ga::Point(200,100));
	points.push_back(Ga::Point(220,130));
	points.push_back(Ga::Point(310,80));
	points.push_back(Ga::Point(400,300));
	points.push_back(Ga::Point(180,280));
	points.push_back(Ga::Point(100,260));

	// draw the filled polygon
	painter.fillPolygon(points);

	// write file to disc
	File=fopen("Testfile","w"); 
	img.write(File);
	fclose(File);

	return 0;
}
