#include "project.h"
#include <QFile>
#include <QFileInfo>
#include "FileIOException"
#include <QCoreApplication>
#include "GeneralException"

/** Consctructor */
Project::Project()
  : mapImage_(0),analysis_(0),analysisRunning_(false)
{
  
}

/** Desctructor */
Project::~Project()
{
}

/* Set filename */
void Project::setFilename(QString filename)
{
  filename_=filename;
}

inline QString Project::filename()
{
  return filename_;
}

/** Read project. The filename must be set before */
void Project::read()
{
  read(filename());
}

/** Read project using the given filename */
void Project::read(QString filename)
{

  QFile fp(filename);
  if (!fp.open(QIODevice::ReadOnly)) {
    qDebug("GeoImageList::read(%s): file not found",
	   filename.toLatin1().constData());
    throw FileIOException(FileIOException::FILE_NOT_EXISTS, filename);
  }
  read(fp);
  fp.close();
  setFilename(filename);
}

/** Read project using the given file pointer */
void Project::read(QIODevice& fp)
{
  MLParser parser(&fp);
  read(parser);
}

/** Read project using the given parser */
void Project::read(MLParser& parser)
{
  QString keywords[]={"geoproject",
                      "geosemnet",
                      "geoimagelist",
                      "geoinstancenet",
		      "geoinstnethist",
		      "geoinstnetfut",
		      "geofilelist",
		      ""};
  enum {
    TOK_GEOPROJECT=1,
    TOK_GEOSEMNET,
    TOK_GEOIMAGELIST,
    TOK_GEOINSTANCENET,
    TOK_GEOINSTNETHIST,
    TOK_GEOINSTNETFUT,
    TOK_GEOFILELIST
  };
  const MLTagTable nodeTagTable(keywords);
  int tag;
  bool inProject = false;
  do {
    tag=parser.tag(nodeTagTable);
    switch (tag) {
    case TOK_GEOPROJECT: {
      inProject=true;
      break;
    } 
    case TOK_GEOSEMNET: {
      if (inProject) {
        ArgDict* args=parser.args();
        QString dir;
	QString netfile;
        MLParser::setString(netfile,args,"file");  //read tempor.
        MLParser::setString(dir,args,"dir");  //read tempor.
        delete args;      
	QDir d(dir);
        semanticNet_.read(d.filePath(netfile));
        }
      break;
    } 
    case TOK_GEOIMAGELIST: {
      if (inProject) { 
        geoImageList_.read(parser);  
      }
      break;
    } 
    case TOK_GEOFILELIST: {
      if (inProject) { 
        fileList_.read(parser);  
      }
      break;
    } 
    case TOK_GEOINSTANCENET: {
      if (inProject) {
        ArgDict* args=parser.args();
        QString dir;
	QString netfile;
        QString map;
        MLParser::setString(netfile,args,"file");  //read tempor.
        MLParser::setString(dir,args,"dir");  //read tempor.
        MLParser::setString(map,args,"map");  //read tempor.
        QDir d(dir);
	try {
	  instanceNet_.setFilename(netfile);
	  instanceNet_.read(d.filePath(netfile));
	  INode *rootNode=instanceNet_.rootNode();
	  if (rootNode) {
	    float gN=rootNode->attributeFloat("geoNorth");
	    float gE=rootNode->attributeFloat("geoEast");
	    float gS=rootNode->attributeFloat("geoSouth");
	    float gW=rootNode->attributeFloat("geoWest");
	    GeoImage* mapImage_=new GeoImage(map,"Map",gW,gN,gE,gS);
	    if (mapImage_) {
	      try {
		mapImage_->load();
	      }
	      catch (FileIOException err) {
		delete mapImage_;
		mapImage_=0;
	      }
	    }
	  }
        }
	catch (FileIOException err) {; }
        delete args;
      }
      break;
    }
    default: {
      ArgDict* args=parser.args();
      delete args;
      break;
    }
    }
  } while ((tag!=MLParser::END_OF_FILE ) && (tag != -TOK_GEOPROJECT));
}

SemanticNet& Project::semanticNet()
{
  return semanticNet_;
}

void Project::analyze()
{
  if (!analysis_) delete analysis_;
  analysis_=new Analysis(&semanticNet_,&geoImageList_,&labelImageList_);
  connect(analysis_,SIGNAL(sigFinished()),this,SLOT(analysisFinished()));
  analysisRunning_=true;
  analysis_->start();
  while (analysisRunning_) 
    QCoreApplication::processEvents(QEventLoop::AllEvents,30);
  instanceNet_.setRootNode(analysis_->instanceRootNode());
  mapImage_=analysis_->mapImage();
}

void Project::analysisFinished()
{
  analysisRunning_=false;
}

void Project::saveResults()
{
  instanceNet_.write();
  QFileInfo finfo(instanceNet_.filename());
  finfo.setFile(finfo.dir().dirName(),finfo.baseName()+".map");
  if (!mapImage_) throw GeneralException(QObject::tr("Map doesn't exists!"));
  mapImage_->replace("file",finfo.filePath());
  mapImage_->write();
}

#if 0
void MainGui::loadPrjFile(QString fname)
{
	_qtguiappl->widget()->setCaption("GeoAIDA ("+QFileInfo(fname).baseName()+")");
  // Clean up
  geoImageList_.clear();
  labelImageList_.clear();
  treeEditor->disconnectSlots();
  sceneViewer->disconnectSlots();
  sceneViewer->rootNode(0);
  trashViewer->disconnectSlots();
  trashViewer->rootNode(0);
  mapViewer->set(0,0);
  /*
 	if (analysis_) delete analysis_;
 	else {
  	if (iNode_) delete iNode_;
  	if (map_) delete map_;
  }
  */
  iNode_=0;
  map_=0;
  analysis_=0;
  readyToAna(FALSE);  //disable the analysis buttons
  interactive(FALSE); //disable the analysis buttons

  MLParser parser(&fp);
  // TEST
  // geoImageList_->part("VIS_1", 5580570.4, 5578235.5, 3402580.5, 3404340.6);
  // geoImageList_->part("VIS_2", 5580570.4, 5578235.5, 3402580.5, 3404340.6, "./TMPTMP.pfm");
  // geoImageList_->part("VIS_77", 5580570.4, 5578235.5, 3402580.5, 3404340.6);
  // TEST
  fp.close();        
  readyToAna(TRUE);  //enable the analysis buttons
}

#endif
