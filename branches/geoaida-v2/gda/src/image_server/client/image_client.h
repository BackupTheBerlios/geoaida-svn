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
/// \file		image_client.h
/// \brief		Prototype of class "ImageClient"
///
///	\date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_CLIENT_H
#define __IMAGE_CLIENT_H

//--- Standard header --------------------------------------------------------//
#include <iostream>

//--- Program header ---------------------------------------------------------//

//--- Misc header ------------------------------------------------------------//
#include <QHostAddress>
#include <QTcpSocket>

//--- Constants --------------------------------------------------------------//
const quint16 REQUEST_PART_OF_IMAGE	= 1;
const quint16 REQUEST_SETUP_SERVER	= 2;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Class that implements a TCP client to connect to image server.
/// 
////////////////////////////////////////////////////////////////////////////////
class ImageClient : public QObject
{

	Q_OBJECT

	public:

		//--- Constructor / Destructor----------------------------------------//
		ImageClient();
		~ImageClient();

		//--- Constant Methods -----------------------------------------------//
		
		//--- Methods --------------------------------------------------------//
		void getPartOfImage(QString InputImage,
							float GeoWest, float GeoNorth,
							float GeoEast, float GeoSouth,
							QString FileName
							);
		void setupServer();

	private slots:

		//--- Methods --------------------------------------------------------//
		void connectionEstablished();
		void displayError(QAbstractSocket::SocketError);

	private:
		//--- Constant Methods -----------------------------------------------//
		void connectToServer() const;
	  
		//--- Private Variables ----------------------------------------------//
		QTcpSocket* pTcpSocket;
		quint8		m_nRequest;
};

#endif
