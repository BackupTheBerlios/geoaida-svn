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
/// \file		image_server.cpp
/// \brief		Implementation of class "ImageServer"
///
/// \date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "ImageServer"
#include "ImageServerThread"

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param pParent Parent object
///
////////////////////////////////////////////////////////////////////////////////
ImageServer::ImageServer(QObject* pParent) : QTcpServer(pParent)
{
	// Server setup
	if (!this->listen(QHostAddress::LocalHost, 19209))
	{
		std::cerr << "ImageServer: Error, server not running." << std::endl;
		this->close();
		return;
	}

	std::cout << "ImageServer: The server is running on port " <<
	this->serverPort() << std::endl;

	// Create instance of image engine
	m_pImageEngine = new ImageEngineDefault;
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Method creates a new thread for every incoming connection.
///
/// \param nSocketDescriptor Associated to socket for this connection
///
////////////////////////////////////////////////////////////////////////////////
void ImageServer::incomingConnection(int nSocketDescriptor)
{
	std::cout << "ImageServer: New incoming connection." << std::endl;
	
	ImageServerThread* pThread = new ImageServerThread(nSocketDescriptor, this, m_pImageEngine);
	connect(pThread, SIGNAL(finished()), pThread, SLOT(deleteLater()));
	connect(pThread, SIGNAL(started() ), pThread, SLOT(threadStarted()));
	
	pThread->start();
}
