#include <QString>
#include <QIODevice>
#include <QDir>
#include "MLParser"
#include "SemanticNet"
#include "InstanceNet"
#include "GeoImage"
#include "GeoImageList"
#include "FileList"

class Project 
{
public:
  Project();
  ~Project();
  void setFilename(QString filename);
  QString filename();
  void read();
  void read(QString filename);
  void read(QIODevice& fp);
  void read(MLParser& parser);
  void readSemanticNet(MLParser& parser);
  void readInstanceNet(MLParser& parser);
  void analyze();
private:
  QString filename_;
  SemanticNet semanticNet_;
  InstanceNet instanceNet_;
  GeoImageList geoImageList_;
  FileList fileList_;
  GeoImage *mapImage_;
};

