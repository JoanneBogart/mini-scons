/// Test program for xmlBase facility.  Parse xml file and optionally
/// write it out to a stream.

#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include "facilities/Util.h"
#include "facilities/commonUtilities.h"

#include <string>
#include <iostream>
#include <fstream>

/**
   Two arguments may be supplied for input and output file.
     * First argument defaults to $XMLBASEROOT/xml/test.xml
     * Second argument defaults to no output file at all.  If supplied,
       the value of  "-" is taken to mean standard output.  Anything
       else is assumed to be a filename.
*/
int main(int argc, char* argv[]) {
  XERCES_CPP_NAMESPACE_USE
  facilities::commonUtilities::setupEnvironment();
  std::string infile;
  if (argc < 2) { 
    infile=facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "test.xml");
  }
  else {
    infile = std::string(argv[1]);
  }
    
  facilities::Util::expandEnvVar(&infile);
 
  xmlBase::XmlParser* parser = new xmlBase::XmlParser(true);

  DOMDocument* doc = 0;
  try {
    doc = parser->parse(infile.c_str());
  }
  catch (xmlBase::ParseException ex) {
    std::cout << "caught exception with message " << std::endl;
    std::cout << ex.getMsg() << std::endl;
    delete parser;
    return 0;
  }

  if (doc != 0) {  // successful
    std::cout << "Document successfully parsed" << std::endl;

    // look up some attributes
    DOMElement* docElt = doc->getDocumentElement();
    DOMElement* attElt = 
      xmlBase::Dom::findFirstChildByName(docElt, "ChildWithAttributes");
    double doubleVal;
    int    intVal;

    try {
      intVal = xmlBase::Dom::getIntAttribute(attElt, "goodInt");
      std::cout << "goodInt value was " << intVal << std::endl << std::endl;
    }
    catch (xmlBase::DomException ex) {
      std::cout << std::endl << "DomException:  " << ex.getMsg() 
                << std::endl << std::endl;
    }
      
    try {
      std::vector<int> ints;
      unsigned nInts = xmlBase::Dom::getIntsAttribute(attElt, "goodInts", ints);
      std::cout << "Found " << nInts << " goodInts:  " <<  std::endl;
      for (unsigned iInt=0; iInt < nInts; iInt++) {
        std::cout << ints[iInt] << "  ";
      }
      std::cout << std::endl << std::endl;
    }
    catch (xmlBase::DomException ex) {
      std::cout << std::endl << "DomException processing goodInts:  " 
                << ex.getMsg() << std::endl << std::endl;
    }

    try {
      std::vector<double> doubles;
      unsigned nD = xmlBase::Dom::getDoublesAttribute(attElt, "goodDoubles", 
                                                  doubles);
      std::cout << "Found " << nD << " goodDoubles:  " <<  std::endl;
      for (unsigned iD=0; iD < nD; iD++) {
        std::cout << doubles[iD] << "  ";
      }
      std::cout << std::endl << std::endl;
    }
    catch (xmlBase::DomException ex) {
      std::cout << std::endl << "DomException processing goodDoubles:  " 
                << ex.getMsg() << std::endl << std::endl;
    }



    try {
      intVal = xmlBase::Dom::getIntAttribute(attElt, "badInt");
      std::cout << "badInt value was " << intVal << std::endl << std::endl;
    }
    catch (xmlBase::DomException ex) {
      std::cout << std::endl << "DomException:  " << ex.getMsg() 
                << std::endl << std::endl;
    }
      
    try {
      doubleVal = xmlBase::Dom::getDoubleAttribute(attElt, "goodDouble");
      std::cout << "goodDouble value was " << doubleVal 
                << std::endl << std::endl;
    }
    catch (xmlBase::DomException ex) {
      std::cout << std::endl << "DomException:  " << ex.getMsg() 
                << std::endl << std::endl;
    }

    try {
      doubleVal = xmlBase::Dom::getDoubleAttribute(attElt, "badDouble");
      std::cout << std::endl << "badDouble value was " << doubleVal 
                << std::endl << std::endl;
    }
    catch (xmlBase::DomException ex) {
      std::cout << std::endl << "DomException:  " << ex.getMsg() 
                << std::endl << std::endl;
    }


    try {
      std::vector<double> doubles;
      unsigned nD = xmlBase::Dom::getDoublesAttribute(attElt, "badDoubles", 
                                                  doubles);
      std::cout << "Found " << nD << " badDoubles:  " <<  std::endl;
      for (unsigned iD=0; iD < nD; iD++) {
        std::cout << doubles[iD] << "  ";
      }
      std::cout << std::endl << std::endl;
    }
    catch (xmlBase::DomException ex) {
      std::cout << std::endl << "DomException processing badDoubles:  " 
                << ex.getMsg() 
                << std::endl << std::endl;
    }

    if (argc > 2) { // attempt to output
      const char  *hyphen = "-";

      std::ostream* out;

      if (*(argv[2]) == *hyphen) {
        out = &std::cout;
      }
      else {   // try to open file as ostream
        char *filename = argv[2];
        out = new std::ofstream(filename);
      }
      *out << "Document source: " << std::string(argv[1]) << std::endl;
      *out << std::endl << "Straight print of document:" << std::endl;
      xmlBase::Dom::printElement(docElt, *out);
      *out << std::endl << std::endl << "Add indentation and line breaks:" 
           << std::endl;
      xmlBase::Dom::prettyPrintElement(docElt, *out, "");
    }
  }
  delete parser;
  return(0);
}
