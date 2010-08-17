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
/// \file		main_client.cpp
/// \brief		
///
/// \date		2010-04-21
/// \author		Karsten Vogt (vogt@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "main_client.h"

//--- Standard header --------------------------------------------------------//
#include <cstdlib>
#include <iostream>
#include <string>

//--- Program header ---------------------------------------------------------//
#include "image_client.h"

using namespace GA::IE;

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
///////////////////////////////////////////////////////////////////////////////
MainClient::MainClient()
{
    QStringList arguments = QCoreApplication::arguments();
    
    m_client = new ImageClient(arguments[1], arguments[2].toInt());
    
    connect(m_client, SIGNAL(responseReceived(quint16)), this, SLOT(processResponse(quint16)));
    connect(m_client, SIGNAL(closed()), this, SLOT(quit()));
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
///////////////////////////////////////////////////////////////////////////////
MainClient::~MainClient()
{
    delete m_client;
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Prints out usage message if command was called incorrectly
///
////////////////////////////////////////////////////////////////////////////////
void MainClient::usage()
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
/// \brief Execute the main program logic
///
////////////////////////////////////////////////////////////////////////////////
void MainClient::run()
{
    QStringList arguments = QCoreApplication::arguments();
    
    if (arguments.count() < 4)
    {
	usage();
	QCoreApplication::exit(EXIT_FAILURE);
    }
    
    if (arguments[3] == tr("adf"))
    {
	if (arguments.count() != 5)
	    QCoreApplication::exit(EXIT_FAILURE);
	
	m_client->addImages(arguments[4]);
    }
    else if (arguments[3] == tr("add"))
    {
	if (arguments.count() != 10)
	    QCoreApplication::exit(EXIT_FAILURE);
	
	m_client->addImage(arguments[4], arguments[5], arguments[6].toFloat(),  arguments[7].toFloat(), arguments[8].toFloat(), arguments[9].toFloat());
    }
    else if (arguments[3] == tr("poi"))
    {
	if (arguments.count() != 10)
	    QCoreApplication::exit(EXIT_FAILURE);
	
	m_client->getPartOfImage(arguments[4], arguments[5].toFloat(), arguments[6].toFloat(), arguments[7].toFloat(), arguments[8].toFloat(), arguments[9]);
    }
    else if (arguments[3] == tr("sus"))
	m_client->setupServer();
    else if (arguments[3] == tr("sds"))
	m_client->shutdownServer();
    else
    {
	qCritical() << tr("Unknown request.");
	usage();
	
	QCoreApplication::exit(EXIT_FAILURE);
    }
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Process the answer of the request and exit the program
///
////////////////////////////////////////////////////////////////////////////////
void MainClient::processResponse(quint16 response)
{
//     qDebug() << tr("ImageClient: Received return value from server: ") 
// 	     << constToString(response);

    QCoreApplication::exit(EXIT_SUCCESS);
}
