#!/usr/bin/env python
"""
Convert old geoaida ml-files to correct xml
"""
import sys
import os
from os.path import join,exists
import errno
from HTMLParser import *
from PyQt4.QtCore import *
from PyQt4.QtXml import *

inputfile=sys.argv[1]
outputfile=sys.argv[2]

class MLParser(HTMLParser):
    """
    This class parses the node files
    """
    level=0
    processChildren=0
    
    def __init__(self,xmlwriter):
        """
        Initialize the parser with a container for area- and line-objects
        """
        self.fp=xmlwriter;
        self.fp.writeStartDocument()
        HTMLParser.__init__(self)
        print "Start"

    def writeAttributes(self,attributes):
        xmlAttribs=QXmlStreamAttributes()
        for a in attributes:
            xmlAttribs.append("",a[0],a[1])
        self.fp.writeAttributes(xmlAttribs)

    def handle_startendtag(self,tag,attributes):
        """
        This method is called for start-end-tag combination e.g. <node atr="val" />
        """
        print("<%s "%(tag))
        self.fp.writeEmptyElement("",tag)
        self.writeAttributes(attributes)
    def handle_starttag(self,tag,attributes):
        """
        This method is called for every start tag e.g. <node atr="val" >
        """
        print("<%s "%(tag))
        self.fp.writeStartElement("",tag)
        self.writeAttributes(attributes)
        self.level+=1;

    def handle_endtag(self,tag):
        """
        This method ist called for every end tag e.g. </node>
        """
        self.fp.writeEndElement()
        self.level-=1;
        
    def close(self):
        """
        Close the parser and the output file
        """
        self.fp.writeEndDocument()
        HTMLParser.close(self)
        

destfile = QFile(outputfile)
destfile.open(QIODevice.WriteOnly)
xmlwriter=QXmlStreamWriter(destfile)
xmlwriter.setAutoFormatting(1);

parser=MLParser(xmlwriter)
srcdata=open(inputfile).read()
srcdata=srcdata.replace("topdown/","topdown.");
srcdata=srcdata.replace("bottomup/","bottomup.");
srcdata=srcdata.replace("\\\"","&quot;");

parser.feed(srcdata)
parser.close()
    
