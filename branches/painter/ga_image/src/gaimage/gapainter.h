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
#include <list>
#include <vector>

//--- project header ---------------------------------------------------------//
#include "gaimage.h"
#include "gapoint.h"

namespace Ga
{

typedef std::vector<Ga::Point> PointArray;
typedef std::vector<unsigned int> IndexArray;

////////////////////////////////////////////////////////////////////////////////
///
/// \brief class for drawing in an image
///
////////////////////////////////////////////////////////////////////////////////
class Painter
{
	public:
		//--- constructor/destructor -----------------------------------------//
		Painter(Image& img, int channel=0);

		//--- methods --------------------------------------------------------//
		void drawPolygon(const PointArray& points);
		void fillPolygon(const PointArray& points);

	protected:
		Image&	img_;		///< image that is used for all operations
		int		channel_;	///< channel used for all operations on the image

	private:

		class Edge
		{
			public:

				Edge(int _y_min, int _y_max, double _dx_dy):
					y_min(_y_min),
					y_max(_y_max),
					dx_dy(_dx_dy),
					x(0.0){};

				int		y_max;
				int		y_min;
				double	dx_dy;
				double	x;
				
		};

		void qSortPointsY(const PointArray&, IndexArray&, const int&, const int&) const;

		friend bool EdgeSortX(const Edge&, const Edge&);
};

bool EdgeSortX(const Painter::Edge&, const Painter::Edge&);


} // namespace Ga

#endif
