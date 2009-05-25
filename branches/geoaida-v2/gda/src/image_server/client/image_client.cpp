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
/// \file		image_client.cpp
/// \brief		Implementation of class "ImageClient"
///
/// \date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "ImageClient"

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
///////////////////////////////////////////////////////////////////////////////
ImageClient::ImageClient()
{
	pTcpSocket = new QTcpSocket(this);
	
	// Establish connections
	connect(pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(displayError(QAbstractSocket::SocketError)));
			 
	connect(pTcpSocket, SIGNAL(connected()), this, SLOT(connectionEstablished()));
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
///////////////////////////////////////////////////////////////////////////////
ImageClient::~ImageClient()
{
	if (pTcpSocket != 0)
	{
		delete pTcpSocket;
		pTcpSocket = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to request a part of an image from image server
///
/// This method connects to the image server to request a part of an image.
///
/// \param InputImage Path and file name of input image
/// \param GeoWest Geo coordinate west of part to be cut
/// \param GeoNorth Geo coordinate north of part to be cut
/// \param GeoEast Geo coordinate east of part to be cut
/// \param GeoSouth Geo coordinate south of part to be cut
/// \param FileName File name
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::getPartOfImage(QString InputImage,
										float GeoWest, float GeoNorth,
										float GeoEast, float GeoSouth,
										QString FileName
										)
{
	m_nRequest = REQUEST_PART_OF_IMAGE;
	connectToServer();
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief If connection is established, this method will begin the data transfer
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::connectionEstablished()
{
	// Prepare outstream to request part of image
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	
	out << (quint16)m_nRequest;

	/// \todo Depending on m_nRequest, all other information should be transferred
	
	connect(pTcpSocket, SIGNAL(disconnected()), pTcpSocket, SLOT(deleteLater()));
	
	pTcpSocket->write(block);
	
	/// \todo If all data is transferred, wait for answer, probably using
	///       slot that connects to readyReadSignal.
	
	pTcpSocket->disconnectFromHost();
	
	switch (m_nRequest)
	{
		case REQUEST_PART_OF_IMAGE:
			std::cout << "ImageClient: Sending REQUEST_PART_OF_IMAGE." << std::endl;
			break;
		case REQUEST_SETUP_SERVER:
			std::cout << "ImageClient: Sending REQUEST_SETUP_SERVER." << std::endl;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to output error message in case of connection error.
///
/// \param QAbstractSocket::SocketError Error description
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::displayError(QAbstractSocket::SocketError)
{
	std::cout << "ImageClient: Error, no connection." << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to setup the image server
///
/// This method connects to the image server and sets it up by transfering
/// image data and geocoordinates.
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::setupServer()
{
	m_nRequest = REQUEST_SETUP_SERVER;
	connectToServer();
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to establish a connection to image server
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::connectToServer() const
{
	std::cout << "ImageClient: Connecting to server." << std::endl;
	pTcpSocket->abort();
	pTcpSocket->connectToHost(QHostAddress::LocalHost,19209);
}
