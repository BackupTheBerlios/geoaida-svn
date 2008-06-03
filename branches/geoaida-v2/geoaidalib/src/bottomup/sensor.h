/***************************************************************************
                          gasensor.h  -  description
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

#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include "Region"
#include "Image"

namespace BottomUp {
class GaBase;

/**
  *@author j�rgen b�ckner
  *basic class for all sensor classes
  *vitual function calc must be overloaded
  */

class Sensor {
public: 
  Sensor() {id_=0;};
  virtual ~Sensor(){;};
  /** calculate the attribute 's_name' and insert the result in the 'node'*/
  virtual void calc(QString s_name="") {};
	
  /** return label nr. */
  unsigned long int id();
  /** return node pointer   */
  Node* node();
  /** set node pointer */
  void node(Node*);


  /** set image pointer   */
  virtual void image(Ga::Image& img)=0;
#ifdef DONT_USE_GAIMAGE
  /** return image pointer   */
  virtual GaBase* image() {return 0;};
  /** set image pointer   */
  virtual void image(GaBase*) {};
  virtual void readImage(QString filename) {};
#endif
protected: // Protected attributes
	/** label type */
	unsigned long int id_;
	/** pointer to node - to set new values and to get the known values */
	Node* node_;
};

//ONLY TEST
class op0815 : public Sensor {
public:
	op0815(){;};
	virtual ~op0815(){;};
	virtual void calc(QString str) {
		QString *val = new QString;
		val->setNum(4242.42);
	};
};

} // namespace BottomUp
#endif
