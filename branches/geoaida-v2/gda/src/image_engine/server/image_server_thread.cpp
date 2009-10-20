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

using namespace GA::IE;

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
    m_nNumberOfParameters = 0;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Slot that receives a request from client
///
///////////////////////////////////////////////////////////////////////////////
void ImageServerThread::getRequest()
{	
    // Prepare stream to request part of image
    QDataStream in(m_pClientSocket);
    in.setVersion(QDataStream::Qt_4_0);
    
    std::cout << "ImageServerThread: Bytes available at socket: " << 
                    m_pClientSocket->bytesAvailable() << std::endl;

    // Get the size of the data stream
    if (m_nStreamSize == 0)
    {
        if (m_pClientSocket->bytesAvailable() < (int)sizeof(quint64))
            return;
        
        in >> m_nStreamSize;
        
        std::cout << "ImageServerThread: Stream size: " << 
                    m_nStreamSize << std::endl;
    }

    // Read header and request
    if ((m_pClientSocket->bytesAvailable() == m_nStreamSize-(int)sizeof(quint64)) &&
        (m_nStreamSize != 0))
    {	
        in >> m_nHeader;
        in >> m_nNumberOfParameters;
        std::cout << "ImageServerThread: Incoming request = " << constToString(m_nHeader)
                << std::endl;
        std::cout << "ImageServerThread: Number of parameters = " << int(m_nNumberOfParameters)
                << std::endl;
        switch (m_nHeader)
        {
            case REQUEST_PART_OF_IMAGE:
            {
                if (m_nNumberOfParameters != 6)
                {
                    std::cout << "ImageServerThread: Wrong number of parameters." << std::endl;
                    this->sendRequestReturnValue(REQUEST_RETURN_VALUE_WRONG_PARAM);
                    return;
                }
                QVariant InputImage;
                QVariant GeoWest;
                QVariant GeoNorth;
                QVariant GeoEast;
                QVariant GeoSouth;
                QVariant FileName;
                in >> InputImage;
                in >> GeoWest;
                in >> GeoNorth;
                in >> GeoEast;
                in >> GeoSouth;
                in >> FileName;

                m_pImageEngine->getPartOfImage(InputImage.toString(), GeoWest.toDouble(),
                                            GeoNorth.toDouble(), GeoEast.toDouble(),
                                            GeoSouth.toDouble(), FileName.toString());
                
                this->sendRequestReturnValue(REQUEST_RETURN_VALUE_ACCEPT);
                break;
            }
            case REQUEST_SETUP_SERVER:
            {
                break;
            }
            case REQUEST_SHUTDOWN_SERVER:
            {
                break;
            }
            default:
                std::cout << "ImageServerThread: Unknown request " << m_nHeader << std::endl;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Send return value back to client.
///
/// \param nReturnValue
///
///////////////////////////////////////////////////////////////////////////////
void ImageServerThread::sendRequestReturnValue(const quint16& nReturn) const
{
    std::cout << "ImageServerThread: Sending " << constToString(nReturn) 
                << std::endl;
    
    // Prepare outstream to send return value
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    
    // Send return value
    out << nReturn;
    m_pClientSocket->write(block);
}
