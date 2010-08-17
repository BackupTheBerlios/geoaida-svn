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
///			Karsten Vogt (vogt@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "image_server.h"
#include "image_engine_commons.h"

using namespace GA::IE;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param unPort  Port for connection
/// \param pParent Qt parent object
///
////////////////////////////////////////////////////////////////////////////////
ImageServer::ImageServer(const quint16& unPort, QObject* pParent) : QTcpServer(pParent)
{
    // Server setup
    if (!this->listen(QHostAddress::Any, unPort))
    {
        qCritical() << tr("ImageServer: Error, server not running.");
	
        this->close();
        return;
    }

    qDebug() << tr("ImageServer: The server is running on port ")
	     << this->serverPort();
	     
    connect(this, SIGNAL(newConnection()), this, SLOT(openPendingClientConnection()));

    // Create instance of image engine
    m_pImageEngine = new ImageEngineDefault;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Slot that opens an incoming client connection
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::openPendingClientConnection()
{
    qDebug() << tr("ImageServer: New incoming connection.");
 
    QTcpSocket* pClientSocket = nextPendingConnection();
    if (!pClientSocket)
    {
      qCritical() << tr("ImageServer: Error, no pending client connections.");
      return;
    }
    
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(getRequest()));
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Slot that receives a request from client
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::getRequest()
{
    QTcpSocket* pClientSocket = static_cast<QTcpSocket*>(this->sender());
    if (!pClientSocket)
    {
	qCritical() << tr("ImageServer: Error, no valid client connection");
	return;
    }
    
    // Prepare stream to request part of image
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_0);
    
    qDebug() << tr("ImageServer: Bytes available at socket: ")
             << pClientSocket->bytesAvailable();

    while (pClientSocket->bytesAvailable() >= sizeof(quint64))
    {
	// Get the size of the data stream
	quint64 nStreamSize;
	
	in >> nStreamSize;
	    
	qDebug() << tr("ImageServer: Stream size: ") << nStreamSize;

	// Read the next packet from the stream
	if ((pClientSocket->bytesAvailable() >= (nStreamSize - sizeof(quint64))) &&
	    (nStreamSize != 0))
	{
	    // Read request type and parameters
	    quint16 nHeader;
	    quint8 nNumberOfParameters;
	    QList<QVariant> ParameterList;
	    
	    in >> nHeader;
	    in >> nNumberOfParameters;
	    
	    for (int i = 0; i < nNumberOfParameters; i++)
	    {
		QVariant parameter;
		
		in >> parameter;
		
		ParameterList.push_back(parameter);
	    }
	    
	    qDebug() << tr("ImageServer: Incoming request = ") << constToString(nHeader);
	    qDebug() << tr("ImageServer: Number of parameters = ") << nNumberOfParameters;
	    
	    // Handle requests
	    switch (nHeader)
	    {
		case REQUEST_ADD_IMAGE:
		  requestAddImage(ParameterList);
		  break;
		  
		case REQUEST_ADD_IMAGES:
		  requestAddImages(ParameterList);
		  break;
		  
		case REQUEST_PART_OF_IMAGE:
		  requestPartOfImage(ParameterList);
		  break;
		  
		case REQUEST_SETUP_SERVER:
		  requestSetupServer(ParameterList);
		  break;
		  
		case REQUEST_SHUTDOWN_SERVER:
		  requestShutdownServer(ParameterList);
		  break;
		
		default:
		  qDebug() << tr("ImageServer: Unknown request ") << nHeader;
	    }
	}
    }
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Handle "add image" request
///
/// \param parameterList
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::requestAddImage(const QList<QVariant> &parameterList)
{
    if (parameterList.count() != 6)
    {
	qCritical() << tr("ImageServer: Wrong number of parameters.");
	
	this->sendRequestReturnValue(REQUEST_RETURN_VALUE_WRONG_PARAM);
	return;
    }
    
    /*
      Parameters:
      0:	filename
      1:	image key
      2:	geo-coord west
      3:	geo-coord north
      4:	geo-coord east
      5:	geo-coord south
    */
    
    m_pImageEngine->addImage(parameterList[0].toString(), parameterList[1].toString(),
			     parameterList[2].toDouble(), parameterList[3].toDouble(),
			     parameterList[4].toDouble(), parameterList[5].toDouble());

    this->sendRequestReturnValue(REQUEST_RETURN_VALUE_ACCEPT);
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Handle "add images" request
///
/// \param parameterList
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::requestAddImages(const QList<QVariant> &parameterList)
{
    if (parameterList.count() != 1)
    {
	qCritical() << tr("ImageServer: Wrong number of parameters.");
	
	this->sendRequestReturnValue(REQUEST_RETURN_VALUE_WRONG_PARAM);
	return;
    }

    /*
      Parameters:
      0:	filename
    */

    m_pImageEngine->addImages(parameterList[0].toString());

    this->sendRequestReturnValue(REQUEST_RETURN_VALUE_ACCEPT);
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Handle "get part of image" request
///
/// \param parameterList
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::requestPartOfImage(const QList<QVariant> &parameterList)
{
    if (parameterList.count() != 6)
    {
	qCritical() << tr("ImageServer: Wrong number of parameters.");
	
	this->sendRequestReturnValue(REQUEST_RETURN_VALUE_WRONG_PARAM);
	return;
    }
    
    /*
      Parameters:
      0:	image key
      1:	geo-coord west
      2:	geo-coord north
      3:	geo-coord east
      4:	geo-coord south
      5:	filename
    */

    m_pImageEngine->getPartOfImage(parameterList[0].toString(), parameterList[1].toDouble(),
				   parameterList[2].toDouble(), parameterList[3].toDouble(),
				   parameterList[4].toDouble(), parameterList[5].toString());

    this->sendRequestReturnValue(REQUEST_RETURN_VALUE_ACCEPT);
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Handle "setup server" request
///
/// \param parameterList
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::requestSetupServer(const QList<QVariant> &parameterList)
{
    if (parameterList.count() != 0)
    {
	qCritical() << tr("ImageServer: Wrong number of parameters.");
	
	this->sendRequestReturnValue(REQUEST_RETURN_VALUE_WRONG_PARAM);
	return;
    }

    this->sendRequestReturnValue(REQUEST_RETURN_VALUE_ACCEPT);
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Handle "shutdown server" request
///
/// \param parameterList
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::requestShutdownServer(const QList<QVariant> &parameterList)
{
    if (parameterList.count() != 0)
    {
	qCritical() << tr("ImageServer: Wrong number of parameters.");
	
	this->sendRequestReturnValue(REQUEST_RETURN_VALUE_WRONG_PARAM);
	return;
    }

    this->sendRequestReturnValue(REQUEST_RETURN_VALUE_ACCEPT);
    shutdown();
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Send return value back to client.
///
/// \param nReturnValue
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::sendRequestReturnValue(const quint16& nReturn) const
{
    qDebug() << tr("ImageServerThread: Sending ") << constToString(nReturn);
    
    QTcpSocket* pClientSocket = static_cast<QTcpSocket*>(this->sender());
    if (!pClientSocket)
    {
	qCritical() << tr("ImageServer: Error, no valid client connection.");
	return;
    }
    
    // Prepare outstream to send return value
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    
    // Send return value
    out << nReturn;
    pClientSocket->write(block);
    pClientSocket->waitForBytesWritten(1000);
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Shuts down the server
///
////////////////////////////////////////////////////////////////////////////////
void ImageServer::shutdown()
{
    qDebug() << tr("ImageServer: Shutting down.");
    
    this->close();
    emit closed();
}
