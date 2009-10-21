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

//--- Program header ---------------------------------------------------------//
#include "ImageEngineBase"
#include "ImageEngineDefault"

//--- Misc header ------------------------------------------------------------//
#include <QCoreApplication>
#include <QTcpServer>

//--- Constants --------------------------------------------------------------//

namespace GA{namespace IE{

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
        ImageServer(const quint16& unPort, QObject* parent=0);

        //--- Methods --------------------------------------------------------//

        //--- Constant Methods -----------------------------------------------//
    
    private slots:
        void shutdown();
    
    private:

        //--- Methods --------------------------------------------------------//
        void eventLoop();
        void incomingConnection(int socketDescriptor);

        //--- Private Variables ----------------------------------------------//
        bool                m_bFinished;        ///< Flags if event loop is still running
        ImageEngineBase*    m_pImageEngine;     ///< Pointer to image engine
};

}}

#endif
