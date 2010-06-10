/* -*- mode:c++ -*- */
//*******************************************************************
//  Filename:   garegionsplitter.h
//  Ort:        TNT, Leibniz Uni. - Hannover, Germany
//
//  Info:	Region Finding and Exporting Interface
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


#ifndef GAREGIONSPLITTER
#define GAREGIONSPLITTER



#include <QList>

#include <cstdlib>
#include <limits>

#include <gaimage.h>
#include <garegionsplittert.h>

class QXmlStreamWriter;

namespace Ga{

/* For Ga::Image specified Region Description 
   Class RegDesc provides the basic region description for one region.
   This class should be derived to get specified region descriptions
   e.g. when having operator depending attributes which should  be 
   included to the region description.
*/
    typedef RegDescT<Image, Image> RegDesc;

    
/*
  The RegionFinder class is responsible for determining
  regions in the data image. It's the only entity knowing
  about the data image in the region finding process.
  
  By deriving from this class it is possible to 
  affect the region classification.
*/
    class RegionFinder
    {
    public:
        
        /*
          mask: Image to mask out pixels in data image
          data: Image in that pixels are compared
          level: Just differences between pixels larger than
          level have consequences.
        */
        RegionFinder(const Image& mask, const Image& data, float level)
            : data_(data),mask_(mask),level_(level) {};
        

        /*
          lpig: meaningless in this implementation
          x_center, y_center: coordinate1 in data image
          x_neighbour, y_neighbour: coordinate2 in data image

          Should return true iff the pixels at the two coordinates be in the same region.

          returns true when mask at coordinate1 equals mask at coordinate2
          AND coordinate1 in data differs from coordinate2 in data no more than level
          else false 
        */
        virtual bool operator()(const Image &lpic,
                        int x_center, int y_center,
                        int x_neighbour, int y_neighbour)
            {
                if (mask_.getInt(x_center, y_center) 
                    != mask_.getInt(x_neighbour, y_neighbour))
                    return false; // outmasked
                
                return (fabs(data_.getFloat(x_center, y_center) - 
                             data_.getFloat(x_neighbour, y_neighbour)) 
                        <= level_);
            }
                
/*
  Can be used to mask out pixels which should not 
  be in any region (eg background pixels).
  
  In this implementation it returns true iff the value in data image is greater than 1
*/
        virtual bool valid(const Image &lpic,
                   int x, int y)
            {
//                return true;
                return (data_.getFloat(x, y) > 1);
            }
        
        /*
          Can be used to give the regions names, depending
          on the value in the input image.
          
          Return empty string when the region name
          in RegionSplitterT should be used;
        */
        
        virtual QString getRegionClass(int x, int y, int channel=0){
            QString out = "";
            return out;
        }


    protected:
        const Image& data_;
        const Image& mask_;
        float level_;
    };
    
    


    QList<RegDesc> splitIntoRegions(Image &labelpic,  
                                     RegionFinder& regfind,
                                     QString regionclass,
                                     QString labelfilename="",
                                     int minsize=0, int maxsize=INT_MAX);
    QList<RegDesc> splitIntoRegions(Image &labelpic,  
                                     RegionFinder& regfind,
                                     int minsize=0, int maxsize=INT_MAX);
    
    int regionsToFile(QString filename, QList<RegDesc>& reglist);
    int regionsToFile(QXmlStreamWriter& out, QList<RegDesc>& reglist);
}

#endif
