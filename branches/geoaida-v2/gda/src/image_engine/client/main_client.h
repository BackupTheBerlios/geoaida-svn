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
/// \file		main_client.h
/// \brief		
///
/// \date		2010-04-21
/// \author		Karsten Vogt (vogt@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __MAIN_CLIENT_H
#define __MAIN_CLIENT_H

//--- Standard header --------------------------------------------------------//
#include <QtCore>

//--- Program header ---------------------------------------------------------//

//--- Misc header ------------------------------------------------------------//

namespace GA{namespace IE{

class ImageClient;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief This class handles the program logic.
/// 
////////////////////////////////////////////////////////////////////////////////
class MainClient : public QObject
{

    Q_OBJECT

    public:

        //--- Constructor / Destructor----------------------------------------//
        MainClient();
        ~MainClient();

        //--- Constant Methods -----------------------------------------------//
        
        //--- Methods --------------------------------------------------------//

    private slots:

        //--- Methods --------------------------------------------------------//
	void usage();
	void run();
	
	void processResponse(quint16 response);

    private:
        //--- Constant Methods -----------------------------------------------//

        //--- Methods --------------------------------------------------------//
        
        //--- Methods --------------------------------------------------------//
    
        //--- Private Variables ----------------------------------------------//
	ImageClient* m_client;
};

}}

#endif
