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

//--- Program header ---------------------------------------------------------//
#include "image_engine_base.h"
#include "image_engine_commons.h"

//--- Misc header ------------------------------------------------------------//
#include <QList>
#include <QTcpSocket>
#include <QThread>
#include <QVariant>

//--- Constants --------------------------------------------------------------//

namespace GA{namespace IE{

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
        ImageServerThread(int nSocketDescriptor, QObject* pParent,
                            ImageEngineBase* pImageEngine);
        ~ImageServerThread();

        //--- Methods --------------------------------------------------------//

        //--- Constant Methods -----------------------------------------------//
    
    private slots:

        void socketDisconnected();
        void threadStarted();
        void getRequest();
        
    signals:
        void shutdown();

    private:
        //--- Constant Methods -----------------------------------------------//
        void sendRequestReturnValue(const quint16& nReturn) const;

        //--- Private Variables ----------------------------------------------//
        QTcpSocket*         m_pClientSocket;        ///< Socket for connection
        quint16             m_nHeader;              ///< Header for data transfer
        quint8              m_nNumberOfParameters;  ///< Number of parameters
        quint64             m_nStreamSize;          ///< Size of data stream
        QList<QVariant>     m_ParameterList;        ///< List of parameters depending on method call
        
        bool m_bShutdownRequested;                  ///< Flags if server should be shut down
        int m_nSocketDescriptor;                    ///< Associated to socket for this connection

        ImageEngineBase* m_pImageEngine;///< Pointer to image engine

};

}}

#endif
