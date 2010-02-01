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
/// \mainpage	Image Server for GeoAIDA
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
#include <iostream.h>

//--- Program header ---------------------------------------------------------//
#include "image_server.h"

//--- Misc header ------------------------------------------------------------//
#include <QCoreApplication>

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Prints out usage message if command was called incorrectly
///
////////////////////////////////////////////////////////////////////////////////
void usage()
{
    std::cout << "Usage: image_server <port>" << std::endl;
    std::cout << "  <port>: Port for connection" << std::endl;
    std::cout << "Example: image_server 19209" << std::endl;
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
    if (argc != 2)
    {
        usage();
        return EXIT_FAILURE;
    }

    QCoreApplication app(argc, argv);
    GA::IE::ImageServer Server(atoi(argv[1]));
    return EXIT_SUCCESS;
}
