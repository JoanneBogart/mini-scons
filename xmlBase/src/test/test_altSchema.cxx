// $Header$
/// Test program for serialization of DOM, stripping of comments

#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"

#include "facilities/commonUtilities.h"

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include <string>
#include <iostream>



int main() {
  facilities::commonUtilities::setupEnvironment();
  // File is well-formed, no reference to dtd or schema
  std::string instanceDoc = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "aDocument.xml");
  std::string theSchema = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "theSchema.xsd");

    
  XERCES_CPP_NAMESPACE_USE
  using xmlBase::Dom;

  xmlBase::XmlParser parser;
  parser.doSchema(true);

  parser.setSchemaLocation(theSchema.c_str(), false);
  DOMDocument* doc = parser.parse(instanceDoc.c_str());
  if (!doc) {
    std::cerr << "Parse of file " << instanceDoc << " failed" << std::endl;
    return 1;
  }
  else {
    std::cout << "Parse of file " << instanceDoc << " succeeded!" 
              << std::endl;
  } 


  return 0;
}
  
  
