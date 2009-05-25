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
/// \file		image_server_thread.h
/// \brief		Prototype of class "ImageServerThread"
///
///	\date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_SERVER_THREAD_H
#define __IMAGE_SERVER_THREAD_H

//--- Standard header --------------------------------------------------------//
#include <iostream>
// #include <string>

//--- Program header ---------------------------------------------------------//
// #include "ImageEngineBase"
// #include "ImageEngineDefault"

//--- Misc header ------------------------------------------------------------//
// #include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

//--- Constants --------------------------------------------------------------//
const quint16 REQUEST_PART_OF_IMAGE	= 1;
const quint16 REQUEST_SETUP_SERVER	= 2;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Class implementing a thread of the TCP server.
/// 
////////////////////////////////////////////////////////////////////////////////
class ImageServerThread : public QThread
{

	Q_OBJECT

	public:

		//--- Constructor / Destructor----------------------------------------//
		ImageServerThread(int nSocketDescriptor, QObject* pParent);
		~ImageServerThread();

		//--- Methods --------------------------------------------------------//

		//--- Constant Methods -----------------------------------------------//
	
	private slots:

		void socketDisconnected();
		void threadStarted();
		void getRequest();

	private:
		//--- Constant Methods -----------------------------------------------//
		const QString requestString(const quint16& nRequest) const;

		//--- Private Variables ----------------------------------------------//
		QTcpSocket*			m_pClientSocket;	///< Socket for connection
//		ImageEngineBase*	m_pImageEngine;		///< Pointer to image engine
		quint16				m_nHeader;			///< Header for data transfer
		
		int m_nSocketDescriptor;				///< Associated to socket for this connection

};

#endif
