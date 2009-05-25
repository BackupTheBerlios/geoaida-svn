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
/// \file		image_server.h
/// \brief		Prototype of class "ImageServer"
///
///	\date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_SERVER_H
#define __IMAGE_SERVER_H

//--- Standard header --------------------------------------------------------//
// #include <iostream>

//--- Program header ---------------------------------------------------------//
#include "ImageEngineBase"
#include "ImageEngineDefault"

//--- Misc header ------------------------------------------------------------//
#include <QTcpServer>
// #include <QTcpSocket>

//--- Constants --------------------------------------------------------------//
// const quint16 REQUEST_PART_OF_IMAGE	= 1;
// const quint16 REQUEST_SETUP_SERVER	= 2;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Class that implements a TCP server to handle images.
/// 
////////////////////////////////////////////////////////////////////////////////
class ImageServer : public QTcpServer
{

	Q_OBJECT

	public:

		//--- Constructor / Destructor----------------------------------------//
		ImageServer(QObject* parent=0);

		//--- Methods --------------------------------------------------------//

		//--- Constant Methods -----------------------------------------------//
	
	protected:

		void incomingConnection(int socketDescriptor);
		
	private:

		//--- Methods --------------------------------------------------------//
// 		void setup();

		//--- Private Variables ----------------------------------------------//
//		QTcpServer* 		m_pTcpServer;		///< QT TCP server instance
//		QTcpSocket*			m_pClientSocket;	///< Socket of connected client
		ImageEngineBase*	m_pImageEngine;		///< Pointer to image engine
//		quint16				m_nHeader;			///< Header for data transfer

};

#endif
