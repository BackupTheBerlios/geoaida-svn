////////////////////////////////////////////////////////////////////////////////
///
/// \file		garegionsplitter.cpp
/// \brief		Implementation of methods and functions for region handling
///
/// \date		Aug. 2007
/// \author		Christian Becker (becker@tnt.uni-hannover.de)
///				Torsten Büschenfeld (bfeld@tnt.uni-hannover.de)
///
/// \note		Tabulator size 4 used
///
////////////////////////////////////////////////////////////////////////////////

#include "garegionsplitter.h"
#include <QXmlStreamWriter>
#include <QFile>

using namespace std;
namespace Ga
{

QList<RegDesc> splitIntoRegions(Image &labelpic,  
                                     RegionFinder& regfind,
                                     QString regionclass,
                                     QString labelfile,
                                     int minsize, int maxsize)
{
	if (maxsize <0 )
		maxsize=INT_MAX;

	QList<RegDesc> rList;
	RegionSplitterT<RegDesc, RegionFinder> rSplitter(rList,labelpic,regfind,minsize,maxsize);
	rSplitter.setRegionClass(regionclass);
	rSplitter.setLabelFile(labelfile);

	rSplitter.split();

	return rList;

}

QList<RegDesc> splitIntoRegions(Image &labelpic,  
                                     RegionFinder& regfind,
                                     int minsize, int maxsize)
{
    return splitIntoRegions(labelpic, regfind, "undefined", "", minsize, maxsize);
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Save region description to file
///
/// \param filename Name of file the description will be saved to
/// \param reglist Structure defining description format
///
////////////////////////////////////////////////////////////////////////////////
int regionsToFile(QString filename, QList<RegDesc>& reglist)
{
  QFile fp(filename);
  if (!fp.open(QIODevice::WriteOnly)) {
    cerr << "can't open region output file \"" 
	 << filename.toLatin1().constData() << "\"" << endl;
    return EXIT_FAILURE;
  }
  
  QXmlStreamWriter out(&fp);
  out.setAutoFormatting(true);
  out.writeStartDocument();
  out.writeStartElement("regionlist");
  for (QList<RegDesc>::Iterator regIter= reglist.begin(); 
       regIter != reglist.end(); 
       ++regIter) {
	   
    RegDesc reg = *regIter;
    if (reg.id_ < 1) // Skip the first element because they contain id 0 (= background regions), 
      continue;// I'm not sure why they are included in the list at all... (there was a time when id 0 and 1 were excluded)
    reg.write(out);
  }
  
  out.writeEndElement();
  out.writeEndDocument();
  return EXIT_SUCCESS;
}
} // namespace Ga
