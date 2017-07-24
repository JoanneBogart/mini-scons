/// Test program for xml facility.  Parse xml file; complain if it fails.
/// Input arg, if present is path to file to be parsed.  Otherwise
/// use default

#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include <xercesc/dom/DOMElement.hpp>
//#include <xercesc/dom/DOMNodeList.hpp>

#include <string>
#include <iostream>
#include <fstream>

#include "facilities/commonUtilities.h"

int main(int argc, char* argv[]) {
  XERCES_CPP_NAMESPACE_USE
  facilities::commonUtilities::setupEnvironment();

  xmlBase::XmlParser parser;
  std::string f("$(XMLBASEXMLPATH)/simpleDoc.xml");
    
  if (argc > 1) {
    f = std::string(argv[1]);
  }
  DOMDocument* doc = parser.parse(f.c_str());
    
  if (doc != 0) {  // successful
    std::cout << "Document successfully parsed" << std::endl;
    //        xmlBase::Dom::prettyPrintElement(docElt, std::cout, "");
  }
  else {
    return 1;
  }
  return 0;
}



