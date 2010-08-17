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
/// \date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_CLIENT_H
#define __IMAGE_CLIENT_H

//--- Standard header --------------------------------------------------------//
#include <iostream>

//--- Program header ---------------------------------------------------------//
#include "image_engine_commons.h"

//--- Misc header ------------------------------------------------------------//
#include <QCoreApplication>
#include <QHostAddress>
#include <QList>
#include <QString>
#include <QTcpSocket>
#include <QVariant>
#include <QAbstractSocket>

namespace GA{namespace IE{

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
        ImageClient(const QString& Host, const quint16& unPort);
        ~ImageClient();

        //--- Constant Methods -----------------------------------------------//
        
        //--- Methods --------------------------------------------------------//
        void getPartOfImage(QString imageKey,
                            float GeoWest, float GeoNorth,
                            float GeoEast, float GeoSouth,
                            QString FileName
                            );
        void addImage(QString FileName, QString imageKey,
                                        float GeoWest, float GeoNorth,
                                        float GeoEast, float GeoSouth);
        void addImages(QString FileName);
        void setupServer();
        void shutdownServer();

    private slots:

        //--- Methods --------------------------------------------------------//
        void displayError(QAbstractSocket::SocketError);
        void receiveData();
        void sendRequest();

    signals:
      
	void responseReceived(quint16 response);
	void closed();
      
    private:
	//--- Private Structs / Classes --------------------------------------//
	struct Request
	{
	    quint16		m_Request;
	    QList<QVariant>	m_ParameterList;
	    
	    Request()
	      : m_Request(0), m_ParameterList()
	    {
	    }
	    
	    Request(quint16 request, const QList<QVariant> &parameterList)
	      : m_Request(request), m_ParameterList(parameterList)
	    {
	    }
	};
	
        //--- Constant Methods -----------------------------------------------//
        void connectToServer() const;

        //--- Methods --------------------------------------------------------//        
    
        //--- Private Variables ----------------------------------------------//
        QTcpSocket*     m_pTcpSocket;           ///< Socked for this connection
        quint16         m_unPort;               ///< Port for connection
        QString         m_Host;                 ///< Host for connection
        
	QList<Request>  m_Requests;		///< Request queue
};

}}

#endif
