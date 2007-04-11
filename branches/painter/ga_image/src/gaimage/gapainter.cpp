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
void Painter::fillPolygon(const PointArray& points)
{
	IndexArray			indices;
	std::vector<Edge>	edges;
	std::vector<Edge>	active_edges;

	// create index array
	for (int i=0; i<points.size(); ++i)
	{
		indices.push_back(i);
	}
	// sort points by their y coordinate
	qSortPointsY(points, indices, 0, points.size()-1);

	// sort edges by their topmost point
	for (int i=0; i<indices.size()-1; ++i)
	{
		Point prev(0,0);
		Point next(0,0);

		// catch overflow
		if (0 == indices[i])
			prev=points.back();
		else
			prev=points[indices[i]-1];
		if (points.size()-1 == indices[i])
			next=points[0];
		else
			next=points[indices[i]+1];
		
		std::cout << "(" << points[indices[i]].x() << ", " << points[indices[i]].y() << ") -- " <<
					"Prev: (" << prev.x() << ", " << prev.y() << ") -- " <<
					"Next: (" << next.x() << ", " << next.y() << ")" << std::endl;
	}

// 	edges.push_back(Edge());
}

// ////////////////////////////////////////////////////////////////////////////////
// ///
// /// \brief method to sort points by their y position using quicksort
// ///
// /// \param points array of points to be sorted
// /// \param indices array of indices for sorted point array
// ///
// ////////////////////////////////////////////////////////////////////////////////
// void Painter::qSortPointsY(PointArray& points, IndexArray& indices, const int& min, const int& max)
// {
// 	if (min < max)
// 	{
// 		int py = points[max].y();
// 
// 		int i = min-1;
// 		int j = max;
// 		while (true)
// 		{
// 			do ++i; while (points[i].y()< py);
// 			do --j; while (points[j].y()> py);
// 			if (i<j)
// 			{
// 				Point p = points[i];
// 				points[i] = points[j];
// 				points[j] = p;
// 			}
// 			else break;
// 		}
// 		Point p = points[i];
// 		points[i] = points[max];
// 		points[max] = p;
// 
// 		qSortPointsY(points, indices, min, i-1);
// 		qSortPointsY(points, indices, i+1, max);
// 	}
// }

////////////////////////////////////////////////////////////////////////////////
///
/// \brief method to sort points by their y position using quicksort
///
/// \param points array of points to be sorted
/// \param indices array of indices for sorted point array
///
////////////////////////////////////////////////////////////////////////////////
void Painter::qSortPointsY(const PointArray& points, IndexArray& indices, const int& min, const int& max) const
{
	if (min < max)
	{
		int py = points[indices[max]].y();

		int i = min-1;
		int j = max;
		while (true)
		{
			do ++i; while (points[indices[i]].y() < py);
			do --j; while (points[indices[j]].y() > py);
			if (i<j)
			{
				unsigned int k = indices[i];
				indices[i] = indices[j];
				indices[j] = k;
			}
			else break;
		}
		unsigned int k = indices[i];
		indices[i] = indices[max];
		indices[max] = k;

		qSortPointsY(points, indices, min, i-1);
		qSortPointsY(points, indices, i+1, max);
	}
}

} // namespace Ga
