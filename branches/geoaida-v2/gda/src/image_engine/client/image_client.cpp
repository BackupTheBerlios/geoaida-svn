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

#include "image_client.h"

using namespace GA::IE;

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param Host Name of host to connect to
/// \param unPort Port for this connection
///
///////////////////////////////////////////////////////////////////////////////
ImageClient::ImageClient(const QString& Host,
                         const quint16& unPort) : m_pTcpSocket(0),
                                                  m_unPort(unPort),
                                                  m_Host(Host)
{
    m_pTcpSocket = new QTcpSocket(this);
    
    // Establish connections
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
    
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
    connect(m_pTcpSocket, SIGNAL(disconnected()), this, SIGNAL(closed()));
    
    // Connect to server
    connectToServer();
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
///////////////////////////////////////////////////////////////////////////////
ImageClient::~ImageClient()
{
    delete m_pTcpSocket;
    m_pTcpSocket = 0;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to add an image on image server
///
/// This method connects to the image server to add an image.
///
/// \param FileName File name
/// \param key Inage key
/// \param GeoWest Geo coordinate west
/// \param GeoNorth Geo coordinate north
/// \param GeoEast Geo coordinate east
/// \param GeoSouth Geo coordinate south
///
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::addImage(QString FileName, QString imageKey,
                                        float GeoWest, float GeoNorth,
                                        float GeoEast, float GeoSouth)
{
    Request request;
    request.m_Request = REQUEST_ADD_IMAGE;
    request.m_ParameterList.push_back(FileName);
    request.m_ParameterList.push_back(imageKey);
    request.m_ParameterList.push_back(GeoWest);
    request.m_ParameterList.push_back(GeoNorth);
    request.m_ParameterList.push_back(GeoEast);
    request.m_ParameterList.push_back(GeoSouth);
    m_Requests.push_front(request);
    
    QTimer::singleShot(0, this, SLOT(sendRequest()));
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to add images on image server
///
/// This method connects to the image server to add images.
///
/// \param FileName File name
///
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::addImages(QString fname)
{
    Request request;
    request.m_Request = REQUEST_ADD_IMAGES;
    request.m_ParameterList.push_back(fname);
    m_Requests.push_front(request);
    
    QTimer::singleShot(0, this, SLOT(sendRequest()));
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
void ImageClient::getPartOfImage(QString imageKey,
                                        float GeoWest, float GeoNorth,
                                        float GeoEast, float GeoSouth,
                                        QString FileName
                                        )
{
    Request request;
    request.m_Request = REQUEST_PART_OF_IMAGE;
    request.m_ParameterList.push_back(imageKey);
    request.m_ParameterList.push_back(GeoWest);
    request.m_ParameterList.push_back(GeoNorth);
    request.m_ParameterList.push_back(GeoEast);
    request.m_ParameterList.push_back(GeoSouth);
    request.m_ParameterList.push_back(FileName);
    m_Requests.push_front(request);
    
    QTimer::singleShot(0, this, SLOT(sendRequest()));
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
    Request request;
    request.m_Request = REQUEST_SETUP_SERVER;
    m_Requests.push_front(request);
    
    QTimer::singleShot(0, this, SLOT(sendRequest()));
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to shut down the image server
///
/// This method connects to the image server and sends a method to shut down
/// the server. That means, that its global event loop is stopped.
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::shutdownServer()
{
    Request request;
    request.m_Request = REQUEST_SHUTDOWN_SERVER;
    m_Requests.push_front(request);
    
    QTimer::singleShot(0, this, SLOT(sendRequest()));
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
    qCritical() << tr("ImageClient: Error, no connection.");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief If connection is established, this method will begin the data transfer
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::sendRequest()
{
  while (!m_Requests.empty())
  {
    Request request = m_Requests.takeLast();
    
    qDebug() << tr("ImageClient: Sending ") << constToString(request.m_Request);
    
    // Prepare outstream to request part of image
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    
    quint8 nNumberOfParameters = request.m_ParameterList.size();

    // Stream header information and parameters
    out << (quint64)0; // Reserved for size of data stream
    out << (quint16)request.m_Request;
    out << (quint8)(nNumberOfParameters);
    for (QList<QVariant>::const_iterator ci=request.m_ParameterList.begin();
					ci!=request.m_ParameterList.end(); ++ci)
    {
      out << (*ci);
    }
    
    qDebug() << tr("ImageClient: Stream size = ") << block.size();
    
    out.device()->seek(0);
    out << (quint64)(block.size()); // Write size of data stream
    
    m_pTcpSocket->write(block);
  }
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method receiving the data that was requested.
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::receiveData()
{
    // Prepare stream to request the data
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    
    quint16 nReturnVal = 0;
    
    if (m_pTcpSocket->bytesAvailable() == sizeof(quint16))
        in >> nReturnVal;
    
    qDebug() << tr("ImageClient: Received return value from server: ") 
                << constToString(nReturnVal);
    
    emit responseReceived(nReturnVal);
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to establish a connection to image server
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::connectToServer() const
{
    qDebug() << tr("ImageClient: Connecting to server.");
    
    m_pTcpSocket->abort();
    m_pTcpSocket->connectToHost(m_Host, m_unPort);
    m_pTcpSocket->waitForConnected();
}
