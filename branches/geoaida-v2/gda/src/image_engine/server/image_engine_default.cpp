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
/// \file		image_engine_default.cpp
/// \brief		Implementation of class "ImageEngineDefault"
///
/// \date		2009-05-19
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "ImageEngineDefault"

using namespace GA::IE;

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to receive a part of an image
///
/// \param InputImage Path and file name of input image
/// \param GeoWest Geo coordinate west of part to be cut
/// \param GeoNorth Geo coordinate north of part to be cut
/// \param GeoEast Geo coordinate east of part to be cut
/// \param GeoSouth Geo coordinate south of part to be cut
/// \param FileName File name
///
/// \return Getting the image part succeeded/failed
///
///////////////////////////////////////////////////////////////////////////////
bool ImageEngineDefault::getPartOfImage(QString InputImage,
                                        float GeoWest, float GeoNorth,
                                        float GeoEast, float GeoSouth,
                                        QString FileName
                                        ) const
{
    std::cout << "ImageEngineDefault: getPartOfImage requested" << std::endl;
    
    std::cout << "ImageEngineDefault: Parameters:" << std::endl;
    std::cout << "  InputImage = " << InputImage.toStdString() << std::endl;
    std::cout << "  GeoWest    = " << GeoWest << std::endl;
    std::cout << "  GeoNorth   = " << GeoNorth << std::endl;
    std::cout << "  GeoEast    = " << GeoEast << std::endl;
    std::cout << "  GeoSouth   = " << GeoSouth << std::endl;
    std::cout << "  FileName   = " << FileName.toStdString() << std::endl;
    
    return true;
}
