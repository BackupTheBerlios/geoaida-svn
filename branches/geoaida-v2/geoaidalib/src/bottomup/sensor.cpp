/***************************************************************************
                          gasensor.cpp  -  description
                             -------------------
    begin                : Mon Jan 22 2001
    copyright            : (C) 2001 by j�rgen b�ckner
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


#include "sensor.h"

using namespace BottomUpLib;

/** return label   */
unsigned long int Sensor::id(void) {
  return id_;
}

/** return node pointer   */
Node* Sensor::node(void) {
  return node_;
}

/** set node pointer   */
void Sensor::node(Node* n){
  node_=n;
  id_ =node_->value("id").toInt(); //get region ID
}


