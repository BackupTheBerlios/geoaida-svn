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
	IndexArray indices;

	// create index array
	for (int i=0; i<points.size(); ++i)
	{
		indices.push_back(i);
	}
	// sort points by their y coordinate
	qSortPointsY(points, indices, 0, points.size()-1);

	// start at the topmost point
	for (int i=0; i<points.size(); ++i)
	{
		std::cout << points[indices[i]].y() << std::endl;
	}
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
// // 		int py = points[indices[max]].y();
// 		int py = points[max].y();
// 
// 		int i = min-1;
// 		int j = max;
// 		while (true)
// 		{
// // 			do ++i; while (points[indices[i]].y() < py);
// // 			do --j; while (points[indices[j]].y() > py);
// 			do ++i; while (points[i].y()< py);
// 			do --j; while (points[j].y()> py);
// 			if (i<j)
// 			{
// // 				unsigned int k = indices[i];
// // 				indices[i] = indices[j];
// // 				indices[j] = k;
// 				Point p = points[i];
// 				points[i] = points[j];
// 				points[j] = p;
// 			}
// 			else break;
// 		}
// // 		unsigned int k = indices[i];
// // 		indices[i] = indices[max];
// // 		indices[max] = k;
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
void Painter::qSortPointsY(PointArray& points, IndexArray& indices, const int& min, const int& max)
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
