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
	std::cout << "Usage: image_client <host> <port> <request>" << std::endl;
	std::cout << "  <host>: Host where the server is running on." << std::endl;
	std::cout << "  <port>: Port for connection" << std::endl;
	std::cout << "  <request>:" << std::endl;
	std::cout << "     poi: part of image" << std::endl;
	std::cout << "     sus: setup server" << std::endl;
	std::cout << "Example: image_client localhost 19209 poi" << std::endl;
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
	if (argc != 4)
	{
		usage();
		return EXIT_FAILURE;
	}

	QCoreApplication app(argc, argv);
	std::cout << argv[1] << std::endl;
	ImageClient Client(QString(argv[1]), atoi(argv[2]));
	
	if (std::string(argv[3]) == "poi")
		Client.getPartOfImage("Test 01", 1.837483, 0.7356, 1.0e20, -19.0, "Test 00002");
	else if (std::string(argv[3]) == "sus")
		Client.setupServer();
	else
	{
		std::cerr << "Unknown request." << std::endl;
		usage();
		return EXIT_FAILURE;
	}
	
	app.exec();

	return EXIT_SUCCESS;
}
