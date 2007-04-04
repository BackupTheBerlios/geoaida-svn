////////////////////////////////////////////////////////////////////////////////
///
/// \file		gapainter.cpp
/// \brief		implementation for class "Painter"
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

#include "gapainter.h"

namespace Ga {

////////////////////////////////////////////////////////////////////////////////
///
/// \brief constructor
///
/// \param img image that will be used for all drawing operations
///
////////////////////////////////////////////////////////////////////////////////
Painter::Painter(Image& img) : img_(img)
{
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief method to draw a polygon, i.e. its outline
///
/// \param points array of points defining the polygon
///
////////////////////////////////////////////////////////////////////////////////
void Painter::drawPolygon(const PointArray& points)
{
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief method to draw a filled polygon
///
/// \param points array of points defining the polygon
///
////////////////////////////////////////////////////////////////////////////////
void Painter::fillPolygon(PointArray& points)
{
	// vector for sorted points, original vector needed for neighbours
	std::vector<Ga::Point> sPoints = points;

	// sort points by their y coordinate
	qSortPointsY(sPoints, 0, sPoints.size()-1);

	// start at the topmost point
	for (int i=0; i<sPoints.size(); ++i)
	{
		
		std::cout << "Processing point " << i << ": " << sPoints[i].x() << ", " << sPoints[i].y() << std::endl;

		// 		img_.set(sPoints[i].x(), sPoints[i].y(), 0);
	}
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief method to sort points by their y position using quicksort
///
/// \param points array of points to be sorted
///
/// \return vector of indices, represents sort order from low to high
///
////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned int> Painter::qSortPointsY(PointArray& points, const int& min, const int& max)
{
	std::vector<unsigned int> indices;

	for (int i=0; i<points.size(); ++i)
	{
		indices.push_back(i);
	}

	/// \todo qsort is working, now change to pure index sorting

	if (min < max)
	{
		int i = min;
		int j = max-1;
		while (i<j)
		{
			while (points[i].y()<points[max].y()) ++i;
			while (points[j].y()>points[max].y()) --j;
			if (i<j)
			{
				Point p = points[i];
				points[i] = points[j];
				points[j] = p;
			}
		}
		Point p = points[i];
		points[i] = points[max];
		points[max] = p;
		qSortPointsY(points, min, i-1);
		qSortPointsY(points, i+1, max);
	}
}

} // namespace Ga
