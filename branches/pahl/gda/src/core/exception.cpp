#include "exception.h"
#include <QObject>

QString Exception::what() const
{
  return QObject::tr("General Exception"); 
}

FileIOException::FileIOException(ExceptionType type, QString filename)
  : type_(type), filename_(filename)
{
}

QString FileIOException::what() const
{
  switch (type_) {
  case FILE_NOT_EXISTS:
    return QObject::tr("File %1 does not exists").arg(filename_);
  case OPEN_FAILED:
    return QObject::tr("Cannot open %1 for writing").arg(filename_);
  }
}

ImageException::ImageException(int x1, int x2, int dx, int y1, int y2, int dy)
  : x1_(x1), x2_(x2), dx_(dx), y1_(y1), y2_(y2), dy_(dy)
{
}

QString ImageException::what() const
{
  return QObject::tr("Image has wrong dimensions x1=%1, x2=%2, dx=%3, y1=%4, y2=%5, dy=%6")
    .arg(x1_).arg(x2_).arg(dx_).arg(y1_).arg(y2_).arg(dy_);
}

NodeException::NodeException(ExceptionType type)
  : type_(type)
{
}

QString NodeException::what() const
{
  return QObject::tr("Node exception");
}
