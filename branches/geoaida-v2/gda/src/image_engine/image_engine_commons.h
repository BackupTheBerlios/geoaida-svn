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

#include <QObject>
#include <string>

namespace GA{namespace IE{

//--- Constants --------------------------------------------------------------//
const quint16 REQUEST_PART_OF_IMAGE	= 1;
const quint16 REQUEST_SETUP_SERVER	= 2;
const quint16 REQUEST_RETURN_VALUE_ACCEPT = 3;
const quint16 REQUEST_RETURN_VALUE_WRONG_PARAM = 4;

//--- Functions --------------------------------------------------------------//
const std::string constToString(const quint16& nConst);

}}

#endif