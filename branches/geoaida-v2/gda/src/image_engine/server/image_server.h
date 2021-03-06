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
/// \date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///			Karsten Vogt (vogt@tnt.uni-hannover.de
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_SERVER_H
#define __IMAGE_SERVER_H

//--- Standard header --------------------------------------------------------//

//--- Program header ---------------------------------------------------------//
#include "image_engine_base.h"
#include "image_engine_default.h"

//--- Misc header ------------------------------------------------------------//
#include <QtCore>
#include <QtNetwork>

//--- Constants --------------------------------------------------------------//

namespace GA{namespace IE{

//class ImageServerThread;

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
	void openPendingClientConnection();
	void getRequest();
        void shutdown();
    
    signals:
	void closed();
      
    private:

	//--- Methods --------------------------------------------------------//
	void requestAddImage(const QList<QVariant> &parameterList);
	void requestAddImages(const QList<QVariant> &parameterList);
	void requestPartOfImage(const QList<QVariant> &parameterList);
	void requestSetupServer(const QList<QVariant> &parameterList);
	void requestShutdownServer(const QList<QVariant> &parameterList);

        //--- Constant Methods -----------------------------------------------//
        void sendRequestReturnValue(const quint16& nReturn) const;

        //--- Private Variables ----------------------------------------------//
        ImageEngineBase*    		m_pImageEngine;     ///< Pointer to image engine
};

}}

#endif
