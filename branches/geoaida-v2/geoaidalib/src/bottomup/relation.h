/***************************************************************************
                          relation.h  -  description
                             -------------------
    begin                : Fri Feb 2 2001
    copyright            : (C) 2001 by jürgen bückner
    email                : bueckner@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RELATION_H
#define RELATION_H

namespace BottomUpLib {
/**
  *@author jürgen bückner
  *basic class for all relation classes
  *vitual function calc must be overloaded
  */

class Relation {
public: 
	Relation();
	~Relation();
};

} 

#endif
