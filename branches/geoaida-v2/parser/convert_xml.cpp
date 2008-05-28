#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QString>
#include <QFile>
#include <QBuffer>

const char* ascii(QStringRef s)
{
  return s.toString().toLatin1().constData();
}

#define refascii(s) s.toString().toLatin1().constData()
#define strascii(s) s.toLatin1().constData()


int main(int argc, char** argv)
{

  QFile srcfile(argv[1]);
  if (!srcfile.open(QIODevice::ReadOnly)) {
    qFatal("%s not found",argv[1]);
  }
  QByteArray buffer(srcfile.readAll());
  srcfile.close();
  QString s=QString::fromLatin1(buffer.data());
  s.replace("\\\"","&quot;");
  s.replace("topdown/","topdown.");
  s.replace("bottomup/","bottomup.");
  buffer=s.toLatin1();
  //  buffer="<node></node>";
  qDebug("%s ", buffer.data());
  
  QFile destfile(argv[2]);
  if (!destfile.open(QIODevice::WriteOnly)) {
    qFatal("%s not found",argv[1]);
  }
  QXmlStreamWriter fp(&destfile);
  fp.setAutoFormatting(true);
  fp.writeStartDocument();

  
  //  srcfile.open(QIODevice::ReadOnly);
  //  QXmlStreamReader parser(&srcfile);
  
  QBuffer srcdata(&buffer);
  srcdata.open(QIODevice::ReadOnly);
  QXmlStreamReader parser(&srcdata);
  
  QXmlStreamReader::TokenType lastTokenType=QXmlStreamReader::Invalid;
  QString tokenName;
  QXmlStreamAttributes attributes;
  while (!parser.atEnd()) {
    
    parser.readNext();
    qDebug("Tokentype: %s",strascii(parser.tokenString()));
    QXmlStreamReader::TokenType tokenType=parser.tokenType();
    switch (tokenType) {
    case QXmlStreamReader::StartElement:
      if (lastTokenType==QXmlStreamReader::StartElement) {
	fp.writeStartElement("",tokenName);
	fp.writeAttributes(attributes);
      }
      tokenName=parser.name().toString();
      attributes=parser.attributes();
      for (int i=0; i<attributes.count(); i++) {
	QXmlStreamAttribute attrib=attributes[i];
	QString value=attrib.value().toString();
	qDebug("--attrb(%d) name=%s namespaceUri=%s qualifiedName=%s value=%s",i,
	       ascii(attrib.name()),
	       ascii(attrib.namespaceUri()),
	       ascii(attrib.qualifiedName()),
	       refascii(attrib.value()));
      }

      lastTokenType=tokenType;
      break;
    case QXmlStreamReader::EndElement:
      if (lastTokenType==QXmlStreamReader::StartElement) {
	fp.writeEmptyElement("",tokenName);
	//	fp.writeStartElement("",tokenName);
       	fp.writeAttributes(attributes);
	//	fp.writeEndElement();
      }
      else {
	fp.writeEndElement();
      }
      lastTokenType=tokenType;
      break;
    case QXmlStreamReader::Invalid:
      qWarning("%s\nin line %d column %d",
	       parser.errorString().toLatin1().constData(),
	       parser.lineNumber(),parser.columnNumber());
      break;
    default:
      qDebug("Tokentype(%d) %s",tokenType,strascii(parser.tokenString()));
    }

      
  }

  fp.writeEndDocument();
  destfile.close();
}

