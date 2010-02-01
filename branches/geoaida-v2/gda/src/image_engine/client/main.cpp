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
#include "image_client.h"

//--- Misc header ------------------------------------------------------------//
#include <QCoreApplication>

//--- Major glocal instances -------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Prints out usage message if command was called incorrectly
///
///
////////////////////////////////////////////////////////////////////////////////
void usage()
{
    std::cout << "Usage: image_client <host> <port> <request>" << std::endl;
    std::cout << "  <host>: Host where the server is running on." << std::endl;
    std::cout << "  <port>: Port for connection" << std::endl;
    std::cout << "  <request>:" << std::endl;
    std::cout << "     adf: add images by file" << std::endl;
    std::cout << "     add: add image" << std::endl;
    std::cout << "     poi: part of image" << std::endl;
    std::cout << "     sus: setup server" << std::endl;
    std::cout << "     sds: shutdown server" << std::endl;
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
    if (argc < 4)
    {
        usage();
        return EXIT_FAILURE;
    }

    QCoreApplication app(argc, argv);
    GA::IE::ImageClient Client(QString(argv[1]), atoi(argv[2]));
    
    if (std::string(argv[3]) == "adf")
    {
        if (argc!=5)
            return EXIT_FAILURE;
        Client.addImages(argv[4]);
    }
    else if (std::string(argv[3]) == "add")
    {
        if (argc!=10)
            return EXIT_FAILURE;
        Client.addImage(argv[4], argv[5], atof(argv[6]),  atof(argv[7]),  atof(argv[8]),  atof(argv[9]));
    }
    else if (std::string(argv[3]) == "poi")
    {
        if (argc!=10)
            return EXIT_FAILURE;
        Client.getPartOfImage(argv[4], atof(argv[5]),  atof(argv[6]), atof( argv[7]),  atof(argv[8]), argv[9]);
    }
    else if (std::string(argv[3]) == "sus")
        Client.setupServer();
    else if (std::string(argv[3]) == "sds")
        Client.shutdownServer();
    else
    {
        std::cerr << "Unknown request." << std::endl;
        usage();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
