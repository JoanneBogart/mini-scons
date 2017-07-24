/// Test program for xml facility.  Parse xml file and write it out to a stream.

#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include <string>
#include <iostream>
#include <fstream>

#include "facilities/commonUtilities.h"

std::string doc_string
(
 "<?xml version=\"1.0\" ?>"
 "<!DOCTYPE TopElement ["
 "  <!ELEMENT TopElement (ChildElt*) >"
 "  <!ELEMENT ChildElt (ChildWithText | EmptyChild)* >"
 "  <!ATTLIST ChildElt  anAttribute CDATA #REQUIRED >"
 "  <!ELEMENT ChildWithText (#PCDATA) >"
 "  <!ATTLIST ChildWithText attr CDATA #IMPLIED>"
 "  <!ELEMENT EmptyChild EMPTY>  ]"
 ">"
 "<TopElement>"
 "  <ChildElt  anAttribute=\"I'm nested but empty\" />"
 "  <ChildElt  anAttribute=\"I'm nested with content\">"
 "     <ChildWithText attr=\"text content\" >"
 "         Text content here."
 "     </ChildWithText>"
 "     <EmptyChild />"
 "  </ChildElt>"
 "</TopElement>"
 );             

int main() {
 XERCES_CPP_NAMESPACE_USE
    facilities::commonUtilities::setupEnvironment();
    xmlBase::XmlParser parser;
    
    DOMDocument* doc = parser.parse(doc_string);
    
    if (doc != 0) {  // successful
        std::cout << "Document successfully parsed" << std::endl;
        DOMElement* docElt = doc->getDocumentElement();
        xmlBase::Dom::prettyPrintElement(docElt, std::cout, "");
    }
    else {
      return 1;
    }
    return 0;
}



