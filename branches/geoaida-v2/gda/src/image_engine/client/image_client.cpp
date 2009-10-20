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
                                                  m_Host(Host),
                                                  m_bFinished(false)
{
    m_pTcpSocket = new QTcpSocket(this);
    
    // Establish connections
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
            
    connect(m_pTcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
///////////////////////////////////////////////////////////////////////////////
ImageClient::~ImageClient()
{
    if (m_pTcpSocket != 0)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = 0;
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
    m_ParameterList.push_back(InputImage);
    m_ParameterList.push_back(GeoWest);
    m_ParameterList.push_back(GeoNorth);
    m_ParameterList.push_back(GeoEast);
    m_ParameterList.push_back(GeoSouth);
    m_ParameterList.push_back(FileName);
    connectToServer();
    eventLoop();
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
    eventLoop();
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to shut down the image server
///
/// This method connects to the image server and sends a method to shut down
/// the server. That means, that its global event loop is stopped.
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::shutDownServer()
{
    m_nRequest = REQUEST_SHUTDOWN_SERVER;
    connectToServer();
    eventLoop();
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
    
    // Stop event loop
    m_bFinished = true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief If connection is established, this method will begin the data transfer
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::sendRequest()
{
    std::cout << "ImageClient: Sending " << constToString(m_nRequest) << std::endl;
    
    // Prepare outstream to request part of image
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    
    quint8 nNumberOfParameters = m_ParameterList.size();

    // Stream header information and parameters
    out << (quint64)0; // Reserved for size of data stream
    out << (quint16)m_nRequest;
    out << (quint8)(nNumberOfParameters);
    for (QList<QVariant>::const_iterator ci=m_ParameterList.begin();
                                        ci!=m_ParameterList.end(); ++ci)
    {
        out << (*ci);
    }
    std::cout << "ImageClient: Stream size = " << block.size() << std::endl;
    out.device()->seek(0);
    out << (quint64)(block.size()); // Write size of data stream
    
    m_pTcpSocket->write(block);
    
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
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
    
    std::cout << "ImageClient: Received return value from server: " 
                << constToString(nReturnVal) << std::endl;
    
    std::cout << "ImageClient: Disconnecting from server." << std::endl;
    m_pTcpSocket->disconnectFromHost();
    
    // Stop event loop
    m_bFinished = true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to establish a connection to image server
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::connectToServer() const
{
    std::cout << "ImageClient: Connecting to server." << std::endl;
    m_pTcpSocket->abort();
    m_pTcpSocket->connectToHost(m_Host, m_unPort);
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to establish a connection to image server
///
///////////////////////////////////////////////////////////////////////////////
void ImageClient::eventLoop()
{
    m_bFinished = false;
    while (!m_bFinished) 
        QCoreApplication::processEvents(QEventLoop::AllEvents,30);
    
    // Has to be connected _after_ event loop is started
    connect(m_pTcpSocket, SIGNAL(disconnected()), m_pTcpSocket, SLOT(deleteLater()));
}
