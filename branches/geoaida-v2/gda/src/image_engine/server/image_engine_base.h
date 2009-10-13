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
/// \file		image_engine_base.h
/// \brief		Prototype of class "ImageEngineBase"
///
///	\date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_ENGINE_BASE_H
#define __IMAGE_ENGINE_BASE_H

//--- Standard header --------------------------------------------------------//
#include <iostream>

//--- Program header ---------------------------------------------------------//

//--- Misc header ------------------------------------------------------------//
#include <QString>
// #include <QTcpServer>
// #include <QTcpSocket>

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Class providing the common interface for image engine implementations
/// 
////////////////////////////////////////////////////////////////////////////////
class ImageEngineBase
{

	public:

		//--- Constructor / Destructor----------------------------------------//
		ImageEngineBase();
		virtual ~ImageEngineBase(){}

		//--- Methods --------------------------------------------------------//
		
    	//--- Constant Methods -----------------------------------------------//
		virtual void getPartOfImage(QString InputImage,
									float GeoWest, float GeoNorth,
									float GeoEast, float GeoSouth,
									QString FileName
									) const = 0;

	private:

		//--- Private Variables ----------------------------------------------//
};

#endif
