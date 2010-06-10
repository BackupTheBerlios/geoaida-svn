#include "exception.h"
#include <QObject>

FileIOException::FileIOException(ExceptionType type, QString filename, 
				 const char* sourcefile, int sourceline)
  : BaseException(sourcefile,sourceline), type_(type), filename_(filename)
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

ImageException::ImageException(ExceptionType type, int x1, int x2, int dx, 
			       int y1, int y2, int dy, 
			       const char* sourcefile, int sourceline)
  : BaseException(sourcefile,sourceline), 
    type_(type), 
    x1_(x1), x2_(x2), dx_(dx), 
    y1_(y1), y2_(y2), dy_(dy),
    filename_("")
{
}

ImageException::ImageException(ExceptionType type, QString filename,
			       const char* sourcefile, int sourceline)
  : BaseException(sourcefile,sourceline), 
    type_(type), 
    x1_(0), x2_(0), dx_(0), 
    y1_(0), y2_(0), dy_(0),
    filename_(filename)
{
}

QString ImageException::what() const
{
  switch (type_) {
  case Dimension:
    return QObject::tr("Image has wrong dimensions x1=%1, x2=%2, dx=%3, y1=%4, y2=%5, dy=%6")
      .arg(x1_).arg(x2_).arg(dx_).arg(y1_).arg(y2_).arg(dy_);
  case UnknownType:
    return QObject::tr("Unknown type of image %1").arg(filename_);
  }
}

NodeException::NodeException(ExceptionType type, 
			     const char* sourcefile, int sourceline)
  : BaseException(sourcefile,sourceline), type_(type)
{
}

QString NodeException::what() const
{
  switch (type_) {
  case ALLOC_FAILED:
    return QObject::tr("Node exception");
  case NOT_FOUND:
    return QObject::tr("No node found");
  }
}

ProcessException::ProcessException(int error, QString cmd,
				   const char* sourcefile, int sourceline)
  : BaseException(sourcefile,sourceline), error_(error), cmd_(cmd)
{
}

QString ProcessException::what() const
{
  return QObject::tr("cmd %1 exited with error %2").arg(cmd_).arg(error_);
}
