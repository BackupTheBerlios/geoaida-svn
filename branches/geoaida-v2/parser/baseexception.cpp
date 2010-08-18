#include "baseexception.h"
#include <QObject>

BaseException::BaseException(const char* sourcefile=0, int sourceline=0)
 : sourcefile_(sourcefile), sourceline_(sourceline)
{
}

QString BaseException::what() const
{
  return QObject::tr("General Exception"); 
}

QString BaseException::debug() const
{
  return QObject::tr("%1 in %2 at %3")
    .arg(classname())
    .arg(sourcefile_)
    .arg(sourceline_);
}

GeneralException::GeneralException(QString errorMessage, 
				   const char* sourcefile, int sourceline)
  : BaseException(sourcefile,sourceline), message_(errorMessage)
{
}

QString GeneralException::what() const
{
  return message_;
}


