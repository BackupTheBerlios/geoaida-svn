/***************************************************************************
                          inode.cpp  -  description
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
 * $Source: /data/cvs/gda/gda/core/inode.cpp,v $
 * $Revision: 1.62 $
 * $Date: 2002/08/19 13:02:19 $
 * $Author: pahl $
 * $Locker:  $
 */

#include <assert.h>
#include "inode.h"
#include "FileIOException"
#ifdef WIN32
//! INTERIMAGE
#include <QMessageBox>
#endif


const char *const INode::iTypeName[] = {
  "TRASH",
  "MI",                  //Missing
  "IH",                  //Hypothesis
  "CI",                  //Complete Instance
  "PI",                  //Partial Instance
  "HI",                  //Complete Holistic Instance
  "TD_ERROR",
  "BU_ERROR",
  "TD_ABORTED",
  "BU_ABORTED"
};

/* Mögliche Übergänge
  IH -> PI -> CI 
  IH -> IM
  PI -> IM
  IH -> HI
*/
#define DEBUGMSG
INode::INode(SNode * node)
{
  init();
  sNode(node);
  if (sNode_) {
    attribute("name", node->GNode::name());
    attribute("class", node->GNode::name());
  }
}

/** Copy constructor */
INode::INode(INode & node):TreeNode < INode, GNode > (node)
{
  sNode_ = 0;
  sNode(node.sNode());
  execState(node.execState());
  status(node.status());
  childList_ = node.childList_;
  temporalChildList_ = node.temporalChildList_;
  temporalCount_ = node.temporalCount_;
  temporalProcessing_ = node.temporalProcessing_;
  childcount_ = node.childcount_;
  aktivcount_ = node.aktivcount_;
  ordercount_ = node.ordercount_;
  aktivorder_ = node.aktivorder_;
  truncation_ = node.truncation_;
  analysis_ = node.analysis_;
  output_ = node.output_;
  if (node.labelImage_)
    labelImage_ = node.labelImage_->shallowCopy();
  else
    labelImage_ = 0;
}

/** Construct an iNode from reading a file with given parser. */
INode::INode(MLParser & parser)
{
  init();
  read(parser);
}

INode::~INode()
{
  if (sNode_)
    sNode_->unlink();
}

/** initialize inode */
void INode::init()
{
  labelImage_ = 0;
  sNode_ = 0;
  status(HI);                   //default;
  execState(TD);                //default;
  aktivcount_ = childcount_ = ordercount_ = aktivorder_ = 0;
  truncation_ = FALSE;
  childList_ = NULL;
  temporalChildList_ = NULL;
  temporalCount_ = 0;
  temporalProcessing_ = false;
}

/** Make a copy of this INode */
GNode *INode::copy()
{
  GNode *gn = new INode(*this);
#ifdef WIN32
  if (gn == 0){
    //cout << "Out of Memory..9";
    exit(1);
  }
#endif
  return gn;
}

/** Returns a new INode */
GNode *INode::newNode()
{
  GNode *gn = new INode(0);
#ifdef WIN32
  if (gn == 0){
    //cout << "Out of Memory..10";
    exit(1);
  }
#endif
  return gn;
}

/** Returns true if  this class is a cname */
bool INode::isA(QString cname)
{
  return (cname == "INode");
}

/** Link a sNode to this iNode. */
bool INode::linkSNode(SNode * parent)
{
  bool status = true;
  if (!parent_) {
    sNode(parent);
    QList < INode* >::const_iterator it=children().constBegin();
    for (; it!=children().constEnd(); ++it) {
      INode *node = *it;
      status = status & node->linkSNode(parent);
    }
  }
  else {
    sNode(parent->findClass(attribute("class")));
    if (!sNode_)
      return false;
    QList< INode* >::const_iterator it = children().constBegin();
    for (; it!=children().constEnd(); ++it) {
      INode *node = *it;
      status = status & node->linkSNode(sNode_);
    }
  }
  return status;
}

/** This method is called, when a task is finished for this inode
  * pid ==  0 -> no operator or node have no holistic operator
  * pid == -1 -> can't start process
  */
void INode::taskFinished(int pid, int exit_state)
{
  try {
    int flag = -1;
#ifdef DEBUGMSG
    qDebug("#  INode::taskFinished (%s)(%p) pid=%d ES=%d",
           name().toLatin1().constData(), this, pid, execState());
#endif
    switch (execState()) {
    case TD:
      if (exit_state!=0) {
        analysis()->setError(true);
        status(TD_ERROR);
        analysis()->ready();
      qDebug("INode::184:clean up necessary?");
        return;
      }
      Q_CHECK_PTR(this->sNode());
      if (sNode_->temporal()) // GILSON: temporal processing
    	flag = evalTemporalTopDown(pid);  
      else
    	flag = evalTopDown(pid);
      break;
    case BU:
      if (exit_state!=0) {
        analysis()->setError(true);
        status(BU_ERROR);
        analysis()->ready();
     qDebug ( "INode::194:clean up necessary?");
        return;
      }
      Q_CHECK_PTR(this->sNode());
      if (sNode_->temporal()) // GILSON: temporal processing
    	  flag = evalTemporalBottomUp(pid);
      else
    	  flag = evalBottomUp(pid);
      break;
    }
#ifdef DEBUGMSG
    qDebug("#  INode::taskFinished: finish ... (%s) pid=%d %p flag=%d\n",
           name().toLatin1().constData(), pid, this, flag);
#endif
    switch (flag) {
    case 1:
      INode::analysis()->ready();
      break;
    case 0:
      delete this;
      break;
    }
  }
  catch(FileIOException err) {
    qDebug("INode::taskFinished(): Exception(%s) %s", name().toLatin1().constData(),
           err.what().toLatin1().constData());
//    emit analysis_->message(err.message());
  }
}

/** handle the results of the temporal BottomUp operator
  * pid ==  0 -> no operator or node have no holistic operator
  * pid == -1 -> can't start process
  * return -1 - error
  * return  0 - delete 'this'
  * return  2 - processing
  * this node (SNode) must have children
  */
int INode::evalTemporalBottomUp(int pid)
{
	// GILSON: INode cannot be root!
//QFile fDebug("debug_evalTemporal.txt");
//fDebug.open(IO_WriteOnly);
//Q3TextStream strDebug(&fDebug);
//strDebug << ".1" << endl;
	
  //Q3PtrList < INode > temporalNodeList; //list of temporal nodes
  QList < INode* > temporalNodeList; //list of temporal nodes
  //Q3PtrList < INode > *oriTemporalNodeList; //original list of temporal nodes
  QList < INode* > *oriTemporalNodeList; //original list of temporal nodes
  int temporalNodeCount; //number of temporal nodes
  //INode *tNode; // auxiliary pointer to temporal nodes
	QList<INode*>::iterator tNode;
  
  oriTemporalNodeList = &(parent()->children()); // list of temporal nodes
  //for (tNode = oriTemporalNodeList->first(); tNode != 0; tNode = oriTemporalNodeList->next())
  for (tNode = oriTemporalNodeList->begin(); tNode != oriTemporalNodeList->end(); ++tNode)
	  temporalNodeList.append(*tNode);
  temporalNodeCount = temporalNodeList.count(); // count of all temporal nodes

//strDebug << ".2 temporalNodeCount: " << temporalNodeCount << endl;

#ifdef DEBUGMSG
  qDebug("#*  INode::evalTemporalBottomUp(%s): pid=%d (%p)\n", name().toLatin1().constData(),
         pid, this);
#endif
  if (pid <= 0) {
#ifdef DEBUGMSG
    qDebug("#  (ERROR) evalTemporalBottomUp(%p): temporal BottomUp operator have no result \n",
           this);
#endif
    //! hier noch das Icon im Browser auf rot setzen (Problem)
    
    for (tNode = temporalNodeList.begin(); tNode != temporalNodeList.end(); ++tNode){
      (*tNode)->status(TRASH);
    }
    parent()->decrementCount();
    return -1;
  }
 
  for (tNode = temporalNodeList.begin(); tNode != temporalNodeList.end(); ++tNode){
//strDebug << ".3 tNode: " << tNode << endl;
	  (*tNode)->execState(BU);
	  analysis_->nodeChange(*tNode);
  }

  QList < INode* > &nodeList = sNode_->evalTemporalBottomUp(this);       //new composition
  analysis_->nodeChange(this);
   
#ifdef DEBUGMSG
  qDebug("#  (INFO)  nodeList count: %d\n", nodeList.count());
#endif
  
  if (nodeList.isEmpty()) {
#ifdef DEBUGMSG
    qDebug
      ("#  (ERROR) evalTemporalBottomUp: QList<<INode> nodeList is empty - all trash\n");
#endif
//strDebug << ".4 nodeList.isEmpty(): " << nodeList.isEmpty() << endl;
    for (tNode = temporalNodeList.begin(); tNode != temporalNodeList.end(); ++tNode){
      (*tNode)->status(TRASH);
    }
    parent()->decrementCount();
    delete & nodeList;
    return 2;
  }
   
  QListIterator < INode* > it(nodeList);
  while (it.hasNext()) {
    INode *iNode = it.next();
    analysis_->nodeChange(iNode);
    parent()->childLink(iNode);
    iNode->status(CI);
//strDebug << ".5 iNode->status(CI): " << iNode->status() << endl;
    //INode *el;
	QList<INode*>::iterator el;
    for (el = iNode->children().begin(); el != iNode->children().end(); ++el) {
//strDebug << ".6 el->status(): " << el->status() << endl;
      if ((*el)->status() <= MI)			
        iNode->status(MI);
		analysis_->nodeChange(*el);        //info to the rest of the world //GILSON: why do that? el does not change!
     }
  }
   
  INode *p = parent();          //for later access
  do {
	  tNode = temporalNodeList.begin();

//strDebug << ".7 (tNode->children()).isEmpty(): " << (tNode->children()).isEmpty() << endl;
//strDebug << ".7 parent()->aktivcount_: " << parent()->aktivcount_ << endl;
//strDebug << ".7 parent()->childcount_: " << parent()->childcount_ << endl;
//strDebug << ".7 parent()->ordercount_: " << parent()->ordercount_ << endl;
//strDebug << ".7 parent()->aktivorder_: " << parent()->aktivorder_ << endl;
//strDebug << ".8 tNode: " << tNode << endl;
//strDebug << ".8 tNode->attribute(name): " << tNode->attribute("name") << endl;
//strDebug << ".8 tNode->attribute(id): " << tNode->attribute("id") << endl;
//strDebug << ".8 this: " << this << endl;
//strDebug << ".8 this->attribute(name): " << this->attribute("name") << endl;
//strDebug << ".8 this->attribute(id): " << this->attribute("id") << endl;

	if (((*tNode)->children()).isEmpty()) {
#ifdef DEBUGMSG
	  qDebug("#  (INFO) evalTemporalBottomUp: empty list - no trash\n");  //GILSON: should indicate the node class
#endif
//strDebug << ".8 " << endl;
	  parent()->childUnlink(*tNode);
	  if ((*tNode) != this) delete (*tNode);
	}
	else {
//strDebug << ".9 " << endl;
	  (*tNode)->status(TRASH);
	}
    temporalNodeList.removeFirst();
  } while (!temporalNodeList.isEmpty());
	  
  p->decrementCount();
  delete & nodeList;

//strDebug << ".10 (this->children()).isEmpty()" << (this->children()).isEmpty() << endl;
//fDebug.close();

  if ((this->children()).isEmpty()) 
    return 0;                   // delete this;
  else
    return 2;
}

/** handle the results of the BottomUp operator
  * pid ==  0 -> no operator or node have no holistic operator
  * pid == -1 -> can't start process
  * return -1 - error
  * return  0 - delete 'this'
  * return  1 - analyze is ready
  * return  2 - processing
  * remember: a node without SNode is a "Trash-Node"
  * this node must have children
  */
int INode::evalBottomUp(int pid)
{
#ifdef DEBUGMSG
  qDebug("#*  INode::evalBottomUp(%s): pid=%d (%p)\n", name().toLatin1().constData(),
         pid, this);
#endif
  if (pid <= 0) {
#ifdef DEBUGMSG
    qDebug("#  (ERROR) evalBottomUp(%p): BottomUp operator have no result \n",
           this);
#endif
    //! hier noch das Icon im Browser auf rot setzen (Problem)
    status(TRASH);
    if (parent())
      parent()->decrementCount();
    return -1;
  }
 
  execState(BU);
   
  if (isRoot()) {               //! Debug
	
    qDebug("INode::evalBottomUp: is Root");
#ifdef WIN32
  QMessageBox::information(0,"INode","evalBottomUp: is Root",QMessageBox::Cancel);
#endif

  }
  analysis_->nodeChange(this);
  QList < INode* > &nodeList = sNode_->evalBottomUp(this);       //new composition
  analysis_->nodeChange(this);

#ifdef DEBUGMSG
  qDebug("#  (INFO)  nodeList count: %d\n", nodeList.count());
#endif
  if (isRoot()) {
    switch (nodeList.count()) {
    case 0:
#ifdef DEBUGMSG
      qDebug("#  (ERROR) No scene found(%p)\n", this);
#endif
#ifdef WIN32
  QMessageBox::warning(0,"INode","evalBottomUp: No scene found");
#endif
      return 1;
    case 1:{
#ifdef DEBUGMSG
        qDebug("#  Root node is unique (%p)\n", this);
#endif
        //alle bis auf die zurückgegebenen aus this entfernen
        {                       //! the children can actually be deleted immediately!
          INode *trashNode = new INode(*this);
#ifdef WIN32
          if (trashNode == 0){
            //cout << "Out of Memory..11";
            exit(1);
          }
#endif
          trashNode->status(TRASH);
          {                     // normally this code should do nothing because the children are already moved
            INode *el;
            while (children().count()) {
              el = (children()).first();
              trashNode->childLink(el);
              analysis_->nodeChange(el);
            }
          }
#if 0
          delete trashNode;
#else
          analysis_->setTrashRoot(trashNode);
#endif
        }
        {
          INode *el;
          INode *iNode = nodeList.first();
          update(iNode->attribList());
          labelImage(iNode->labelImage());
          el = (iNode->children()).first();
          while (el) {
            childLink(el);
            analysis_->nodeChange(el);
            el = (iNode->children()).first();
          }
          delete iNode;
        }
        delete & nodeList;
        return 1;               //=> analyze is ready
      }
    default:
#ifdef DEBUGMSG
      qDebug("#  (ERROR) Root node must be unique (%p)\n", this);
#endif
#ifdef WIN32
  QMessageBox::warning(0,"INode","evalBottomUp: Root node must be unique");
#endif
      delete & nodeList;
      return -1;
    }
  }
   
  if (nodeList.isEmpty()) {
#ifdef DEBUGMSG
    qDebug
      ("#  (ERROR) evalBottomUp: QList<<INode> nodeList is empty - all trash\n");
#endif
    status(TRASH);
    parent()->decrementCount();
    delete & nodeList;
    //return 0; // XXX TEST - fuer die Ergebnisdarstellung
    return 2;                   //=>
  }

  QList < INode* >::const_iterator it=nodeList.constBegin();
  for (; it!=nodeList.constEnd(); ++it) {
    INode *iNode = *it;
    analysis_->nodeChange(iNode);
    parent()->childLink(iNode);
    iNode->status(CI);
    QList<INode*>::iterator el;
    for (el = iNode->children().begin(); 
	 el != iNode->children().end();
         ++el) {
      if ((*el)->status() <= MI)
        iNode->status(MI);
      analysis_->nodeChange(*el);        //info to the rest of the world //GILSON: why do that? el does not change!
     }
  }
   
  INode *p = parent();          //for later access
  if ((this->children()).isEmpty()) {
#ifdef DEBUGMSG
    qDebug("#  (INFO) evalBottomUp: empty list - no trash\n");  //
#endif
    parent()->childUnlink(this);
    p->decrementCount();
    delete & nodeList;
    return 0;                   // delete this;
  }
  else {
    //this->sNode(0); //make a TRASH Node; -> has no SNode
    this->status(TRASH);
    p->decrementCount();
    delete & nodeList;
    //return 0; // XXX TEST - fuer die Ergebnisdarstellung
    return 2;
  }
   
}

/** handle the results of the TopDown operator
  * pid ==  0 -> no operator or node have no holistic operator
  * pid == -1 -> can't start process
  * never call for "root" node (this is not root node)
  * this can be "last" node
  */
int INode::evalTopDown(int pid)
{
#ifdef DEBUGMSG
  qDebug("#*  INode::evalTopDown(%s)(%p): pid=%d Start\n",
         name().toLatin1().constData(), this, pid);
#endif
  Q_CHECK_PTR(sNode_);
//Hier Absturz weil parent = 0
  Q_CHECK_PTR(parent());
  INode* parent=INode::parent();
  if (pid <= 0) {
#ifdef DEBUGMSG
    qDebug
      ("#  (ERROR -1/0) evalTopDown: can't start process \n  try it structural\n");
#endif
#warning Without Dummy TopDown nothing works
    if (!isLast() && !truncation())
      childTopDown();           //do it structural
    else if (isLast()) {                      //is last node
      status(MI);
      execState(BU);
      parent->childUnlink(this);      //!remove this temporary INode
      parent->decrementCount();
      return 0;
    } else { //is trash
      status(TRASH);
      execState(BU);
      parent->childUnlink(this);      //!remove this temporary INode
      parent->decrementCount();
      return 0;
    }
  }

  if (sNode_->holistic()) {     //pid > 0
    QList < INode* > iNodeList = sNode_->evalTopDown(this);
#ifdef DEBUGMSG
qDebug("##*** evalTopDown: %d(min) < %d(ist) < %d(max)",sNode()->minNumNode(),iNodeList.count(), sNode()->maxNumNode());
#endif
    if (!(sNode()->minNumNode() == 0 && sNode()->maxNumNode() == 0 )) //there are restrictions on the number of nodes
      if (iNodeList.count() < sNode()->minNumNode() || 
    		  (iNodeList.count() > sNode()->maxNumNode() && !(sNode()->maxNumNode() == 0))) { //GILSON
#ifdef DEBUGMSG
qDebug("*****  iNodeList.count %d, sNode->minNumNode %d, sNode->maxNumNode %d \n",iNodeList.count(),sNode()->minNumNode(),sNode()->maxNumNode());
#endif
        status(MI);
        execState(BU);
        truncation(TRUE);
        parent->status(TRASH);
        parent->truncation(TRUE);
        parent->childUnlink(this);      //!remove this temporary INode
        parent->decrementCount();
        return 0;
      }
#ifdef DEBUGMSG
    qDebug("#  (INFO) sNode is holistic; count new nodes: %d\n", iNodeList.count());
#endif

    if (iNodeList.isEmpty()) {
#ifdef DEBUGMSG
      qDebug("#  (warning) operator returned no result - missing Instance");
#endif
      status(MI);
      execState(BU);
      //! unlink here
      parent->childUnlink(this);      //!remove this temporary INode
      parent->decrementCount();
      return 0;
    }

    parent->incrementCount(iNodeList.count() - 1);
    parent->childUnlink(this);        //!remove this temporary INode
    QList<INode*>::iterator el;
    for (el = iNodeList.begin(); 
	 el != iNodeList.end(); 
	 ++el) {
#ifdef DEBUGMSG
      qDebug("# einhaengen: (%p) %s in %s", 
	     this,
	     (*el)->name().toLatin1().constData(), 
	     parent->name().toLatin1().constData());
#endif
      if ((*el)->isLast())
        (*el)->status(CI);  //(*el)->execState(BU); ???????????
      else
        (*el)->status(PI);
      (*el)->execState(TD); //}?????????????
      parent->childLink(*el);         //insert the new INodes
      analysis_->nodeChange(*el);
      if ((*el)->isLast())
        parent->decrementCount();
      else
        (*el)->childTopDown();     //last node have no children
    }
    //delete this; //XXXX
#ifdef DEBUGMSG
    qDebug("# einhaengen: (%p) fertig in %s",this,parent->name().toLatin1().constData());
#endif
    return 0;
  }
  else {
    qDebug
      ("#  (ERROR 1) taskFinished no operator found \n  try it structural\n");
    if (isLast())
      qDebug("#  (ERROR 1) last node must be holistic!\n");
    parent->childUnlink(this);        //!remove this temporary INode
    return 0;
  }
}

/** handle the results of the TemporalTopDown operator
  * pid ==  0 -> no operator or node have no holistic operator
  * pid == -1 -> can't start process
  */
int INode::evalTemporalTopDown(int pid)
{
#ifdef DEBUGMSG
  qDebug("#*  INode::evalTemporalTopDown(%s)(%p): pid=%d Start\n",
         name().toLatin1().constData(), this, pid);
#endif
  Q_CHECK_PTR(sNode_);
  if (pid <= 0) {
#ifdef DEBUGMSG
    qDebug
      ("#  (ERROR -1/0) evalTemporalTopDown: can't start process\n");
#endif
    analysis()->setError(true);
    status(TD_ERROR);
    analysis()->ready();
    qDebug("temporalTopDown error: can't start process. Clean up necessary?");
    return 1;
  }

  temporalProcessing_ = true;
  temporalChildList_ = sNode_->evalTemporalTopDown(this); // creates list of children for this INode
  childcount_ = temporalChildList_->count(); // count of all nodes to be processed 

/*
QFile fDebug("debug_evalTemporalTopDown.txt");
fDebug.open(IO_Append);
Q3TextStream strDebug(&fDebug);
strDebug << ".1" << endl;
strDebug << ".1 this->attribute(name): " << this->attribute("name") << endl;
strDebug << ".1 this->attribute(id): " << this->attribute("id") << endl;
strDebug << ".1 this->attribute(class): " << this->attribute("class") << endl;
strDebug << ".7 this->aktivcount_: " << this->aktivcount_ << endl;
strDebug << ".7 this->childcount_: " << this->childcount_ << endl;
strDebug << ".7 this->ordercount_: " << this->ordercount_ << endl;
strDebug << ".7 this->aktivorder_: " << this->aktivorder_ << endl;
strDebug << ".7 sNode_->temporal(): " << sNode_->temporal() << endl;
strDebug << ".7 temporalProcessing_: " << temporalProcessing_ << endl;
fDebug.close();
*/
  temporalChildTopDown(true);

  return 2;
}

/** execTemporalTopDown - Calls the TemporalTopDown operator to create the initial hyphoteses for the conceptual children of "this" ("this" is a temporal node). */
void INode::execTemporalTopDown()
{
//QFile fDebug("debug_execTemporalTopDown.txt");
//fDebug.open(IO_Append);
//Q3TextStream strDebug(&fDebug);
//strDebug << ".1" << endl;
//strDebug << ".1 this->attribute(name): " << this->attribute("name") << endl;
//strDebug << ".1 this->attribute(id): " << this->attribute("id") << endl;
//strDebug << ".1 this->attribute(class): " << this->attribute("class") << endl;
//strDebug << ".7 this->aktivcount_: " << this->aktivcount_ << endl;
//strDebug << ".7 this->childcount_: " << this->childcount_ << endl;
//strDebug << ".7 this->ordercount_: " << this->ordercount_ << endl;
//strDebug << ".7 this->aktivorder_: " << this->aktivorder_ << endl;
//strDebug << ".7 sNode_->temporal(): " << sNode_->temporal() << endl;
//strDebug << ".7 temporalProcessing_: " << temporalProcessing_ << endl;
//fDebug.close();

#ifdef DEBUGMSG
  qDebug("#*  INode::execTemporalTopDown(%s)(%p): Start\n", 
	 name().toLatin1().constData(),this);
#endif
  if (analysis()->error()) {
    status(TD_ABORTED);
    return;
  }
  execState(TD);
  Q_CHECK_PTR(this->sNode());
  if (sNode_->holistic())
    sNode_->execTemporalTopDownOp(this); //start Temporal TD
  else { //no operator, semantic network definition error
      analysis()->setError(true);
      status(TD_ERROR);
      analysis()->ready();
      qDebug("TemporalChildTopDown error: clean up necessary?");
      return;
   }
}

/** temporalChildTopDown - Calls the TopDown operator for all (conceptual) children of "this" ("this" is not a leaf node). */
/** return:
  0 - all subnodes have been handled, or node is trash
  1 - more subnodes must be handled
/** first:
  0 - FALSE - call to handle nodes with next priority order
  1 - TRUE (default) - first call for this node
*/
bool INode::temporalChildTopDown(bool first)
{
	
#ifdef DEBUGMSG
  qDebug("#  INode::temporalChildTopDown (%s) cont_td (this %p) first: %d", 
	 name().toLatin1().constData(), 
	 this, 
	 first);
    qDebug("##(xxx) INode::temporalChildTopDown (%s) (this %p) #childcount %d, ordercount %d, aktivorder %d, aktivcount %d, truncation %d",
	   name().toLatin1().constData(), 
	   this, 
	   childcount_,
	   ordercount_,
	   aktivorder_,
	   aktivcount_,
	   truncation());
#endif
  //INode *inode; // auxiliary pointer
  
  QList<INode*>::iterator inode;
  
  if (first) {// first call of childTopDown for this INode
    aktivorder_=-1; // priority (order) of nodes to be processed 
    ordercount_=0;
  } else { // recursive call of childTopDown for this INode
    if (aktivcount_ > 0 || ordercount_ > 0 ) return 1; // there are more subnodes to process
    if (childcount_ <= 0) return 0; //all sub nodes have been processed
    if (truncation()) return 0; //all sub nodes have been processed, or node is trash
  }

  // At this point: (ordercount_ == 0) && (childcount_ > 0)
  while (ordercount_ == 0) {
    aktivorder_ ++;
    for (inode = temporalChildList_->begin(); inode != temporalChildList_->end(); ++inode)
      if((*inode)->sNode_->order() == aktivorder_) ordercount_++; // counts nodes with same priority (order)
  }
#ifdef DEBUGMSG
    qDebug("## INode::temporalChildTopDown (%s) (this %p) #childs %d, ordercount %d, aktivorder %d, aktivcount_ %d",
	   name().toLatin1().constData(), 
	   this, 
	   childcount_, 
	   ordercount_, 
	   aktivorder_,
	   aktivcount_);
#endif

  for (inode = temporalChildList_->begin(); inode != temporalChildList_->end(); ++inode) {
    if ((*inode)->sNode_->order() == aktivorder_) { //for each child concept (of the current order), creates a new hypothesis
      incrementCount(); //einer mehr in der queue -> siehe decrementCount() /GILSON: increments aktivcount_
      childcount_--; ordercount_--; //aktuelle wird gleich bearbeitet
      (*inode)->status(HI);
      (*inode)->execState(TD);
      childLink(*inode);         // mount new node in the tree
      (*inode)->execTopDown(first);     //start topdown operator
      analysis_->nodeChange(0); //info to the rest of the world
    }
  }
  return 1;
}

/** childTopDown - Calls the TopDown operator for all (conceptual) children of "this" ("this" is not a leaf node). */
/** return:
  0 - all subnodes have been handled, or node is trash
  1 - more subnodes must be handled
/** first:
  0 - FALSE - call to handle nodes with next priority order
  1 - TRUE (default) - first call for this node
*/
bool INode::childTopDown(bool first)
{
#ifdef DEBUGMSG
  qDebug("#  INode::childTopDown (%s) cont_td (this %p) first: %d", 
	 name().toLatin1().constData(),
	 this, first);
  qDebug("##(xxx) INode::childTopDown (%s) (this %p) #childcount %d, ordercount %d, aktivorder %d, aktivcount %d, truncation %d",
	 name().toLatin1().constData(), 
	 this, 
	 childcount_,
	 ordercount_,
	 aktivorder_,
	 aktivcount_,
	 truncation());
#endif

//QFile fDebug("debug_childTopDown.txt");
//fDebug.open(IO_Append);
//Q3TextStream strDebug(&fDebug);
//strDebug << ".1" << endl;
//strDebug << ".1 this->attribute(name): " << this->attribute("name") << endl;
//strDebug << ".1 this->attribute(id): " << this->attribute("id") << endl;
//strDebug << ".1 this->attribute(class): " << this->attribute("class") << endl;
//strDebug << ".7 this->aktivcount_: " << this->aktivcount_ << endl;
//strDebug << ".7 this->childcount_: " << this->childcount_ << endl;
//strDebug << ".7 this->ordercount_: " << this->ordercount_ << endl;
//strDebug << ".7 this->aktivorder_: " << this->aktivorder_ << endl;
//strDebug << ".7 sNode_->temporal(): " << sNode_->temporal() << endl;
//strDebug << ".7 temporalProcessing_: " << temporalProcessing_ << endl;
//strDebug << ".7 first: " << first << endl;
//fDebug.close();
    
  //SNode *el; // auxiliary pointer
  QList<SNode*>::iterator el;
  if (first) {// first call of childTopDown for this INode
    childList_ = &(sNode_->children()); // creates list of children for this INode
    childcount_ = childList_->count(); // count of all nodes to be processed 
    aktivorder_=-1; // priority (order) of nodes to be processed 
    if (childcount_ == 0) { // no sub nodes: leaf node
      aktivcount_ = 0; // indicates no more top-down activity for this INode (ok to start bottom-up processing)
      execBottomUp(); //executes bottom-up operator
      return 0;
    }
  } else { // recursive call of childTopDown for this INode
    if (aktivcount_ > 0 || ordercount_ > 0 ) return 1; // there are more subnodes to process
    if (childcount_ <= 0) return 0; //all sub nodes have been processed
    if (truncation()) return 0; //all sub nodes have been processed, or node is trash
  }

  // At this point: (ordercount_ == 0) && (childcount_ > 0)
  while (ordercount_ == 0) {
    aktivorder_ ++;
    for (el = childList_->begin(); el != childList_->end(); ++el)
      if((*el)->order() == aktivorder_) ordercount_++; // counts nodes with same priority (order)
  }

#ifdef DEBUGMSG
    qDebug("## INode::childTopDown (%s) (this %p) #childs %d, ordercount %d, aktivorder %d, aktivcount_ %d",
	   name().toLatin1().constData(), 
	   this, 
	   childcount_, 
	   ordercount_, 
	   aktivorder_,
	   aktivcount_);
#endif

  INode *inode;
  for (el = childList_->begin(); el != childList_->end(); ++el) {
    if (truncation()) { //no longer meets (min, max) conditions
      aktivcount_ = 0;
      execBottomUp();
      return 0;
    }
    if ((*el)->order() == aktivorder_) { //for each child concept (of the current order), creates a new hypothesis
      incrementCount(); //einer mehr in der queue -> siehe decrementCount() /GILSON: increments aktivcount_
      childcount_--; ordercount_--; //aktuelle wird gleich bearbeitet
      inode = new INode(*el);    //new INode
      Q_CHECK_PTR(inode);
      inode->status(HI);
      inode->execState(TD);
      childLink(inode);         // mount new node in the tree
      inode->execTopDown(first);     //start topdown operator
      analysis_->nodeChange(0); //info to the rest of the world
    }
  }
  return 1;
}

/** call the TopDown Operator of the SNode  */
void INode::execTopDown(bool first)
{
#ifdef DEBUGMSG
  qDebug("#*  INode::execTopDown(%s)(%p): Start\n", name().toLatin1().constData(),this);
#ifdef WIN32
  QMessageBox::information(0,"INode","execTopDown",QMessageBox::Default);
#endif
#endif
  if (analysis()->error()) {
    status(TD_ABORTED);
    return;
  }
  execState(TD);
  Q_CHECK_PTR(this->sNode());
  if (sNode_->temporal()) { // GILSON: temporal processing
    if ((isLast()) || (isRoot())) { // temporal node cannot be leaf nor root
      analysis()->setError(true);
      status(TD_ERROR);
      analysis()->ready();
      qDebug ( "Semantic Net Error: Temporal node cannot be leaf nor root!");
      return;
    }
    else { // change directly to partial instance: no structural TD processing at temporal node
      // next lines correspond to a dummy topdown to generate hypothesis of temporal node
      configure(parent()->attribList());
      attribute("class", sNode_->name());
      attribute("name", sNode_->name() + QString().sprintf("_%03d", attributeInt("id")));
      labelImage_ = (parent()->labelImage_)->shallowCopy();
      status(PI);

      if (first) {// first temporal node, proceed like normal node
    	  temporalProcessing_ = false;
    	  childTopDown();
      }
      else {
    	  temporalProcessing_ = true;
    	  execTemporalTopDown();
      }
    }
  }
  else { // regular, non temporal processing
	  if (sNode_->holistic())
	    sNode_->execTopDownOp(this);        //start TD
	  else if (!sNode_->attributeBool("td_multiclass"))
	    taskFinished(0,0);
	  else { //no operator, remove initial hypothesis
	    parent()->decrementCount();
	    parent()->childUnlink(this);
	    delete this;
	  }
  }
}

/** returns the corresponding SNode */
SNode *INode::sNode()
{
  return sNode_;
}

/** Sets the SNode this INode ist type of */
void INode::sNode(SNode * node)
{
  if (sNode_)
    sNode_->unlink();
  sNode_ = node;
  if (node)
    node->link();
}

/** Sets the filename for operator output    */
void INode::output(QString val)
{
  output_ = val;
}

/** Gets the filename for operator output    */
QString & INode::output()
{
  return output_;
}

/** decrement the aktiv counter * aktiv TopDown child operators
 ++ RETURN:
  * -1 -> ERROR (aktivcount < 0)
  *  0 -> aktivcount == 0
  *  1 -> aktivcount > 0
  *  "this" in not a last node*/
int INode::decrementCount()
{
  bool doneWithNode;
  int temporalProcessed;
  int temporalTotal;

#ifdef DEBUGMSG
  qDebug("#  decrementCount(%s)(%p): Start, aktivcount = %d\n",
         name().toLatin1().constData(), this, aktivcount_);
#endif
  analysis_->nodeChange(this);
  if (aktivcount_ == 0) {
#ifdef DEBUGMSG
    qDebug("#  (ERROR) INode::decrementCount < 0!!\n");
#endif
    return -1;
  }

  aktivcount_--;
  
//QFile fDebug("debug_decrementCount.txt");
//fDebug.open(IO_Append);
//Q3TextStream strDebug(&fDebug);
//strDebug << ".1" << endl;
//strDebug << ".1 this->attribute(name): " << this->attribute("name") << endl;
//strDebug << ".1 this->attribute(id): " << this->attribute("id") << endl;
//strDebug << ".1 this->attribute(class): " << this->attribute("class") << endl;
//strDebug << ".7 this->aktivcount_: " << this->aktivcount_ << endl;
//strDebug << ".7 this->childcount_: " << this->childcount_ << endl;
//strDebug << ".7 this->ordercount_: " << this->ordercount_ << endl;
//strDebug << ".7 this->aktivorder_: " << this->aktivorder_ << endl;
//strDebug << ".7 sNode_->temporal(): " << sNode_->temporal() << endl;
//strDebug << ".7 temporalProcessing_: " << temporalProcessing_ << endl;
//fDebug.close();

  if ((sNode_->temporal()) && (temporalProcessing_)) // GILSON: temporal processing if 'childList_.isEmpty()' although temporal node: regular TD processing
	doneWithNode = !temporalChildTopDown(false);
  else
	doneWithNode = !childTopDown(false);
  
  if (doneWithNode) { //all sub nodes have been handled or node is trash
    if (truncation() && !isRoot()) {
      parent()->decrementCount();
    } 
    else {
      if (sNode_->temporal()) {
    	parent()->temporalCount_++;
    	temporalProcessed = parent()->temporalCount_; // count of temporal nodes to be processed
    	temporalTotal = parent()->childList_->count(); // total number of temporal nodes 
    	if (temporalProcessed == temporalTotal)  // check if execTemporalBottomUp() can be executed
    	  execTemporalBottomUp();  // jetzt geht es wieder nach oben
    	else if (ordercount_==0) // there are temporal nodes with different order -> temporal sequence
    		parent()->decrementCount();
      }
      else
	      execBottomUp(); // jetzt geht es wieder nach oben, jawohl!
    }
    return 0;
  }
  return 1;
}


/** increment the aktiv counter  */
void INode::incrementCount(int c)
{
#ifdef DEBUGMSG
  qDebug("#  incrementCount(%s)(%p): Start, aktivcount = %d + %d\n",
	 name().toLatin1().constData(), this, aktivcount_, c);
#endif
  if (c > 0)
    aktivcount_ += c;
}

/** Execute the bottomUp operator */
void INode::execBottomUp()
{
#ifdef DEBUGMSG
  qDebug("#*  INode::execBottomUp(%s)(%p): Start\n", name().toLatin1().constData(), this);
#endif
  if (analysis()->error()) {
    status(BU_ABORTED);
    return;
  }

  execState(BU);
  Q_CHECK_PTR(this->sNode());
  if (sNode_)
    sNode_->execBottomUpOp(this);       //start BU
}

/** Execute the temporalBottomUp operator */
void INode::execTemporalBottomUp()
{
#ifdef DEBUGMSG
  qDebug("#*  INode::execTemporalBottomUp(%s)(%p): Start\n", 
	 name().toLatin1().constData(), 
	 this);
#endif

	QList<INode*> auxList = (this->parent())->children();

  if (analysis()->error()) {
	  QList<INode*>::iterator itSibling;
	  for (itSibling = auxList.begin(); itSibling != auxList.end(); ++itSibling) {	
		INode *iNode = *itSibling;
		iNode->status(BU_ABORTED);
	  }
	  return;
  }

  // execState(BU);
  QList <INode*>::iterator itSibling;
  for (itSibling = auxList.begin(); itSibling != auxList.end(); ++itSibling) {	
	INode *iNode = *itSibling;
	iNode->execState(BU);
  }
  Q_CHECK_PTR(this->sNode());
  if (sNode_)
    sNode_->execTemporalBottomUpOp(this);       //start BU
}

/** return TRUE if the Node is the last Node (leaf node) or the last node to analyze otherwise return FALSE  */
bool INode::isLast()
{
#ifdef DEBUGMSG
  qDebug("#isLast# INode::isLast(%s)(%p): %d\n", 
	 name().toLatin1().constData(), this, sNode_->attributeBool("resultNode"));
#endif
  if (sNode_->attributeBool("resultNode")) return TRUE;
  return ((sNode_->children()).isEmpty());
}

/** return TRUE if the Node is the root node of the net otherwise return FALSE  */
bool INode::isRoot()
{
  return (!parent());
}

/** Sets the label image or returns the label image if image=0 */
GeoImage *INode::labelImage(GeoImage * image)
{
  if (image) {
    if (labelImage_)
      labelImage_->unlink();
    labelImage_ = image->shallowCopy();
  }
  return labelImage_;
}

/** Sets up the region description for this inode using the available information e.g. labelimage */
void INode::setGeoRegion(float gW, float gN, float gE, float gS, INode* parent)
{
#define defFloat(name) float name=0.0; MLParser::setFloat(name,&attribList_,#name);
#define defInt(name) int name=0; MLParser::setInt(name,&attribList_,#name);
#define defString(name) QString name; MLParser::setString(name,&attribList_,#name);
  defFloat(geoNorth);
  defFloat(geoSouth);
  defFloat(geoWest);
  defFloat(geoEast);
  defFloat(scale_x);
  defFloat(scale_y);
  defInt(offset_x);
  defInt(offset_y);
  defInt(urx);
  defInt(ury);
  defInt(llx);
  defInt(lly);
  defString(file);
  defString(name);
  if ((gN == gS) && (gW == gE)) {
    qDebug("INode::setRegion: gN=gS && gW==gE");
    gN = geoNorth;
    gS = geoSouth;
    gW = geoWest;
    gE = geoEast;
  }
  //! Dealing with the case "geocoordinates AND bounding box are given" is missing
  //! Geocoordinates of the labelimage must be calculated in this case.
  labelImage_ =
    analysis()->labelImageList()->loadLabelImage(file, sNode()->GNode::name(), gW, gN, gE, gS);
#define TRY_TO_CORRECT_SCALING_ERRORS
#ifdef TRY_TO_CORRECT_SCALING_ERRORS
  // Correct errors in geocoordinates cause by scaling
  if (parent && scale_x && scale_y
     && parent->labelImage() && (scale_x!=1.0 || scale_y!=1.0)) {
    qDebug("INode::setRegion: correct geocoordinates:");
    qDebug("  old: gW=%f, gN=%f, gE=%f, gS=%f",gW,gN,gE,gS);
    GeoImage* labelImage=parent->labelImage();
    int pllx=int(offset_x/scale_x);
    int pury=int(offset_y/scale_y);
    int purx=int((offset_x+labelImage_->cols())/scale_x);
    int plly=int((offset_y+labelImage_->rows())/scale_y);
    qDebug(" scale_x=%f, scale_y=%f, offset_x=%d offset_y=%d",
           scale_x, scale_y, offset_x, offset_y);
    qDebug(" pllx=%d, plly=%d, purx=%d, pury=%d",
           pllx, plly, purx, pury);
    labelImage->geoBBox(pllx,plly,purx,pury,gN,gS,gW,gE);
    qDebug("  new: gW=%f, gN=%f, gE=%f, gS=%f",gW,gN,gE,gS);
    labelImage_->geoWest(gW);
    labelImage_->geoNorth(gN);
    labelImage_->geoEast(gE);
    labelImage_->geoSouth(gS);
  }
#endif
  Q_ASSERT(labelImage_);
  labelImage_->load();
  if (!attribList_.contains("geoNorth") || attribList_.value("geoNorth").isEmpty()) { // geo coordinates are not set
    qDebug("INode::setRegion: geo ausrechnen");
    labelImage_->geoBBox(llx, lly, urx, ury, geoWest, geoNorth, geoEast,
                         geoSouth);
  }
  else if (!attribList_.contains("llx")) {       // boundingbox not set, but geo coordinates are set
    qDebug("INode::setRegion: bbox ausrechnen");
    // if file geocoordinates describe a smaller area than the inode geocoordinates,
    // adjust the inode geocoordinates
    if (geoWest<gW) geoWest=gW;
    if (geoNorth>gN) geoNorth=gN;
    if (geoEast>gE) geoEast=gE;
    if (geoSouth<gS) geoSouth=gS;

    labelImage_->picBBox(geoWest, geoNorth, geoEast, geoSouth, llx, lly, urx,
                         ury);
    attribList_.replace("llx",QString().sprintf("%d", llx));
    attribList_.replace("lly",QString().sprintf("%d", lly));
    attribList_.replace("urx",QString().sprintf("%d", urx));
    attribList_.replace("ury",QString().sprintf("%d", ury));
  }
  else {
    //! Todo: calculate the labelimage geo coordinates from the given geo coordinates
    //        and label boundingbox
  }
  attribList_.replace("geoNorth",QString().sprintf("%f", geoNorth));
  attribList_.replace("geoSouth",QString().sprintf("%f", geoSouth));
  attribList_.replace("geoWest", QString().sprintf("%f", geoWest));
  attribList_.replace("geoEast", QString().sprintf("%f", geoEast));
  qDebug("INode::setGeoRegion: %s file=%s(%f,%f,%f,%f) bbox=(%5d, %5d, %5d %5d) geo=(%f, %f, %f, %f) res=(%f, %f)",
	 name.toLatin1().constData(), file.toLatin1().constData(), 
	 gW, gN, gE, gS, llx, lly, urx, ury,
	 geoWest, geoNorth, geoEast, geoSouth, labelImage_->resolutionX(),
	 labelImage_->resolutionY());

#undef defFloat
#undef defInt
}

/** for preparing the result label image every inode gets a new id */
int INode::setNewID(int id)
{
  attribute("IDStart", id);
  QList < INode* >::iterator it = children().begin();
  for (; it!=children().end(); ++it) {
    ++id;
    id = (*it)->setNewID(id);
  }
  attribute("IDEnd", id);
  return id;
}

/** merge inodes labelimage into the result image */
void INode::mergeResultImage(GeoImage & resultImg)
{
  qDebug("INode::mergeResultImage: name=%s", name_.toLatin1().constData());
  QList < INode* >::iterator it = (children().begin());
  for (; it!=children().end(); ++it) {
    INode *node = (*it);
    if (node->status() == TRASH)
      continue;
    if (node->labelImage() && node->attributeInt("id")) {
      // id==0 is background and should not be merged into the result image
      if (!resultImg.
          mergeInto(*(node->labelImage()), attributeInt("IDStart"),
                    node->attributeInt("id"), node->attributeInt("IDStart")))
        node->attribute("status", "deleted");
    }
    else {
      node->attribute("status", "no labelimage");
      qDebug("INode::mergeResultImage: %s ommitted",
             node->name().toLatin1().constData());
    }
  }
  for (it=children().begin(); it!=children().end(); ++it) {
    INode *node = (*it);
    if (node->status() == TRASH)
      continue;
    if (node->attributeInt("id")) {
      // id==0 is background and should not be merged into the result image
      node->mergeResultImage(resultImg);
    }
    else {
      qDebug("INode::mergeResultImage: %s ommitted",
             node->name().toLatin1().constData());
    }
  }
}

/** returns the path to this inode */
QString INode::path()
{
#ifdef WIN32
  if (parent())
    return parent()->path() + name() + "\\";
  return name() + "\\";
#else
  if (parent())
    return parent()->path() + name() + "/";
  return name() + "/";
#endif
}

/** set flag to handle min and max restriction of snode definition */
void INode::truncation(bool v){
  qDebug("****:truncate %d => %d (%s %p)",truncation_,v,name().toLatin1().constData(),this);
  truncation_ = v;
  if (truncation_) status(TRASH);
}
/** get flag to handle min and max restriction of snode definition */
bool INode::truncation(void){
  return truncation_;
}


Analysis *INode::analysis_;
