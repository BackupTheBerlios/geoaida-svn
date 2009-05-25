/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////
///
/// \mainpage	Image Client for GeoAIDA
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///
/// \file		main.cpp
/// \brief		Main program
///
/// \date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

//--- Standard header --------------------------------------------------------//
#include <cstdlib>
#include <iostream>
#include <string>

//--- Program header ---------------------------------------------------------//
#include "ImageClient"

//--- Misc header ------------------------------------------------------------//
#include <QCoreApplication>

//--- Major glocal instances -------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Prints out usage message if command was called incorrectly
///
/// \note This usage and the parameters are only for testing purposes.
///
////////////////////////////////////////////////////////////////////////////////
void usage()
{
	std::cout << "Usage: image_client [REQUEST]" << std::endl;
	std::cout << "  REQUEST:" << std::endl;
	std::cout << "   poi: part of image" << std::endl;
	std::cout << "   sus: setup server" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Main function
///
/// This is the entrance point for program startup.
///
/// \param	argc number of given arguments
/// \param	argv array, storing the arguments
/// \return	exit code
///
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	
	if (argc != 2)
	{
		usage();
		return EXIT_FAILURE;
	}

	ImageClient Client;
	
	if (std::string(argv[1]) == "poi")
		Client.getPartOfImage("Test 01", 1,1,1,1, "Test 02");
	if (std::string(argv[1]) == "sus")
		Client.setupServer();
	
	app.exec();

	return EXIT_SUCCESS;
}
