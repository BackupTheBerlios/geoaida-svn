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
Painter::Painter(Image& img, int channel) : img_(img), channel_(channel)
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
	std::list<Edge>		active_edges;

	// create index array
	for (int i=0; i<points.size(); ++i) indices.push_back(i);

	// sort points by their y coordinate
	qSortPointsY(points, indices, 0, points.size()-1);

	// create and sort edges by their topmost point
	for (int i=0; i<indices.size()-1; ++i)
	{
		Point p=points[indices[i]];
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
		
		// insert edges in vector
		if (next.y() >= p.y())
		{
			edges.push_back(Edge(p.y(), next.y(), static_cast<double>(next.x()-p.x())/(next.y()-p.y())));
			edges.back().x = p.x();
		}
		if (prev.y() >= p.y())
		{
			edges.push_back(Edge(p.y(), prev.y(), static_cast<double>(prev.x()-p.x())/(prev.y()-p.y())));
			edges.back().x = p.x();
		}
	}

	// begin filling the polygon
	for (unsigned int i=points[indices[0]].y(); i<=points[indices.back()].y(); ++i)
	{
		// select active edges
		active_edges.clear();
		for (int j=0; j<edges.size(); ++j)
		{
			if ((edges[j].y_min <= i) && (i < edges[j].y_max))
			{
				active_edges.push_back(edges[j]);
				active_edges.back().x = static_cast<double>(i-edges[j].y_min)*active_edges.back().dx_dy + edges[j].x;
			}
		}

		// sort active edges
		active_edges.sort(EdgeSortX);

		// draw horizontal lines
		std::list<Edge>::const_iterator ci=active_edges.begin();
		while (ci != active_edges.end())
		{
			int x1 = static_cast<int>((*ci).x); ++ci;
			int x2 = static_cast<int>((*ci).x);

			img_.fillRow(i, x1, x2, 0.0, 0, true);

			if (ci != active_edges.end()) ++ci;
		}

	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// \brief compare function for std::sort algorithm
///
/// \param e1 edge 1
/// \param e2 edge 2
///
/// \return edge 1 smaller than edge 2?
///
////////////////////////////////////////////////////////////////////////////////
bool EdgeSortX(const Painter::Edge& e1, const Painter::Edge& e2)
{
  return e1.x < e2.x;
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
			do --j; while ((points[indices[j]].y() > py) && (j>0));
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
