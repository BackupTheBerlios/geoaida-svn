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
/// \file		image_engine_commons.h
/// \brief		Common variables for image engine
///
///	\date		2009-10-13
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_ENGINE_COMMONS_H
#define __IMAGE_ENGINE_COMMONS_H

#include <QtCore>

////////////////////////////////////////////////////////////////////////////////
///
/// \namespace GA \brief Global namespace for GeoAIDA.
/// \namespace GA::IE \brief Namespace for image engine related code
///
////////////////////////////////////////////////////////////////////////////////

namespace GA{namespace IE{

//--- Constants --------------------------------------------------------------//
const quint16 REQUEST_PART_OF_IMAGE	= 1;            ///< Signal to ask server
                                                    ///  for getPartOfImage
                                                    ///  method
const quint16 REQUEST_SETUP_SERVER	= 2;            ///< Signal to ask server
                                                    ///  for setup method
const quint16 REQUEST_SHUTDOWN_SERVER  = 3;         ///< Signal to ask server
                                                    ///  to shut itsself down
const quint16 REQUEST_RETURN_VALUE_ACCEPT = 4;      ///< Signal from server 
                                                    ///  showing that request
                                                    ///  is accepted
const quint16 REQUEST_RETURN_VALUE_WRONG_PARAM = 5; ///< Signal from server
                                                    ///  showing that a wrong
                                                    ///  number of parameters
                                                    ///  was transmitted
const quint16 REQUEST_ADD_IMAGE	= 6;                ///< Signal to ask server
                                                    ///  for addImage
                                                    ///  method
const quint16 REQUEST_ADD_IMAGES= 7;                ///< Signal to ask server
                                                    ///  for addImages
                                                    ///  method

//--- Functions --------------------------------------------------------------//
const char* constToString(const quint16& nConst);

}}

#endif
