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
/// \file		image_client_commons.cpp
/// \brief		Implementation of common variables and functions
///
/// \date		2009-10-14
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "image_engine_commons.h"

using namespace GA::IE;

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Translates constants to string for output.
///
/// \param nConst Constant descriptor
///
///////////////////////////////////////////////////////////////////////////////
const char* GA::IE::constToString(const quint16& nConst)
{
    switch(nConst)
    {
        case REQUEST_PART_OF_IMAGE:
            return QT_TR_NOOP("REQUEST_PART_OF_IMAGE");
        case REQUEST_SETUP_SERVER:
            return QT_TR_NOOP("REQUEST_SETUP_SERVER");
        case REQUEST_SHUTDOWN_SERVER:
            return QT_TR_NOOP("REQUEST_SHUTDOWN_SERVER");
        case REQUEST_RETURN_VALUE_ACCEPT:
            return QT_TR_NOOP("REQUEST_RETURN_VALUE_ACCEPT");
        case REQUEST_RETURN_VALUE_WRONG_PARAM:
            return QT_TR_NOOP("REQUEST_RETURN_VALUE_WRONG_PARAM");
        case REQUEST_ADD_IMAGE:
            return QT_TR_NOOP("REQUEST_ADD_IMAGE");
        case REQUEST_ADD_IMAGES:
            return QT_TR_NOOP("REQUEST_ADD_IMAGES");
        default:
            return QT_TR_NOOP("REQUEST_UNKNOWN");
    }
}
