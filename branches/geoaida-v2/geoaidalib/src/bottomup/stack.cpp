/***************************************************************************
                          stack.cpp  -  description
                             -------------------
    begin                : Wed Aug 1 2001
    copyright            : (C) 2001 by Martin Pahl, Jürgen Bückner
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
//  #define DEBUGMSG

#include "stack.h"
#include "StackElementNodeList"
#include "StackElementNumber"
#include "StackElementString"
#include "StackElementStack"
#include "RegionSensor"
#include "NodeList"
#include "qregexp.h"
#include <qmap.h>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace BottomUpLib;



//######### NUMERIC FUNCTION  #########
bool Stack::numfkt(double f(double, double))
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::NUMBER && el2->type() == StackElement::NODELIST) {       //value + list
      double d = ((StackElementNumber *) el1)->data();     //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      for (NodeList::Iterator it=nl.begin();
	   it!=nl.end();
	   ++it) {
        Node *node = it.value();      //nl.find(*it);
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        StackElement *nse = node->stackPop();      //pop value from node
        if (nse->type() != StackElement::NUMBER)
          throw CleanUp(false);
        double nd = ((StackElementNumber *) nse)->data();
        node->stackPush(new StackElementNumber(f(nd, d))); //push value to node
#ifdef DEBUGMSG
        printf("<<< >>> %f * %f = %f\n", nd, d, f(nd, d));
#endif //DEBUGMSG
      }
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::NODELIST && el2->type() == StackElement::NODELIST) {        // 2 lists
      NodeList & nl1 = ((StackElementNodeList *) el1)->data();     //liste holen
      NodeList & nl2 = ((StackElementNodeList *) el2)->data();     //liste holen
      if (nl1.size() != nl2.size())
        throw CleanUp(false);   //unterschiedliche Listengroesse
      {
	NodeList::Iterator it2=nl2.begin();
	for (NodeList::Iterator it1=nl1.begin();
	     it1 != nl1.end();
	     ++it1, ++it2) {
	  Node *node1 = it1.value();    //nl.find(*it);
	  Node *node2 = it2.value();    //nl.find(*it);
	  if (node1->stackCount() < 1)
	    throw CleanUp(false); //sind noch genug da?
	  if (node2->stackCount() < 1)
	    throw CleanUp(false); //sind noch genug da?
	  StackElement *nse1 = node1->stackPop();    //pop value from node
	  StackElement *nse2 = node2->stackPop();    //pop value from node
	  if (nse1->type() != StackElement::NUMBER
	      || nse2->type() != StackElement::NUMBER)
	    throw CleanUp(false);
	  double nd1 = ((StackElementNumber *) nse1)->data();
	  double nd2 = ((StackElementNumber *) nse2)->data();
	  node2->stackPush(new StackElementNumber(f(nd2, nd1)));     //push value to node
#ifdef DEBUGMSG
	  printf("<<< >> %f * %f = %f\n", nd2, nd1, f(nd2, nd1));
#endif //DEBUGMSG
	}
      }
      push(new StackElementNodeList(nl2));
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::NUMBER && el2->type() == StackElement::NUMBER) {    //value + value
      double d1 = ((StackElementNumber *) el1)->data();
      double d2 = ((StackElementNumber *) el2)->data();
      push(new StackElementNumber(f(d1, d2)));
#ifdef DEBUGMSG
      printf("<< >> %f (?) %f = %f\n", d1, d2, f(d2, d1));
#endif //DEBUGMSG
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

static inline double add_(double d1, double d2)
{
  return d1 + d2;
}
static inline double sub_(double d1, double d2)
{
  return d1 - d2;
}
static inline double mul_(double d1, double d2)
{
  return d1 * d2;
}
static inline double div_(double d1, double d2)
{
  return d1 / d2;
}
static inline double max_(double d1, double d2)
{
  return (d1 > d2) ? d1 : d2;
}
static inline double min_(double d1, double d2)
{
  return (d1 < d2) ? d1 : d2;
}

/** return the max. value of the both topmost stack elements
    1) ... [value] [value] -> [value] : compare the both topmost stack values
    2) ... [nodelist] [value] -> [nodelist] : compare the local stack value with given value
*/
bool Stack::numMax()
{
  return numfkt(max_);
}

/** return the min. value of the both topmost stack elements
    1) ... [value] [value] -> [value] : compare the both topmost stack values
    2) ... [nodelist] [value] -> [nodelist] : compare the local stack value with given value
*/
bool Stack::numMin()
{
  return numfkt(min_);
}

/**addition of two stack elements.
  * possible types:
  * scalar, scalar, push the result back to stack
  * scalar, list - adds each list element (local stack entry) with scalar, push the list back to stack
  * list, list - adds list element with list element using the local stack in node, lower list push back to stack
  */
bool Stack::add()
{
  return numfkt(add_);
}

/** substraction of two stack elements.
  * possible types:
  * scalar, scalar, push the result back to stack
  * scalar, list - subtracts each list element (local stack entry) with scalar, push the list back to stack
  * list, list - subtracts list element with list element using the local stack in node, lower list push back to stack
  */
bool Stack::sub()
{
  return numfkt(sub_);
}

/** multiplication of two stack elements.
  * possible types:
  * scalar, scalar, push the result back to stack
  * scalar, list - multiply each list element (local stack entry) with scalar, push the list back to stack
  * list, list - multiply list element with list element using the local stack in node, lower list push back to stack
  */
bool Stack::mul()
{
  return numfkt(mul_);
}

/** division of two stack elements.
  * possible types:
  * scalar, scalar, push the result back to stack
  * scalar, list - divide each list element (local stack entry) with scalar, push the list back to stack
  * list, list - divide list element with list element using the local stack in node, lower list push back to stack
  */
bool Stack::div()
{
  return numfkt(div_);
}

//######### LOGIC FUNCTION  #########
/** logic operator for numeric values to mange the stack (eq, neq, gt, lt, get, let) */
bool Stack::lofkt(bool f(double, double), bool sf(QString, QString))
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::NUMBER && el2->type() == StackElement::NODELIST) {       //value + list
      cout << "Stack::lofkt: el1 =" << el1->type() << " el2=" << el2->
        type() << endl;
      double d = ((StackElementNumber *) el1)->data();     //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      //NodeList* selected=new NodeList;// new nodelist for result
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();
        if (node->stackCount() < 1) {   //sind noch genug da?
          cerr << "Not enough elements on local stack " << endl;
          throw CleanUp(false);
        }
        StackElement *nse = node->stackPop();      //pop value from node
        if (nse->type() != StackElement::NUMBER) {
          cerr << "Wrong element type (" << nse->type()
            << ") on local stack - expected " << StackElement::NUMBER << endl;
          throw CleanUp(false);
        }
        double nd = ((StackElementNumber *) nse)->data();
        //if ((f(nd,d)))
        node->stackPush(new StackElementNumber(f(nd, d))); //push value to node
        //selected->insert(it.key(),node);
#ifdef DEBUGMSG
        printf(".<< > (lofkt nu-no) %f ? %f = %d\n", d, nd, f(nd, d));
#endif //DEBUGMSG
      }
#ifdef DEBUGMSG
      cout << "SIZE nl (lofkt nu-no): " << nl.size() << endl;
      //nl.info();
#endif //DEBUGMSG
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::NODELIST && el2->type() == StackElement::NODELIST) {        //value + list
      NodeList & nl1 = ((StackElementNodeList *) el1)->data();     //liste holen
      NodeList & nl2 = ((StackElementNodeList *) el2)->data();     //liste holen
      if (nl1.size() != nl2.size())
        throw CleanUp(false);   //unterschiedliche Listengroesse
      {
	NodeList::Iterator it2=nl2.begin();
	for (NodeList::Iterator it1=nl1.begin();
	     it1 != nl1.end();
	     ++it1, ++it2) {
	  Node *node1 = it1.value();    //nl.find(*it);
	  Node *node2 = it2.value();    //nl.find(*it);
	  if (node1->stackCount() < 1)
	    throw CleanUp(false); //sind noch genug da?
	  if (node2->stackCount() < 1)
	    throw CleanUp(false); //sind noch genug da?
	  StackElement *nse1 = node1->stackPop();    //pop value from node
	  StackElement *nse2 = node2->stackPop();    //pop value from node
	  if (nse1->type() != StackElement::NUMBER
	      || nse2->type() != StackElement::NUMBER)
	    throw CleanUp(false);
	  double nd1 = ((StackElementNumber *) nse1)->data();
	  double nd2 = ((StackElementNumber *) nse2)->data();
	  //if ((f(nd1,nd2)))
	  node2->stackPush(new StackElementNumber((f) (nd2, nd1)));  //push value to node
#ifdef DEBUGMSG
	  printf(".<<< >> (lofkt no-no) %f ? %f = %d\n", nd2, nd1,
		 (*f) (nd2, nd1));
#endif //DEBUGMSG
	}
      }

      push(new StackElementNodeList(nl2));
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::NUMBER && el2->type() == StackElement::NUMBER) {    //value + value
      double d1 = ((StackElementNumber *) el1)->data();
      double d2 = ((StackElementNumber *) el2)->data();
      push(new StackElementNumber((*f) (d2, d1)));
#ifdef DEBUGMSG
      printf(".<< >> (lofkt no-no) %f * %f = %d\n", d2, d1, (f) (d2, d1));
#endif //DEBUGMSG
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::STRING && el2->type() == StackElement::NODELIST) {  //string + list
      QString d = ((StackElementString *) el1)->data();    //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      NodeList *selected = new NodeList;        // new nodelist for result
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();
        QString name = node->classname();
        node->stackPush(new StackElementNumber(sf(name, d)));      //push value to node
        // if ((sf(node->classname(),d)))
        selected->insert(it.key(), node);        //XYXY
#ifdef DEBUGMSG
        printf(".<< > (lofkt s-no) %s ? %s = %d\n", d.latin1(),
               node->classname().latin1(), sf(node->classname(), d));
#endif //DEBUGMSG
      }
#ifdef DEBUGMSG
      cout << "SIZE (lofkt s-no) selected: " << selected->size() << endl;
      //nl.info();
#endif //DEBUGMSG
      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

// logic fkt for nummeric values
static inline bool eq_(double d1, double d2)
{
  return d1 == d2;
}

static inline bool neq_(double d1, double d2)
{
  return !(d1 == d2);
}

static inline bool gt_(double d1, double d2)
{
  return d1 > d2;
}

static inline bool get_(double d1, double d2)
{
  return d1 >= d2;
}

static inline bool lt_(double d1, double d2)
{
  return d1 < d2;
}

static inline bool let_(double d1, double d2)
{
  return d1 <= d2;
}

static inline bool and_(double d1, double d2)
{
  return ((d1!=0.0) && (d2!=0.0));
}

static inline bool nand_(double d1, double d2)
{
  return (!((d1!=0.0) && (d2!=0.0)));
}

static inline bool or_(double d1, double d2)
{
  return ((d1!=0.0) || (d2!=0.0));
}

static inline bool nor_(double d1, double d2)
{
  return (!((d1!=0.0) || (d2!=0.0)));
}



// logic fkt for string values
/**Returns TRUE if s1 is equal to s2 or FALSE if they are different. */
static inline bool seq_(QString d1, QString d2)
{
  return d1 == d2;
}

/**Returns TRUE if s1 is not equal to s2 or FALSE if they are equal.  */
static inline bool sneq_(QString d1, QString d2)
{
  return d1 != d2;
}

/**Returns TRUE if s1 is lexically greater than s2 or FALSE if it is not. */
static inline bool sgt_(QString d1, QString d2)
{
  return d1 > d2;
}

/**Returns TRUE if s1 is lexically greater than or equal to s2 or FALSE if it is not.  */
static inline bool sget_(QString d1, QString d2)
{
  return d1 >= d2;
}

/**Returns TRUE if s1 is lexically less than s2 or FALSE if it is not.  */
static inline bool slt_(QString d1, QString d2)
{
  return d1 < d2;
}

/**Returns TRUE if s1 is lexically less than or equal to s2 or FALSE if it is not.  */
static inline bool slet_(QString d1, QString d2)
{
  return d1 <= d2;
}

/** equality of two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  */
bool Stack::eq()
{
  return lofkt(eq_, seq_);
}

/** inequality of two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  */
bool Stack::neq()
{
  return lofkt(neq_, sneq_);
}

/** 'greater than' of two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  */
bool Stack::gt()
{
  return lofkt(gt_, sgt_);
}

/** 'greater or equal than' of two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  */
bool Stack::get()
{
  return lofkt(get_, sget_);
}

/** 'lower than' of two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  */
bool Stack::lt()
{
  return lofkt(lt_, slt_);
}

/** 'lower or equal than' of two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  */
bool Stack::let()
{
  return lofkt(let_, slet_);
}

/** and-operation on two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  * does not work for strings 
  */

bool Stack::andop()
{
  return lofkt(and_, seq_);
}

bool Stack::nandop()
{
  return lofkt(nand_, seq_);
}

/** or-operation on two stack elements.
  * possible types:
  * scalar, scalar - push the result [0,1] back to stack
  * scalar, list - push new list back to stack where the comparison 'local stack in node' and 'scalar' is true
  * list, list - push new list back to stack where the comparison 'local stack in node1' and 'local stack in node2' is true
  * does not work for strings 
  */
bool Stack::orop()
{
  return lofkt(or_, seq_);
}

bool Stack::norop()
{
  return lofkt(nor_, seq_);
}



//#########  FUNCTION  #########

QMap < QString, bool(Stack::*)() > Stack::functionTable_;
QMap < QString, bool(*)(Stack &) > Stack::externFunctionTable_;

bool Stack::initFunctionTable_ = true;
NodeList *Stack::nodeList_ = 0;
QString Stack::filename_;
ArgDict Stack::attribList_;

void Stack::initFunctionTable() const
{
  functionTable_.insert(QString("=="), &Stack::eq);
  functionTable_.insert(QString(">="), &Stack::get);
  functionTable_.insert(QString(">"), &Stack::gt);
  functionTable_.insert(QString("<"), &Stack::lt);
  functionTable_.insert(QString("nl.count"), &Stack::nl_count);
  functionTable_.insert(QString("<="), &Stack::let);
  functionTable_.insert(QString("!="), &Stack::neq);
  functionTable_.insert(QString("dup"), &Stack::dup);
  functionTable_.insert(QString("drop"), &Stack::drop);
  functionTable_.insert(QString("swap"), &Stack::swap);
  functionTable_.insert(QString("selectInvers"), &Stack::selectInvers);
  functionTable_.insert(QString("selectClass"), &Stack::selectClass);
  functionTable_.insert(QString("select"), &Stack::select);
  functionTable_.insert(QString("*"), &Stack::mul);
  functionTable_.insert(QString("/"), &Stack::div);
  functionTable_.insert(QString("+"), &Stack::add);
  functionTable_.insert(QString("concat"), &Stack::concat);
  functionTable_.insert(QString("-"), &Stack::sub);
  functionTable_.insert(QString("&"), &Stack::andop);
  functionTable_.insert(QString("&&"), &Stack::andop);
  functionTable_.insert(QString("and"), &Stack::andop);
  functionTable_.insert(QString("|"), &Stack::orop);
  functionTable_.insert(QString("||"), &Stack::orop);
  functionTable_.insert(QString("or"), &Stack::orop);
  functionTable_.insert(QString("calc"), &Stack::calcAttribute);
  functionTable_.insert(QString("nodelist"), &Stack::pushNodeList);
  functionTable_.insert(QString("list2stack"), &Stack::list2stack);
  functionTable_.insert(QString("stack2list"), &Stack::stack2list);
  functionTable_.insert(QString("set"), &Stack::setAttrib);
  functionTable_.insert(QString("get"), &Stack::getAttrib);
  functionTable_.insert(QString("node.set"), &Stack::nodeSet);
  functionTable_.insert(QString("node.get"), &Stack::nodeGet);
  functionTable_.insert(QString("runlocal"), &Stack::runLocal);
  functionTable_.insert(QString("node.run"), &Stack::runLocal);
  functionTable_.insert(QString("pushlocal"), &Stack::pushLocal);
  functionTable_.insert(QString("node.push"), &Stack::pushLocal);
  functionTable_.insert(QString("maxDist"), &Stack::maxDist);
  functionTable_.insert(QString("min"), &Stack::numMin);
  functionTable_.insert(QString("max"), &Stack::numMax);
  functionTable_.insert(QString("merge"), &Stack::merge);
  functionTable_.insert(QString("merge_nn"), &Stack::merge_nn);
  functionTable_.insert(QString("set_judgement"), &Stack::set_judgement);
  functionTable_.insert(QString("nl_average"), &Stack::nl_average);
  functionTable_.insert(QString("nl_sum"), &Stack::nl_sum);
  functionTable_.insert(QString("nl_div"), &Stack::nl_div);
  functionTable_.insert(QString("nl.average"), &Stack::nl_average);
  functionTable_.insert(QString("nl.sum"), &Stack::nl_sum);
  functionTable_.insert(QString("nl.div"), &Stack::nl_div);
  functionTable_.insert(QString("nl_max"), &Stack::nl_max);
  functionTable_.insert(QString("node.max"), &Stack::nl_max);
  functionTable_.insert(QString("nl.average"), &Stack::nl_average);
  functionTable_.insert(QString("nl.sum"), &Stack::nl_sum);
  functionTable_.insert(QString("nl.div"), &Stack::nl_div);
  functionTable_.insert(QString("nl.run"), &Stack::nlRun);
  functionTable_.insert(QString("nl.push"), &Stack::nlPush);
  functionTable_.insert(QString("nl.pop"), &Stack::nlPop);
  functionTable_.insert(QString("nl.set"), &Stack::nlSet);
  functionTable_.insert(QString("nl.get"), &Stack::nlGet);
}

/** register a new function */
void Stack::registerFunction(QString name, bool function(Stack &))
{
  externFunctionTable_.insert(name, function);
}

Stack::Stack(NodeList * nl, QString filename)
{
  if (initFunctionTable_) {
    initFunctionTable();
    initFunctionTable_ = false;
  }
  if (nl)
    nodeList_ = nl;
  if (!filename.isEmpty())
    filename_ = filename;
}

Stack::Stack(const Stack & stack)
{
  copy(stack);
}

Stack::~Stack()
{
  while (!isEmpty())
    delete pop();
}

Stack & Stack::operator = (const Stack & stack) {
  return copy(stack);
}

Stack & Stack::copy(const Stack & stack)
{
  if (this != &stack) {
    clear();
    for (Iterator it=begin(); it!=end(); ++it) {
      push((*it)->copy());
    }
  }
  return *this;

}

/** Duplicate the top most element */
bool Stack::dup()
{
  if (isEmpty())
    return false;
  StackElement *el = pop();
  push(el);
  el = el->copy();
  push(el);
  return true;
}

/** Drop the topmost element
 */
bool Stack::drop()
{
  if (isEmpty())
    return false;
  StackElement *el = pop();
  if (el)
    delete el;
  return true;
}

/** Swap the two topmost stack elements */
bool Stack::swap()
{
  if (count() < 2)
    return false;
  StackElement *el1 = pop();
  StackElement *el2 = pop();
  push(el1);
  push(el2);
  return true;

}

/** Filter the nodes in nodelist with local stack entry 1=TRUE */
bool Stack::select()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);
    el1 = pop();
    if (el1->type() == StackElement::NODELIST) {
      NodeList & nl = ((StackElementNodeList *) el1)->data();
      NodeList *selected = new NodeList;        // new nodelist for result
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        StackElement *nse = node->stackPop();      //pop value from node
        if (nse->type() != StackElement::NUMBER)
          throw CleanUp(false);
        //double nd=((StackElementNumber*)nse)->data();
        if (((StackElementNumber *) nse)->data() == 1)
          selected->insert(it.key(), node);      //XYXY
#ifdef DEBUGMSG
        printf("(select) %f\n", ((StackElementNumber *) nse)->data());
#endif //DEBUGMSG
      }
#ifdef DEBUGMSG
      cout << "SIZE selected (select.): " << selected->size() << endl;
#endif //DEBUGMSG
      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::NUMBER
             || el2->type() == StackElement::NODELIST) {
      double d1 = ((StackElementNumber *) el1)->data();
      NodeList & nl = ((StackElementNodeList *) el2)->data();
      NodeList *selected = new NodeList;        // new nodelist for result
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        StackElement *nse = node->stackPop();      //pop value from node
        if (nse->type() != StackElement::NUMBER)
          throw CleanUp(false);
        //double nd=((StackElementNumber*)nse)->data();
        if (((StackElementNumber *) nse)->data() == d1)
          selected->insert(it.key(), node);      //XYXY
#ifdef DEBUGMSG
        printf("(select) %f %f\n", d1, ((StackElementNumber *) nse)->data());
#endif //DEBUGMSG
      }
#ifdef DEBUGMSG
      cout << "SIZE selected (select..): " << selected->size() << endl;
#endif //DEBUGMSG
      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** Filter the nodes in nodelist with local stack entry 1=FALSE */
bool Stack::selectInvers()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);
    el1 = pop();
    if (el1->type() == StackElement::NODELIST) {
      NodeList & nl = ((StackElementNodeList *) el1)->data();
      NodeList *selected = new NodeList;        // new nodelist for result
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        StackElement *nse = node->stackPop();      //pop value from node
        if (nse->type() != StackElement::NUMBER)
          throw CleanUp(false);
        //double nd=((StackElementNumber*)nse)->data();
        if (((StackElementNumber *) nse)->data() != 1)
          selected->insert(it.key(), node);      //XYXY
#ifdef DEBUGMSG
        printf("(selectInvers) %f\n", ((StackElementNumber *) nse)->data());
#endif //DEBUGMSG
      }
#ifdef DEBUGMSG
      cout << "SIZE selected (selectInvers): " << selected->size() << endl;
#endif //DEBUGMSG
      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::NUMBER
             || el2->type() == StackElement::NODELIST) {
      double d1 = ((StackElementNumber *) el1)->data();
      NodeList & nl = ((StackElementNodeList *) el2)->data();
      NodeList *selected = new NodeList;        // new nodelist for result
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        StackElement *nse = node->stackPop();      //pop value from node
        if (nse->type() != StackElement::NUMBER)
          throw CleanUp(false);
        //double nd=((StackElementNumber*)nse)->data();
        if (((StackElementNumber *) nse)->data() != d1)
          selected->insert(it.key(), node);      //XYXY
#ifdef DEBUGMSG
        printf("(selectInvers) %f %f\n", d1,
               ((StackElementNumber *) nse)->data());
#endif //DEBUGMSG
      }
#ifdef DEBUGMSG
      cout << "SIZE selected (selectInvers): " << selected->size() << endl;
#endif //DEBUGMSG
      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}


/** Filter all the nodelist in elem 2 with the classname in elemen 1 */
bool Stack::selectClass()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);
    el1 = pop();
    el2 = pop();
    if (el1->type() != StackElement::STRING
        || el2->type() != StackElement::NODELIST)
      throw CleanUp(false);
    QString s = ((StackElementString *) el1)->data();
    NodeList & nl = ((StackElementNodeList *) el2)->data();
    NodeList *selected = nl.selectClass(s);
    if (!selected)
      throw CleanUp(false);
    //selected->info();
    push(new StackElementNodeList(*selected));
    //XXXXXXXXXXXXX delete selected;
    throw CleanUp(true);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** calculate attribute for all member in nodelist and put it on the local stack
  * exist funktion:
  * size - node size in pixel
  * area - node size in m^2
  * x_center - node center x-value in image coordinates
  * y_center - node center y-value in image coordinates
  * llx - lower left x-point of enclose rectangle
  * lly - lower left y-point of enclose rectangle
  * urx - upper right x-point of enclose rectangle
  * ury - upper right y-point of enclose rectangle
  * circumference - circumference of the label [0,...,1]
  * roundness - roundness of the label [0,...,1]
  * squareness - squareness of the label [0,...,1]
  * compactness - compactness of the label [0,...,1]
  * thickness - thickness of the label = count shrinking steps in pixel
  * M11 - momentum M11 of the label
  * M02 - momentum M02 of the label
  * M20 - momentum M20 of the label
  * orthogonality - orthogonality of the label using hought (take a while)
*/
bool Stack::calcAttribute()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    RegionSensor *rs = new RegionSensor();
    if (count() < 2)
      throw CleanUp(false);
    el1 = pop();
    el2 = pop();
    if (el1->type() != StackElement::STRING
        && el2->type() != StackElement::NODELIST)
      throw CleanUp(false);
    QString s = ((StackElementString *) el1)->data();
    NodeList & nl = ((StackElementNodeList *) el2)->data();
    nl.calcForSelect("ALL", s, rs);
    push(new StackElementNodeList(nl));
    if (rs)
      delete rs;
    throw CleanUp(true);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}



typedef bool(Stack::*StackFunction) ();
typedef bool ExternFunction(Stack &);

/** Execute the given cmd */
bool Stack::run(QString cmd)
{
  cmd = cmd.simplified();
  QStringList cmdList = cmd.split(" ");
  for (QStringList::Iterator it = cmdList.begin(); it != cmdList.end(); ++it) {


    // Debug Stack types
    cout << "(" << count() << "): ";
    {
      for (Iterator it=begin(); it!=end(); ++it) {
        cout << (*it)->typeName() << " ";
      }
      cout << endl;
    }

    
    QMap < QString, bool(*)(Stack &) >::ConstIterator efit =
      externFunctionTable_.find(*it);
    if (efit!=externFunctionTable_.end()) {
      cout << "(" 
	   << count() 
	   << ") Calling extern function " 
	   << it->toLatin1().constData() 
	   << endl;
      ExternFunction *func = efit.value();
      if (!(*func) (*this))
        return false;
      continue;
    }

    QMap < QString, bool(Stack::*)() >::const_iterator fit =
      functionTable_.find(*it);

    if (fit!=functionTable_.end()) {
      cout << "(" 
	   << count() 
	   << ") Calling function "
	   << it->toLatin1().constData() 
	   << endl;
      StackFunction func = fit.value();
      if (!(*this.*func) ()) {
        cerr << "function " 
	     << it->toLatin1().constData() 
	     << " failed!" 
	     << endl;
        return false;
      }
      continue;
    }
    if (((*it).length() > 0) && (*it)[0] == '\"') {
      QString s = (*it);
      while (it != cmdList.end()) {

        if ((s[s.length() - 1] == '\"') && (s.length() >= 2)
            && (s[s.length() - 2] != '\\'))

          break;
        ++it;
        s += " " + (*it);
      }
      if ((s[(int) s.length() - 1] != '\"') || (s.length() >= 2)
          && (s[(int) s.length() - 2] == '\\'))
        return false;

      s = s.mid(1, s.length() - 2);
      s = s.replace(QRegExp("\\\\\""), "\"");
      cout << "(" << count() << ") String:" << s.toLatin1().constData() << endl;
      push(new StackElementString(s));
      continue;
    }
    bool ok;
    double v = (*it).toDouble(&ok);
    if (!ok){
      cerr << "Stack::run Error: " 
	   << it->toLatin1().constData() 
	   << " neither command nor argument. Exiting. " 
	   << endl; 
      return false;
    }
    cout << "(" 
	 << count() 
	 << ") Num:" 
	 << it->toLatin1().constData() 
	 << endl;
    push(new StackElementNumber(v));
  }
  return true;
}

/** Push the nodelist on the stack */
bool Stack::pushNodeList()
{
  if (nodeList_) {
    push(new StackElementNodeList(*nodeList_));
    return true;
  }
  else
    return false;

}

/** write the topmost stackelement, which should be a nodelist or a stack of nodlist, to the given textstream */
bool Stack::write(QXmlStreamWriter & fp, QString keyword)
{
  StackElement *el1 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);
    el1 = pop();
    if (el1->type() == StackElement::NODELIST) {
      NodeList & nl = ((StackElementNodeList *) el1)->data();
      //nl.merge(false);
      if (keyword == "node") {
        nl.genGroupImage();
        nl.write(fp, filename_ + ".bu.plm");
      }
      else {
        nl.writeNodeFile(fp);
      }
      throw CleanUp(true);
    }
    if (el1->type() == StackElement::STACK) {
      Stack & stack = ((StackElementStack *) el1)->data();
      while (stack.count()) {
        if (!stack.write(fp, keyword))
          throw CleanUp(false);
      }
      throw CleanUp(true);
    }
    throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    return e.status_;
  }
}

/** write the topmost stackelement to the given filename */
void Stack::write(QString filename, QString keyword)
{
  QFile fp(filename);           // 'XML' - description
  nodeList_->setImgName(filename + ".bu.node.plm");
  if (!fp.open(QIODevice::WriteOnly)) {
    fprintf(stderr, "Stack::write: file not accesable to %s\n",
            filename.toLatin1().constData());
    return;
  }
  QXmlStreamWriter str(&fp);
  str.setAutoFormatting(true);
  str.writeStartDocument();
  str.writeStartElement(keyword+"list");
  write(str, keyword);
  str.writeEndElement();
  str.writeEndDocument();
}

/** concatenate the both topmost lists in stack */
bool Stack::concat()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;

  try {
    if (count() < 2)
      throw CleanUp(false);
    el1 = pop();
    el2 = pop();
    if (el1->type() != StackElement::NODELIST
        && el2->type() != StackElement::NODELIST)
      throw CleanUp(false);
    NodeList & nl1 = ((StackElementNodeList *) el1)->data();
    NodeList & nl2 = ((StackElementNodeList *) el2)->data();
    nl2 += nl1;
    push(new StackElementNodeList(nl2));
    throw CleanUp(true);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** push n lists from stack to a 'seperate stack' on stack */
bool Stack::list2stack()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);
    el1 = pop();
    if (el1->type() != StackElement::NUMBER)
      throw CleanUp(false);
    int d1 = int (((StackElementNumber *) el1)->data());   //Anzahl der Listen
    if (d1 < 0)
      throw CleanUp(false);
    Stack *se = new Stack();
    for (int i = 0; i < d1; i++) {      //Listen umsetzen
      if (count() < 1)
        throw CleanUp(false);
      el2 = pop();
      if (el2->type() != StackElement::NODELIST)
        throw CleanUp(false);
      se->push(el2);
      //se->insertAt(0,new StackElementList(el2));
    }
    push(new StackElementStack(*se));       //stack mag kein insertAt!
    throw CleanUp(true);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** push n lists from the 'seperate stack' and the numbers of lists to stack  */
bool Stack::stack2list()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);
    el1 = pop();
    if (el1->type() != StackElement::STACK)
      throw CleanUp(false);
    Stack & st = ((StackElementStack *) el1)->data();      //stack vom stack
    int v(st.count());
    for (Stack::Iterator it=st.begin(); it!=st.end(); ++it) {
      push((*it)->copy());
    }
    /*
       while (!st.isEmpty()) { //über alle elemente
       if (st.count()<1) throw CleanUp(false);
       el2=st.pop();
       if (el2->type()!=StackElement::NODELIST) throw CleanUp(false);
       push(el2);
       }
     */
    push(new StackElementNumber(v));
    throw CleanUp(true);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** set an attribute; stack -> node
  make only a copy - don't change the local stack*/
bool Stack::nodeSet()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING && el2->type() == StackElement::NODELIST) {       //value + list
      QString attribName = ((StackElementString *) el1)->data();   //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();      //nl.find(*it);
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        StackElement *nse = node->stackPop();      //pop value from node
        QString val;
        switch (nse->type()) {
        case StackElement::STRING:
          val = ((StackElementString *) nse)->data();
          break;
        case StackElement::NUMBER:{
            double nd = ((StackElementNumber *) nse)->data();
            val.sprintf("%f", nd);
          }
          break;
        default:
          throw CleanUp(false);
          break;
        }
        node->replace(attribName, new QString(val));
        delete nse;
//                  node->stackPush(nse);
      }
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

bool Stack::nlSet()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING && el2->type() == StackElement::NODELIST) {       //value + list
      QString attribName = ((StackElementString *) el1)->data();   //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      if (nl.stackCount() < 1)
        throw CleanUp(false);   //sind noch genug da?
      StackElement *nse = nl.stackPop();   //pop value from node
      QString val;
      switch (nse->type()) {
      case StackElement::STRING:
        val = ((StackElementString *) nse)->data();
        break;
      case StackElement::NUMBER:{
          double nd = ((StackElementNumber *) nse)->data();
          val.sprintf("%f", nd);
        }
        break;
      default:
        throw CleanUp(false);
        break;
      }
      nl.set(attribName, val);
      delete nse;
//          nl.stackPush(nse);
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

bool Stack::setAttrib()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING) {     //value + list
      QString attribName = ((StackElementString *) el1)->data();   //wert holen
      QString val;
      switch (el2->type()) {
      case StackElement::STRING:
        val = ((StackElementString *) el2)->data();
        break;
      case StackElement::NUMBER:{
          double nd = ((StackElementNumber *) el2)->data();
          val.sprintf("%f", nd);
        }
        break;
      default:
        throw CleanUp(false);
        break;
      }
      attribList_.replace(attribName, val);
//          push(el2);
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** get an attribute */
bool Stack::nodeGet()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;

  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING && el2->type() == StackElement::NODELIST) {       //value + list
      QString attribName = ((StackElementString *) el1)->data();   //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();      //nl.find(*it);
        
	if (!node->contains(attribName)) {
	  cerr << "Warning: Attribute not set: " 
	       << node->classname().toLatin1().constData() << "::" 
	       << node->name().toLatin1().constData() 
	       << "(" << attribName.toLatin1().constData() << ")" << endl;
        }
        QString val = node->value(attribName);
        bool ok;
        double d = val.toDouble(&ok);
        if (ok)
          node->stackPush(new StackElementNumber(d));
        else
          node->stackPush(new StackElementString(val));
      }
      push(new StackElementNodeList(nl));
     throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;

    return e.status_;
  }
}

/** get an attribute */
bool Stack::nlGet()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING && el2->type() == StackElement::NODELIST) {       //value + list
      QString attribName = ((StackElementString *) el1)->data();   //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      QString val = nl.get(attribName);
#ifdef DEBUGMSG
      cout << "val="<<val<<endl;
#endif      
      bool ok;
      double d = val.toDouble(&ok);
      if (ok)
        nl.stackPush(new StackElementNumber(d));
      else
        nl.stackPush(new StackElementString(val));
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** get an attribute */
bool Stack::getAttrib()
{
  StackElement *el1 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING) {     //value + list
      QString attribName = ((StackElementString *) el1)->data();   //wert holen
      QString val;
      MLParser::setString(val, &attribList_, attribName);
      bool ok;
      double d = val.toDouble(&ok);
      if (ok)
        push(new StackElementNumber(d));
      else
        push(new StackElementString(val));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    return e.status_;
  }
}

/** merge all node - using 'p' weighting
... [nodelist] -> [nodelist]
 - set the variable 'nodelist' new to the returned list
 - put 'nodelist' to the stack
*/
bool Stack::merge()
{
  StackElement *el1 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   //value + list
      NodeList & nl = ((StackElementNodeList *) el1)->data();      //liste holen
      nodeList_ = nl.merge(false, filename_ + ".bu.node.plm");
      push(new StackElementNodeList(*nodeList_));
      throw CleanUp(true);
    }
    else if (el1->type() == StackElement::STACK) { //      value + stack
#ifdef DEBUGMSG
      cout << "merge mit stack" << endl;
#endif //DEBUGMSG
      Stack & st = ((StackElementStack *) el1)->data();    //      stack holen
      Stack *backst_ = new Stack;       // Ergebnis stack anlegen
      NodeList res_list;
      while (st.count() >= 1) {
        StackElement *el = st.pop();
        if (el->type() != StackElement::NODELIST)
          throw(CleanUp(false));
        NodeList & nl = ((StackElementNodeList *) el)->data();     //      liste holen;
        NodeList *part_list = nl.merge(false, filename_ + ".bu.node.plm");
        res_list += *part_list;
        backst_->push(new StackElementNodeList(*part_list));
       delete el;
       delete part_list;
      }
      push(new StackElementStack(*backst_));
      *nodeList_ = res_list;
      delete backst_;
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    return e.status_;
  }
}

/** merge all node - using 'p' weighting
  generate new nodes
  ... [nodelist] -> [nodelist]
 - set the variable 'nodelist' new to the returned list
 - put 'nodelist' on the stack

*/
bool Stack::merge_nn()          //merge with New Nodes
{
  StackElement *el1 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   //value + list
      NodeList & nl = ((StackElementNodeList *) el1)->data();      //liste holen
      //NodeList* newNL = nl.merge(true,filename_+".bu.node.plm");
      nodeList_ = nl.merge(true, filename_ + ".bu.node.plm");
      push(new StackElementNodeList(*nodeList_));
      //push(new StackElementNodeList(*newNL));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    return e.status_;
  }
}

/** run a command on the local stacks */
bool Stack::runLocal()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING && el2->type() == StackElement::NODELIST) {       //value + list
      QString command = ((StackElementString *) el1)->data();      //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();      //nl.find(*it);
        (node->stack()).run(command);
      }
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}


/** run a command on the local stacks */
bool Stack::nlRun()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() == StackElement::STRING && el2->type() == StackElement::NODELIST) {       //value + list
      QString command = ((StackElementString *) el1)->data();      //wert holen
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      nl.stack().run(command);
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}


/** push a stackelement on the local stacks */
bool Stack::pushLocal()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el2->type() == StackElement::NODELIST) {   //value + list
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      for (NodeList::Iterator it=nl.begin();
	     it!=nl.end();
	     ++it) {
        Node *node = it.value();      //nl.find(*it);
        (node->stack()).push(el1->copy());
      }
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}


/** push a stackelement on the local stacks */
bool Stack::nlPush()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el2->type() == StackElement::NODELIST) {   //value + list
      NodeList & nl = ((StackElementNodeList *) el2)->data();      //liste holen
      nl.stackPush(el1);
      push(new StackElementNodeList(nl));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** pop a stackelement on the local stacks */
bool Stack::nlPop()
{
  StackElement *el1 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   //value + list
      NodeList & nl = ((StackElementNodeList *) el1)->data();      //liste holen
      StackElement *el = nl.stackPop();
      push(new StackElementNodeList(nl));
      push(el);
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    return e.status_;
  }
}


/** group nodes with distance < maxDist by using 'Minimum Spanning Tree'.
input: [nodelist] [value]
output: [stack of lists] */
bool Stack::maxDist()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen
    if (el1->type() != StackElement::NUMBER
        || el2->type() != StackElement::NODELIST)
      throw CleanUp(false);
    double maxdist = ((StackElementNumber *) el1)->data(); //wert holen
    NodeList & nl = ((StackElementNodeList *) el2)->data();        //liste holen
    RegionSensor *rs = new RegionSensor();
    nl.calcForSelect("ALL", "xGeoCenter", rs);
    nl.calcForSelect("ALL", "yGeoCenter", rs);
    QList < GeoPos* > *gpList = new QList < GeoPos* >;;
    for (NodeList::Iterator it=nl.begin();
	   it!=nl.end();
	   ++it) {     //hilfsliste fuellen
      Node *node = it.value();        //nl.find(*it);
      GeoPos *gp =
        new GeoPos(node, (node->getValue("xGeoCenter")).toFloat(),
                   (node->getValue("yGeoCenter")).toFloat());
      gp->key(it.key());
      gpList->append(gp);
    }

    maxdist *= maxdist;         //quadratischen abstand verwenden
    QList < QList < GeoPos* >* >gpll;       //liste von listen - das Ergebnis
    QList < GeoPos* > *gpt = new QList < GeoPos* >; //neue tmp-liste zum zwischenspeichern
    GeoPos *gpo, *gpot;
    int flag;
    QList < GeoPos* > *gpin = gpList;
    QList < GeoPos* > *gpout = gpt;
    QList < GeoPos* > *gpswitch;
    while (!gpin->isEmpty()) {
      gpo = gpin->takeLast();       //object holen
      QList < GeoPos* > *gpl = new QList < GeoPos* >;       //neue liste
      gpll.append(gpl);         //liste in ergebnis liste einfügen
      gpl->append(gpo);         //objekt in ergebnis liste einfügen
      flag = 1;
      if (!gpin->isEmpty())
        while (flag) {
          if (gpin->isEmpty())
            flag = 0;
          if (!gpin->isEmpty()) {
            gpot = gpin->takeLast();        //test-object holen
#ifdef DEBUGMSG
            cout << " ## " << maxdist << " <?< " << gpo->
              qDist(gpot) << " : " << (maxdist < gpo->qDist(gpot)) << endl;
#endif //DEBUGMSG
            if (maxdist < gpo->qDist(gpot))
              gpout->append(gpot);      //abstand zu gross
            else {
              gpl->append(gpot);        //in ergebnis liste einfügen
              flag = 1;
            }
          }
        }                       //while (flag)
      gpswitch = gpin;          //listen umtauschen
      gpin = gpout;
      gpout = gpswitch;
    }                           //while (!gpin->isEmpty())

    Stack *st = new Stack;      //ergebnis auf den stack schreiben
#ifdef DEBUGMSG
    cout << "## Anz. Listen: " << gpll.count() << endl;
#endif //DEBUGMSG
    while (!gpll.isEmpty()) {
      NodeList *rnl = new NodeList;
      QList < GeoPos* > *grl = gpll.takeLast();
#ifdef DEBUGMSG
      cout << " ## Ele.Anz.: " << grl->count() << endl;
#endif //DEBUGMSG
      while (!grl->isEmpty()) {
        GeoPos *gx = grl->takeLast();
        rnl->insert(gx->key(), gx->node());
      }
      st->push(new StackElementNodeList(*rnl));
    }
    if (gpList)
      delete gpList;
    if (gpt)
      delete gpt;
    if (rs)
      delete rs;
    push(new StackElementStack(*st));
    throw CleanUp(true);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** set the judgement for the 'stack' see also function 'nl_average' and 'nl_max'
 */
bool Stack::set_judgement()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   //value
      NodeList & nl = ((StackElementNodeList *) el1)->data();      //liste holen
      el2 = nl.stackPop();      //ele. vom listenstack holen
      //nl.judgement(nl.stackPop());
      if (el2->type() == StackElement::NUMBER) {
        float f = ((StackElementNumber *) el2)->data();    //Wert holen
        nl.set("p", QString().sprintf("%f", f));
#if 0
        nl.judgement(f);
#endif
      }
      else
        throw CleanUp(false);   //wrong type
      push(new StackElementNodeList(nl));  // nodelist back to stack
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** calculate average over all nodes (menmber of list). Take all top local stack
values from node and write the result in the 'nodelist' stack.
see also function 'set_judgement' of class 'stack' */
//... [nodelist] -> [nodelist]
bool Stack::nl_average()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   //list
      NodeList & nl = ((StackElementNodeList *) el1)->data();      //liste holen
      NodeList *selected = new NodeList;        // new nodelist for result
      float sum = 0.0, val = 0.0;
      int count = 0;
      for (NodeList::Iterator it=nl.begin();
	   it!=nl.end();
	   ++it) {  
        Node *node = it.value();      //nl.find(*it);
        selected->insert(it.key(), node);
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        el2 = node->stackPop();
        if (el2->type() == StackElement::NUMBER) {
          val = ((StackElementNumber *) el2)->data();      //Wert holen
          count++;
          sum += val;
        }
      }
      //Y nl.stackPush(new StackElementNumber(sum/float(val))); //average to nodelist stack
      selected->stackPush(new StackElementNumber(sum / float (count)));    //average to nodelist stack
      push(new StackElementNodeList(*selected));
      //Y push(new StackElementNodeList(nl));// nodelist back to stack
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** calculate max over all nodes (menmber of list). Take all top local stack
values from node and write the result in the 'nodelist' stack.
see also function 'set_judgement' of class 'stack' */
//... [nodelist] -> [nodelist]
bool Stack::nl_max()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   //list
      NodeList & nl = ((StackElementNodeList *) el1)->data();      //liste holen
      NodeList *selected = new NodeList;        // new nodelist for result
      float val = 0.0, max = -9999999.0;
      for (NodeList::Iterator it=nl.begin();
	   it!=nl.end();
	   ++it) {  
        Node *node = it.value();      //nl.find(*it);
        selected->insert(it.key(), node);
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        el2 = node->stackPop();
        if (el2->type() == StackElement::NUMBER) {
          val = ((StackElementNumber *) el2)->data();      //Wert holen
          if (val > max)
            max = val;
        }
      }
      //Y nl.stackPush(new StackElementNumber(max));//max to nodelist stack
      selected->stackPush(new StackElementNumber(max));    //max to nodelist stack
      push(new StackElementNodeList(*selected));
      //Y push(new StackElementNodeList(nl));// nodelist back to stack
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** calculate sum over all nodes (menmber of list). Take all top local stack
values from node and write the result in the 'nodelist' stack.
see also function 'set_judgement' of class 'stack' */
//... [nodelist] -> [nodelist]
bool Stack::nl_sum()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   //list
      NodeList & nl = ((StackElementNodeList *) el1)->data();      //liste holen
      NodeList *selected = new NodeList;        // new nodelist for result
      float sum = 0.0, val = 0.0;
      int count = 0;
      for (NodeList::Iterator it=nl.begin();
	   it!=nl.end();
	   ++it) {  
        Node *node = it.value();      //nl.find(*it);
        selected->insert(it.key(), node);
        if (node->stackCount() < 1)
          throw CleanUp(false); //sind noch genug da?
        el2 = node->stackPop();
        if (el2->type() == StackElement::NUMBER) {
          val = ((StackElementNumber *) el2)->data();      //Wert holen
          count++;
          sum += val;
        }
      }
#ifdef DEBUGMSG
      cout << "sum= " << sum << endl;
#endif //DEBUGMSG

      selected->stackPush(new StackElementNumber(sum));    //       sum to nodelist stack
      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** calculate sum over all nodes (menmber of list). Take all top local stack
values from node and write the result in the 'nodelist' stack.
see also function 'set_judgement' of class 'stack' */
//... [nodelist] -> [nodelist]
bool Stack::nl_div()
{
  StackElement *el1 = 0;
  StackElement *el2 = 0;
  StackElement *el3 = 0;
  StackElement *el4 = 0;
  try {
    if (count() < 2)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    el2 = pop();                //von stack holen        
    if ((el1->type() == StackElement::NODELIST) && (el1->type() == StackElement::NODELIST)) { // 2 lists
      NodeList & nl1 = ((StackElementNodeList *) el1)->data();     //       liste 1 holen
      NodeList & nl2 = ((StackElementNodeList *) el2)->data();     //       liste 2 holen
      NodeList *selected = new NodeList;        // new nodelist for result
      //      2 values from nodelists
      float div1, div2, div;
      el3 = nl1.stackPop();
      el4 = nl2.stackPop();
      if (el3->type() == StackElement::NUMBER)
        div1 = ((StackElementNumber *) el3)->data();
      else
        throw CleanUp(false);
      if (el4->type() == StackElement::NUMBER)
        div2 = ((StackElementNumber *) el4)->data();
      else
        throw CleanUp(false);
      //      nodelists empty
      if (div1 + div2 == 0.0)
        div = 0.0;
		else
        div = div1 / (div1+div2);
#ifdef DEBUGMSG
      cout << div1 << " / " << div1+div2 << "=Ergebnis= " << div << endl;
#endif
      //      compose result
      selected->stackPush(new StackElementNumber(div));

      for (NodeList::Iterator it=nl1.begin();
	   it!=nl1.end();
	   ++it) {  
        Node *node = it.value();
        selected->insert(it.key(), node);
      }
      for (NodeList::Iterator it=nl2.begin();
	   it!=nl2.end();
	   ++it) {  
        Node *node = it.value();
        selected->insert(it.key(), node);
      }

      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    if (el2)
      delete el2;
    return e.status_;
  }
}

/** count number of nodes in nodelist and
write result to nodelist stack */
//... [nodelist] -> [nodelist]
bool Stack::nl_count()
{
  StackElement *el1 = 0;
  try {
    if (count() < 1)
      throw CleanUp(false);     //sind noch genug da?
    el1 = pop();                //von stack holen
    if (el1->type() == StackElement::NODELIST) {   // 1 lists
      NodeList & nl1 = ((StackElementNodeList *) el1)->data();     //       liste 1 holen
      NodeList *selected = new NodeList;        // new nodelist for result
      //       values from nodelists
      int sum = 0;

      for (NodeList::Iterator it=nl1.begin();
	   it!=nl1.end();
	   ++it) {  
        Node *node = it.value();
        selected->insert(it.key(), node);
        sum++;
      }

      selected->stackPush(new StackElementNumber(sum));

#ifdef DEBUGMSG
      cout << "in nl_count: Anzahl=" << sum << endl;
#endif

      push(new StackElementNodeList(*selected));
      throw CleanUp(true);
    }
    else
      throw CleanUp(false);
  }
  catch(CleanUp e) {
    if (el1)
      delete el1;
    return e.status_;
  }
}
