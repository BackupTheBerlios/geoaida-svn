/***************************************************************************
                          neighbour.cpp  -  description
                             -------------------
    begin                : Thu May 30 2002
    copyright            : (C) 2002 by jrgen bckner
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

/** group nodes with centroid distance < max [m]
input: [nodelist1][nodelist2][value]
output: [stack of lists] */

//	#define DEBUGMESSAGES

#include "neighbour.h"
#include "StackElement"
#include "StackElementNodeList"
#include "StackElementNumber"
#include "StackElementString"
#include "StackElementStack"
#include "StackElementString"
#include "RegionSensor"
#include "Node"

using namespace BottomUpLib;

class GeoPosition {//help class to calc overlap area
  public:
    GeoPosition(Node* n,  int llx, int lly, int urx, int ury, float area) {
      node_= n;
		llx_= llx;
		lly_= lly;
		urx_= urx;
		ury_= ury;
		area_= area;
		//center of bounding box
		bbc_= Point2D ( (int) (((urx_ - llx_)/2)+llx_) , (int) (((lly_ - ury_)/2)+ury_) );
  }

  bool bbOverlap (const GeoPosition &pos, QString dir, float max_diff){
		// pos.Schatten
    // float 	lmax= sqrt(area_/2.0),
		float lmax= sqrt(0.25*area_),
//		float lmax= sqrt(0.05*area_),
					sundir, //	direction of sunbeam;
					dir_diff,
					max_dist=4.5;
		if (dir.compare("S") == 0)	// sun from south
			sundir= 90.0;
		else if (dir.compare("SW") == 0)	// sun from south-west
			sundir= 45.0;
		else if (dir.compare("SE") == 0)	// sun from south-east
			sundir= 135.0;
		else if (dir.compare("W") == 0)	// sun from west
			sundir= 0.0;
		else if (dir.compare("E") == 0)	// sun from east
			sundir= 180.0;

		//	position shade <-> roof
		float dist= (float) bbc_.distance(pos.bbc_);
		float dirvalue= (float) bbc_.direction(pos.bbc_);

					#ifdef DEBUGMESSAGES
					cout<<"Mittelpunkt Dach XXXXX ("<<bbc_.x()<<" "<<bbc_.y()<<")"<<endl;
					cout<<"Mittelpunkt Schatten ("<<pos.bbc_.x()<<" "<<pos.bbc_.y()<<")"<<endl;
					cout<<"dist="<<dist<<" lmax="<<lmax<<endl;		
		      cout<<"dir="<<dir<<endl;		
					#endif //DEBUGMESSAGES


		//	check distance shade <-> roof
		if (dist <= max_dist*lmax)	{
			//	check size of roof and shade
			if ( (area_ / pos.area_) >= 1.0){				
				//	check direction shade <-> roof
				dir_diff= abs( (int) (dirvalue-sundir));
				if (dir_diff > 180.0)
	      	   dir_diff= 360.0 - dir_diff;
				if (dir_diff <= max_diff){
					return (1);									
					}
				}
          }
		#ifdef DEBUGMESSAGES
		cout<<"vor Rückgabe bb_overlap"<<endl;
		#endif
	  	return (0);
  }

    QString key() {return key_;}
    void key(QString k){key_ = k;}
    Node* node(){return node_;}
    QString key_;

	 int llx_, lly_, urx_, ury_;
	 float area_;
    Node* node_;
	Point2D bbc_;
};

bool BottomUpLib::neighbour(Stack& stack){
  StackElement *el1=0;
  StackElement *el2=0;
  StackElement *el3=0;
  StackElement *el4=0;

  try {
    if (stack.count()<4) throw Stack::CleanUp(false); //	sind noch genug Werte auf dem stack?
    el1=stack.pop(); 	//Abstandswert vom stack holen
    el2=stack.pop(); 	//Richtung vom stack holen
    el3=stack.pop(); 	//nodelist Schatten vom stack holen
    el4=stack.pop(); 	//nodelist Dach vom stack holen
	  // Typen testen
    if (el1->type()!=StackElement::NUMBER || el2->type()!=StackElement::STRING || el3->type()!=StackElement::NODELIST || el4->type()!=StackElement::NODELIST)
		throw Stack::CleanUp(false);
  	//	Werte holen
    float maxdist=((StackElementNumber*)el1)->data();		//	max eudist
    QString dir=((StackElementString*)el2)->data();			//	Sonnenrichtung
    NodeList& nl2=((StackElementNodeList*)el3)->data();	//	Schatten
    NodeList& nl1=((StackElementNodeList*)el4)->data();	//	Daecher

	#ifdef DEBUGMESSAGES
	cout<<"in function neighbour"<<endl;		
	#endif

    {
      RegionSensor *rs1 = new RegionSensor();
      nl1.calcForSelect("ALL", "llx", rs1);
      nl1.calcForSelect("ALL", "lly", rs1);
      nl1.calcForSelect("ALL", "urx", rs1);
      nl1.calcForSelect("ALL", "ury", rs1);
      nl1.calcForSelect("ALL", "area", rs1);
       delete rs1;
      RegionSensor *rs2 = new RegionSensor();
      nl2.calcForSelect("ALL", "llx", rs2);
      nl2.calcForSelect("ALL", "lly", rs2);
      nl2.calcForSelect("ALL", "urx", rs2);
      nl2.calcForSelect("ALL", "ury", rs2);
      nl2.calcForSelect("ALL", "area", rs2);
      delete rs2;
     }
    Stack* st= new Stack;  // Ergebnis stack anlegen

    QHash<QString,Node*>::iterator it1=nl1.begin();
	// nodelist1 durchlaufen Daecher
    while ( it1!=nl1.end() ) {
      Node *node1 = it1.value();
      NodeList *nlist = new NodeList;
      GeoPosition gp(node1, 
		     node1->getValue("llx").toInt(),  
		     node1->getValue("lly").toInt(),
		     node1->getValue("urx").toInt(), 
		     node1->getValue("ury").toInt(), 
		     node1->getValue("area").toFloat() );
      gp.key(it1.key());
      int shade=0;
      QHash<QString,Node*>::iterator it2=nl2.begin();
      // nodelist2 durchlaufen Schattenbereiche
      while ( it2!=nl2.end() ) {
      	Node *node2 = it2.value();
	if (gp.bbOverlap(GeoPosition (node2, 
				      node2->getValue("llx").toInt(),  
				      node2->getValue("lly").toInt(),
				      node2->getValue("urx").toInt(), 
				      node2->getValue("ury").toInt(),
				      node2->getValue("area").toFloat() ),
			 dir, maxdist)){
	  if (!shade){
#ifdef DEBUGMESSAGES
	    cout<<"Haus in nodelist eingehaengt"<<endl;		
#endif //DEBUGMESSAGES
//	insert node1 Haus
	    nlist->insert(node1->value("addr"), node1 );	
	    shade=1;
	  }
	  //	insert node2 Schatten
	  nlist->insert(node2->value("addr"), node2 );	
#ifdef DEBUGMESSAGES
	  cout<<"Schatten in nodelist eingehaengt"<<endl;		
#endif //DEBUGMESSAGES
	}
	++it2;
      }
      if (shade){
	st->push(new StackElementNodeList(*nlist));
#ifdef DEBUGMESSAGES
	cout<<"nodelist auf lokalen stack gepusht"<<endl;			
#endif //DEBUGMESSAGES
      }
      delete nlist;
      ++it1;
    }
    
    stack.push(new StackElementStack(*st));
#ifdef DEBUGMESSAGES
    cout<<"gefundene Haeuser mit Schatten:"<<st->count()<<" auf globalen Stack gepusht"<<endl;
#endif //DEBUGMESSAGES
    delete st;
    throw Stack::CleanUp(true);
  }
  catch(Stack::CleanUp e) {
    if (el1) delete el1;
    if (el2) delete el2;
    if (el3) delete el3;
    if (el4) delete el4;
    return e.status_;
  }
}

