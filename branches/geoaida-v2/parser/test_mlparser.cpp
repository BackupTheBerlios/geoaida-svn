#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include "MLParser"

#include <QXmlStreamWriter>

static QString keywords[] = { "node", "region", "group", "" };
const MLTagTable nodeTagTable(keywords);

const char* ascii(QStringRef s)
{
  return s.toString().toLatin1().constData();
}

const char* ascii(const QString& s)
{
  return s.toLatin1().constData();
}

int main(int argc, char** argv)
{

#if 0
  {
    QFile file(argv[1]);
    if (!file.open(QIODevice::WriteOnly)) {
      qFatal("%s not found",argv[1]);
    }
    QXmlStreamWriter fp(&file);
    fp.setAutoFormatting(true);
    fp.writeStartDocument();
    fp.writeDTD("<!DOCTYPE geoaida>");
    fp.writeNamespace("geoaida","geoaida");
    fp.writeStartElement("geoaida","node");
    fp.writeAttribute("value","We\"rt");
    fp.writeStartElement("","node");
    fp.writeEmptyElement("geoaida","node");
    fp.writeAttribute("class","Szene");
    fp.writeAttribute("name","Deutschland");
    fp.writeAttribute("tobdown.vis","bild1");
    fp.writeEndDocument();
    file.close();
  }
#endif

#if 0
  {
    qDebug("test_MLParser: test the reading of ML files");
    QFile file(argv[1]);
    if (!file.open(QIODevice::ReadOnly)) {
      qFatal("%s not found",argv[1]);
    }
    MLParser parser(&file);
    while (!parser.atEnd()) {
      parser.readNext();
      qDebug("\nTAG tokenString=%s name=%s namespaceUri=%s",
	     ascii(parser.tokenString()),
	     ascii(parser.name()),
	     ascii(parser.namespaceUri())
	     );
      if (parser.tokenType()==QXmlStreamReader::StartElement) {
	QXmlStreamAttributes attribs=parser.attributes();
	qDebug(" #attribs %d",attribs.count());
	for (int i=0; i<attribs.count(); i++) {
	  QXmlStreamAttribute attrib=attribs[i];
	  qDebug("--attrb name=%s namespaceUri=%s qualifiedName=%s value=%s",
		 ascii(attribs[i].name()),
		 ascii(attribs[i].namespaceUri()),
		 ascii(attribs[i].qualifiedName()),
		 ascii(attribs[i].value()));
	}
	
	
      }
    }
  }
#else
  qDebug("test_MLParser: test the reading of ML files");
  QFile file(argv[1]);
  if (!file.open(QIODevice::ReadOnly)) {
    qFatal("%s not found",argv[1]);
  }
  MLParser parser(&file);
  int tag=parser.tag(nodeTagTable);
  qDebug("First tag %s",parser.lasttagstr().toLatin1().constData());
  while (!parser.atEnd()) {
    if (tag>0) {
      
      qDebug("START_TAG %s",parser.lasttagstr().toLatin1().constData());
    }
    tag=parser.tag(nodeTagTable);
  }
#endif
}

