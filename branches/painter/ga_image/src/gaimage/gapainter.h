////////////////////////////////////////////////////////////////////////////////
///
/// \file		gapainter.h
/// \brief		prototype for class "Painter"
///
/// \author		Torsten Büschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __GA_PAINTER_H
#define __GA_PAINTER_H

//--- standard header --------------------------------------------------------//
#include <vector>
#include <list>

//--- project header ---------------------------------------------------------//
#include "gaimage.h"
#include "gapoint.h"

namespace Ga
{

typedef std::vector<Ga::Point> PointArray;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief class for drawing in an image
///
////////////////////////////////////////////////////////////////////////////////
class Painter
{
	public:
		//--- constructor/destructor -----------------------------------------//
		Painter(Image& img);

		//--- methods --------------------------------------------------------//
		void drawPolygon(const PointArray& points);
		void fillPolygon(PointArray& points);

	protected:
		Image& img_;	///< image that is used for all operations

	private:
		void qSortPointsY(PointArray&, const int&, const int&);

		struct Edge
		{
			y_top
		};
};

} // namespace Ga

#endif
