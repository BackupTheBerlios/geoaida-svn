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
/// \file		image_server_thread.cpp
/// \brief		Implementation of class "ImageServerThread"
///
/// \date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "ImageServerThread"

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param nSocketDescriptor Description of socket for this connection
/// \param pParent Qt parent object
/// \param pImageEngine Pointer to instance of the image engine the server uses
///
///////////////////////////////////////////////////////////////////////////////
ImageServerThread::ImageServerThread(int nSocketDescriptor, QObject* pParent,
									const ImageEngineBase* const pImageEngine) :
									m_nSocketDescriptor(nSocketDescriptor),
									QThread(pParent),
									m_pImageEngine(pImageEngine)
{
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
///////////////////////////////////////////////////////////////////////////////
ImageServerThread::~ImageServerThread()
{
	if (m_pClientSocket!=0)
	{
		delete m_pClientSocket;
		m_pClientSocket = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Quits the thread if connection (socket) is disconnected.
///
///////////////////////////////////////////////////////////////////////////////
void ImageServerThread::socketDisconnected()
{
	std::cout << "ImageServerThread: Socket disconnected, leaving thread." << std::endl;
	quit();
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief This method sets up the socket when the thread is started.
///
///////////////////////////////////////////////////////////////////////////////
void ImageServerThread::threadStarted()
{
	std::cout << "ImageServerThread: Thread running." << std::endl;
	
	m_pClientSocket = new QTcpSocket(this);

	connect(m_pClientSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
	connect(m_pClientSocket, SIGNAL(readyRead()), this, SLOT(getRequest()));
	
	if (!m_pClientSocket->setSocketDescriptor(m_nSocketDescriptor))
	{
		std::cerr << "ImageServerThread: Error setting socket descriptor." << std::endl;
		return;
	}
	m_nHeader = 0;
	m_nStreamSize = 0;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Slot that receives a request from client
///
///////////////////////////////////////////////////////////////////////////////
void ImageServerThread::getRequest()
{
	std::cout << "ImageServerThread: Ready to receive data." << std::endl;
	
	// Prepare outstream to request part of image
	QDataStream in(m_pClientSocket);
	in.setVersion(QDataStream::Qt_4_0);
	
	if (m_nHeader == 0)
	{
		// Header is 2 bytes (sizeof(quint16))
		if (m_pClientSocket->bytesAvailable() < (int)sizeof(quint16))
			return;
		
		std::cout << "ImageServerThread: Bytes available at socket: " << 
					m_pClientSocket->bytesAvailable() << std::endl;

		in >> m_nHeader;
	}
	
	/// \todo Depending on what was requested, prepare to receive other information.
	///       If ready, send answer to client.
// 	if (m_pClientSocket->bytesAvailable() < m_nBlockSize)
// 		return;
	
	std::cout << "ImageServerThread: Incoming request = " << requestString(m_nHeader)
				<< std::endl;
	
	if (m_nStreamSize == 0)
	{
		// Header is 2 bytes (sizeof(quint16))
		if (m_pClientSocket->bytesAvailable() < (int)sizeof(quint64))
			return;
		
		std::cout << "ImageServerThread: Bytes available at socket: " << 
					m_pClientSocket->bytesAvailable() << std::endl;

		in >> m_nStreamSize;
		std::cout << "ImageServerThread: Size of stream: " << m_nStreamSize << std::endl;
	}

	switch (m_nHeader)
	{
		case REQUEST_SETUP_SERVER:
		{
			break;
		}
		case REQUEST_PART_OF_IMAGE:
		{
			m_pImageEngine->getPartOfImage("Test 1", 1, 1, 1, 1, "Test 2");
			break;
		}
		default:
			std::cout << "ImageServerThread: Unknown request " << m_nHeader << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Translates request constant into string.
///
/// \param nRequest Request descriptor
///
///////////////////////////////////////////////////////////////////////////////
const std::string ImageServerThread::requestString(const quint16& nRequest) const
{
	switch(nRequest)
	{
		case REQUEST_PART_OF_IMAGE:
			return "REQUEST_PART_OF_IMAGE";
		case REQUEST_SETUP_SERVER:
			return "REQUEST_SETUP_SERVER";
		default:
			return "REQUEST_UNKNOWN";
	}
}
