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
///
////////////////////////////////////////////////////////////////////////////////

#include "image_server.h"
#include "image_server_thread.h"

using namespace GA::IE;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param unPort  Port for connection
/// \param pParent Qt parent object
///
////////////////////////////////////////////////////////////////////////////////
ImageServer::ImageServer(const quint16& unPort, QObject* pParent) : QTcpServer(pParent),
                                                                    m_bFinished(false)
{
    // Server setup
    if (!this->listen(QHostAddress::Any, unPort))
    {
        std::cerr << "ImageServer: Error, server not running." << std::endl;
        this->close();
        return;
    }

    std::cout << "ImageServer: The server is running on port " <<
    this->serverPort() << std::endl;

    // Create instance of image engine
    m_pImageEngine = new ImageEngineDefault;
    
    // Start the event loop
    eventLoop();
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Shuts down the server
///
/// \todo Shutting down the server is not 100 percent clean yet. The thread
///       that was instanciated by the shutdown request will be quit correctly
///       but others should also. Therefore, the ImageServer could have a listen
///       of threads. These have to be quit before shutting down. To make the
///       server know that a thread ended, a signal could be send to delete it
///       from threadlist.
///
////////////////////////////////////////////////////////////////////////////////
void ImageServer::shutdown()
{
    std::cout << "ImageServer: Shutting down." << std::endl;
    m_bFinished = true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Starts the event loop
///
///////////////////////////////////////////////////////////////////////////////
void ImageServer::eventLoop()
{
    m_bFinished = false;
    while (!m_bFinished) 
        QCoreApplication::processEvents(QEventLoop::AllEvents,30);
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Method creates a new thread for every incoming connection.
///
/// \param nSocketDescriptor Associated to socket for this connection
///
////////////////////////////////////////////////////////////////////////////////
void ImageServer::incomingConnection(int nSocketDescriptor)
{
    std::cout << "ImageServer: New incoming connection." << std::endl;
    
    ImageServerThread* pThread = new ImageServerThread(nSocketDescriptor, this, m_pImageEngine);
    connect(pThread, SIGNAL(finished()), pThread, SLOT(deleteLater()));
    connect(pThread, SIGNAL(started() ), pThread, SLOT(threadStarted()));
    connect(pThread, SIGNAL(shutdown()), this   , SLOT(shutdown()));
    
    pThread->start();
}
