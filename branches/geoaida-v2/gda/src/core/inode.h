/***************************************************************************
                          inode.h  -  description
                             -------------------
    begin                : Thu Sep 28 2000
    copyright            : (C) 2000 by Martin Pahl
    email                : pahl@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * $Source: /data/cvs/gda/gda/core/inode.h,v $
 * $Revision: 1.32 $
 * $Date: 2002/05/13 10:17:42 $
 * $Author: gerhards $
 * $Locker:  $
 */

#ifndef INODE_H
#define INODE_H

#include "GeoImage"
#include "GeoImageList"
#include "GNode"
#include "SNode"
#include "Analysis"

enum IType
{ TRASH, MI, IH, CI, PI, HI, TD_ERROR, BU_ERROR, TD_ABORTED, BU_ABORTED };  //analysis status of the instance INode
enum EState
{ TD, BU };                     //exec. status TopDown BottomUp

//class SNode;
class Analysis;

class INode:public TreeNode < INode, GNode >
{
public:
  static const char *const iTypeName[];
  INode(SNode * node);
  /** Copy constructor */
  INode(INode & node);
  /** Construct an iNode from reading a file with given parser. */
  INode(MLParser & parser);
  virtual ~INode();
  /** initialize inode */
  virtual void init();
  /** Returns a new INode */
  virtual GNode *newNode();
  /** Make a copy of this INode */
  virtual GNode *copy();
  /** returns true if this class is a cname */
  virtual bool isA(QString cname);
  /** Link a sNode to this iNode. */
  bool linkSNode(SNode * parent);
  /** This method is called, when a task is finished for this inode */
  void taskFinished(int pid, int status, QString cmd);
  /** Sets the SNode this INode is type of */
  void sNode(SNode * node);
  /** returns the corresponding SNode */
  SNode *sNode();
  /** Sets the filename for operator output    */
  void output(QString val);
  /** Gets the filename for operator output    */
  QString & output();
  /** set the analysis status of the INode {TRASH, MI, IH, CI, PI, HI}*/
  void status(IType st)
  {
    status_ = st;
    attribute("status", iTypeName[st]);
  };
  /** return the analysis status of the INode {TRASH, MI, IH, CI, PI, HI} */
  IType status() const
  {
    return status_;
  };
  /** indicates if children of this temporal INode node are under temporal processing (top-down) */
  bool temporalProcessing(void)
  {
    return temporalProcessing_;
  };
  /** Sets the pointer to the analyze class */
  static void analysis(Analysis * an)
  {
    analysis_ = an;
  };
  /** Get the pointer to the analyze class */
  static Analysis *analysis(void)
  {
    return analysis_;
  };

  /** start the analyze at the root node*/
  void startAnalyze()
  {
    childTopDown();
  };
  /** set the exec. status of the instace {TopDown BottomUp} */
  void execState(EState es)
  {
    execState_ = es;
  };
  /** return the exec. status of the instace {TopDown BottomUp} */
  EState execState(void)
  {
    return execState_;
  };
  /** call the TopDown Operator of the SNode  */
  void execTopDown(bool first = FALSE);
  /** Execute the bottomUp operator */
  void execBottomUp();
  /** call the TemporalTopDown operator to create the initial hyphoteses for the conceptual children of "this" ("this" is a temporal node). */
  void execTemporalTopDown();
  /** Execute the temporalBottomUp operator */
  void execTemporalBottomUp();  
  /** decrement the aktiv counter * aktiv TopDown child operators
    * -1 -> ERROR (aktivcount < 0)
    *  0 -> aktivcount == 0
    *  1 -> aktivcount > 0 */
  int decrementCount();
  /** increment the aktiv counter  */
  void incrementCount(int c = 1);
  /** Sets the label image or returns the label image if image=0 */
  GeoImage *labelImage(GeoImage * image = 0);
  /** Sets up the region description for this inode using the available information e.g. labelimage */
  void setGeoRegion(float geoWest = 0.0, float geoNorth = 0.0, float geoEast =
                    0.0, float geoSouth = 0.0, INode* parent=0);
  /** for preparing the result label image every inode gets a new id */
  int setNewID(int id);
  /** merge inodes labelimage into the result image */
  void mergeResultImage(GeoImage & resultImg);
  /** returns the path to this inode */
  QString path();
  /** set flag to handle min and max restriction of snode definition */
  void truncation(bool);
  /** get flag to handle min and max restriction of snode definition */
  bool truncation(void);
protected:                     // Protected attributes
  /** handle the results of the BottomUp operator
    * pid ==  0 -> no operator or node have no holistic operator
    * pid == -1 -> can't start process
    * return 0 - delete 'this'
    * remember: a node without SNode is a "Trash-Node"
    * this node must have children
    */
  int evalBottomUp(int pid);
  /** handle the results of the temporal BottomUp operator
    * pid ==  0 -> no operator or node have no holistic operator
    * pid == -1 -> can't start process
    * return -1 - error
    * return  0 - delete 'this'
    * return  2 - processing
    * this node (SNode) must have children
    */
  int evalTemporalBottomUp(int pid);
  /** handle the results of the TopDown operator
    * pid ==  0 -> no operator or node have no holistic operator
    * pid == -1 -> can't start process
    */
  int evalTopDown(int pid);
  /** handle the results of the TemporalTopDown operator
    * pid ==  0 -> no operator or node have no holistic operator
    * pid == -1 -> can't start process
    */
  int evalTemporalTopDown(int pid);
  /** call the TopDown operator for all childs (of this)*/
  bool childTopDown(bool first = TRUE);
  /** call the TopDown operator for all (conceptual) children of a temporal node*/
  bool temporalChildTopDown(bool  first = TRUE);
  /** return TRUE if the Node is leaf node of the net otherwise return FALSE  */
  bool isLast();
  /** return TRUE if the Node is the root node of the net otherwise return FALSE  */
  bool isRoot();
  
  /** pointer to the SNode this INode is type of */
  SNode *sNode_;
  /** Holds the filename for operator output */
  QString output_;
  /**analysis status of the instace {TRASH, MI, IH, CI, PI, HI}*/
  IType status_;
  /**exec. status of the instace {TopDown BottomUp}*/
  EState execState_;
  /** counter for the aktive childs (TopDown)*/
  unsigned int aktivcount_;
  /** label image showing the position of this instance node */
  GeoImage *labelImage_;
  /** pointer to the analyze */
  static Analysis *analysis_;
  /** anzahl der nodes die bei td noch ausgef�hrt werden m�ssen. */
  /** number of nodes not yet processed in top-down step. */
  int childcount_;
  /** number of child temporal nodes already processed. Only used for parents of temporal nodes. */
  int temporalCount_;
  /** anzahl der nodes die bei td f�r die aktuelle priorit�t noch ausgef�hrt werden m�ssen. */
  /** number of nodes of the current priority not yet processed in top-down step. */
  int ordercount_;
  /** Memo f�r die Priorit�t, wird von 0 ab hochgez�hlt */
  /** priority counter, starts from 0 */
  int aktivorder_;
  /** flag to handle min and max restriction of snode definition */
  bool truncation_;
  /** childlist of 'this' used in childTopDown */
  QList < SNode* > *childList_;
  /** flag to indicate that children of a temporal node will actually undergo temporal processing  */
  bool temporalProcessing_;
  /** tempChildlist of 'this' used in temporalChildTopDown */
  QList <INode*> *temporalChildList_;

};

#endif
