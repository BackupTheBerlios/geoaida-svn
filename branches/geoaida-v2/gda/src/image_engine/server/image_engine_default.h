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
/// \file		image_engine_default.h
/// \brief		Prototype of class "ImageEngineDefault"
///
///	\date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_ENGINE_DEFAULT_H
#define __IMAGE_ENGINE_DEFAULT_H

//--- Standard header --------------------------------------------------------//

//--- Program header ---------------------------------------------------------//
#include "ImageEngineBase"

//--- Misc header ------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Class providing the common interface for image engine implementations
/// 
////////////////////////////////////////////////////////////////////////////////
class ImageEngineDefault : public ImageEngineBase
{

	public:

		//--- Constructor / Destructor ---------------------------------------//
		ImageEngineDefault(){}
		~ImageEngineDefault(){}

		//--- Methods --------------------------------------------------------//

		//--- Constant Methods -----------------------------------------------//
		bool getPartOfImage(QString InputImage,
							float GeoWest, float GeoNorth,
							float GeoEast, float GeoSouth,
							QString FileName
							) const;

};

#endif
