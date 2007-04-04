/* -*- mode:c++ -*- */
//*******************************************************************
//  Filename:   garegression.cpp
//  Ort:        TNT, Uni. - Hannover, Germany
//
//  Info:	Segmentation by Linear Regression
//
// Copyright (C) 2007 Christian Becker
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//           --------      
//          /     ^ /      
//         /  ^^   /--     
//        / ^ � ^^/  /
//        --------  /--
//         /  �  * /+ /
//         --------  /
//          /   ~~  /
//          --------
//
//*******************************************************************

#include "gaimage.h"
#include "gaimaget.h"
#include <gsl/gsl_matrix.h>

namespace Ga {



Image regression(const Image& imgIn,
		 const Image& maskIn,
		 const Image& learnfeatureIn1,
		 const Image& learnfeatureIn2
                    )
{
   Image result(typeid(int),imgIn.sizeX(),imgIn.sizeY()); 

   
   
   gsl_matrix_int * m = gsl_matrix_int_alloc(10, 3);
   
   return result;
}


} // namespace Ga
