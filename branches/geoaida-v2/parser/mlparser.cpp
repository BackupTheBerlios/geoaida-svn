// QtGui - Library 
// Copyright (C) 1999 Martin Pahl, Universität Hannover
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

/*
 * $Source: /data/cvs/qtguiapplication/qtgui/qtparser/MLParser.cpp,v $
 * $Revision: 1.4 $
 * $Date: 2005/01/27 08:15:28 $
 * $Author: pahl $
 * $Locker:  $
 */

#include "mlparser.h"
#include <QTextStream>

//#include <ctype.h>
//#include <qstringlist.h>
#include <QRegExp>

#define DEBUGMSG

QXmlStreamWriter& ArgDict::write(QXmlStreamWriter& fp) const
{
  for (ArgDictConstIterator it=constBegin();it!=constEnd(); ++it) {
    fp.writeAttribute(it.key(),it.value());
#if 0
    QString s=it.value();
    fp << it.key() << "=\""
       << (s.replace(QRegExp("\\\\"),QString("\\\\"))).replace(QRegExp("\""),QString("\\\""))
       << "\" ";
#endif
  }
  return fp;
}

void ArgDict::insert(QString name, int val) {
  QString s=QString().sprintf("%d",val);
  insert(name.toLower(),s);
}

void ArgDict::insert(QString name, double val) {
  QString s=QString().sprintf("%f",val);
  insert(name.toLower(),s);
}

void ArgDict::insert(QString name, QString val) {
  QHash<QString,QString>::insert(name.toLower(),val);
}

void ArgDict::insert(QString name, const char* val) {
  insert(name,QString(val));
}

void ArgDict::insert(QString name, bool val) {
  QString s(val ? "TRUE" : "FALSE" );
  insert(name.toLower(),s);
}

void ArgDict::replace(QString name, int val) {
  insert(name.toLower(),val);
}

void ArgDict::replace(QString name, double val) {
  insert(name.toLower(),val);
}

void ArgDict::replace(QString name, QString val) {
  insert(name.toLower(),val);
}

void ArgDict::replace(QString name, const char* val) {
  insert(name.toLower(),QString(val));
}

void ArgDict::replace(QString name, bool val) {
  insert(name.toLower(),val);
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& fp, ArgDict& argDict)
{
  return argDict.write(fp);
}

/*****************************************************************
CLASS: MLTagTable
*****************************************************************/
MLTagTable::MLTagTable() : QHash<QString,int>()
{
  tagtable_=0;
}

MLTagTable::MLTagTable(const QString *table) : QHash<QString,int>() 
{
  set(table);
}

void MLTagTable::set(const QString *table)
{
  int i;
  int size;

  for (size=0; !table[size].isEmpty(); size++);
  
  tagtable_=new int[size];

  for (i=0; !table[i].isEmpty(); i++) {
    tagtable_[i]=i+1;
    insert(table[i],tagtable_[i]);
  }
}

MLTagTable::~MLTagTable()
{
  if (tagtable_) delete tagtable_;
}


/*****************************************************************
CLASS: MLParser
*****************************************************************/
MLParser::MLParser(QIODevice *fp) : QXmlStreamReader(fp)
{
  //  this->setCodec("ISO 8859-1");
}

MLParser::~MLParser()
{
}

int MLParser::tag(const MLTagTable &tagtable)
{
  int tag;
  TokenType token;
  while (!atEnd()) {
    token=readNext();
    lasttag_=name().toString();
    tag=tagtable[lasttag_];
    switch (token) {
    case Invalid:
      qWarning("MLParser::tag: invalid token in line %d at column %d",lineNumber(),columnNumber());
      break;
    case StartDocument:
    case EndDocument:
    case Characters:
      break;
    case StartElement: 
      return tag;
    case EndElement: 
      return -tag;
    default:
      qWarning("MLParser::tag: unknown token %d",token);
    }
  }
  return END_OF_FILE;
}

ArgDict *MLParser::args(ArgDict *argdict, bool insertMode)
{
  ArgDict *dict;
  if (argdict) dict=argdict;
  else { 
    dict=new ArgDict();
  }
  QXmlStreamAttributes attribs=attributes();
  for (QXmlStreamAttributes::ConstIterator it=attribs.constBegin();
       it!=attribs.constEnd();
       ++it) {
    if (insertMode)
      dict->insert(it->name().toString().toLower(),it->value().toString());
    else
      dict->replace(it->name().toString().toLower(),it->value().toString());
#ifdef DEBUGMSG
    qDebug("MLParser::args %s=%s\n",
	   it->name().toString().toLower().toLatin1().constData(),
	   it->value().toString().toLatin1().constData());
#endif
    
  }
    return dict;
}  
#if 0
QString MLParser::readWord()
{
  if (data_->lc_=='>') data_->readOneChar();
  data_->incLineNumber();
  return data_->readWord();
}

int MLParser::addToLineNumber(int offset)
{
  data_->linenumber_+=offset;
  return data_->linenumber_;
}
#endif

QString MLParser::lasttagstr()
{
  return lasttag_;
}

int MLParser::lineNumber()
{
  return QXmlStreamReader::lineNumber();
}


void MLParser::storeStr(const QString& key, const QString& value)
{
  stringStorage_.insert(key,value);
}

void MLParser::store(const QString& key, void *value)
{
  voidStorage_.insert(key,value);
}

QString MLParser::getStr(const QString& key)
{
  return stringStorage_[key];
}

void* MLParser::get(const QString& key)
{
  return voidStorage_[key];
}

/*****************************************************************
CLASS: MLParser static functions
*****************************************************************/

void MLParser::setString(QString &str, ArgDict *dict, 
				       QString name) 
{
  QString s=dict->value(name.toLower());
  if (!s.isEmpty()) str=s;
}

void MLParser::setBool(bool &var, ArgDict *dict, QString name) 
{
  QString s=dict->value(name.toLower());
  if (!s.isEmpty()) {
      var=true;
      if ((s.toUpper()=="OFF")
	  || (s.toUpper()=="0")
	  || (s.toUpper()=="FALSE")
	  || (s.toUpper()=="AUS")) var=false;
  }
}

void MLParser::setInt(int &var, ArgDict *dict, QString name) {
  QString s=dict->value(name.toLower());
  if (!s.isEmpty()) {
    bool ok;
    int val=s.toInt(&ok);
    if (ok) var=val;
  }
}

void MLParser::setDouble(double &var, ArgDict *dict,
				       QString name) 
{
  QString s=dict->value(name.toLower());
  if (!s.isEmpty()) {
    bool ok;
    double val=s.toDouble(&ok);
    if (ok) var=val;
  }
}

void MLParser::setFloat(float &var, ArgDict *dict,
				       QString name)
{
  QString s=dict->value(name.toLower());
  if (!s.isEmpty()) {
    bool ok;
    float val=s.toFloat(&ok);
    if (ok) var=val;
  }
}


int MLParser::getOption(QString val, QString *keywords)
{
  MLTagTable tagtable(keywords);
  int tag=tagtable[val];
  return tag;
}

void MLParser::setOption(int &var, ArgDict *dict, 
				       QString name, QString *options)
{
  QString s=dict->value(name.toLower());
  if (!s.isEmpty()) var=getOption(s,options);
}

void MLParser::setBitMask(int &var, ArgDict *dict, 
			  QString name, QString *options, int *optionvals)
{
  QString s=dict->value(name.toLower());
  MLTagTable tagtable(options);

  if (!s.isEmpty()) {
    var=0;
    QString attrib=s;
    attrib.replace(QRegExp("[|,\\.;:]")," ");
    QTextStream istr(&attrib);
    QString part;
    while (!istr.atEnd()) {
      istr >>part;
      int tag=tagtable[part];
      if (tag) {
	if (optionvals)
	  var+=optionvals[tag-1];
	else
	  var+=(1 << (tag-1));
      }
    }

#ifdef DEBUGMSG
    qDebug("MLParser::setBitMask(): name=%s val=%s(%d)\n",
	   name.toLatin1().constData(),s.toLatin1().constData(),var);
#endif
  }
}


