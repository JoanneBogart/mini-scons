// $Header$
/// Test program for serialization of DOM, stripping of comments

#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"

#include "facilities/commonUtilities.h"

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include <string>
#include <iostream>

unsigned stripAndWrite(const std::string& fname, bool standalone=false);

XERCES_CPP_NAMESPACE_USE

int main() {
  using facilities::commonUtilities;
  using xmlBase::Dom;

  // Test out creating a new document
  DOMElement* myDoc = Dom::makeDocument("myDoc");
  /* DOMElement* emptyNode =*/ //commented to avoid 'unused variable' warning
  Dom::makeChildNode(myDoc, "emptyElement");
  DOMElement* secondChild = Dom::makeChildNode(myDoc, "secondChild");
  /* DOMElement* grandChild = */ //commented to avoid 'unused variable' warning
  Dom::makeChildNodeWithContent(secondChild, "grandChild", "yoo-hoo");
  /* DOMElement* intChild =  */
  Dom::makeChildNodeWithContent(secondChild, "intChild", 2);
  /* DOMElement* hexChild =  */
  Dom::makeChildNodeWithHexContent(secondChild, "hexChild", 1023);

  if (Dom::writeIt(myDoc, "myDoc.xml")) {
    std::cout << "Successfully wrote myDoc.xml" << std::endl;
  } else {
    std::cout << "Unable to write myDoc.xml" << std::endl;
  }

  commonUtilities::setupEnvironment();
  // File is well-formed, no reference to dtd or schema
  std::string WFfile = 
    commonUtilities::joinPath(commonUtilities::getXmlPath("xmlBase"), 
                              "test.xml");
  unsigned ret = stripAndWrite(WFfile);
  std::string summary = ret ? std::string("FAILURE") : std::string("SUCCESS");
  std::cout << "strip and write " << WFfile << " was a " 
            << summary << std::endl;

  // Here's a file with embedded dtd
  std::string embeddedDtd = 
    commonUtilities::joinPath(commonUtilities::getXmlPath("xmlBase"), 
                              "test-dtd.xml");
  ret = stripAndWrite(embeddedDtd); 
  summary = ret ? std::string("FAILURE") : std::string("SUCCESS");
  std::cout << "strip and write " << embeddedDtd << " was a " 
            << summary << std::endl;

  // Now do it again for test file referencing dtd
  std::string refDtd = 
    commonUtilities::joinPath(commonUtilities::getXmlPath("xmlBase"),
                              "myIFile.xml");
  ret = stripAndWrite(refDtd, true);
  summary = ret ? std::string("FAILURE") : std::string("SUCCESS");
  std::cout << "strip and write " << refDtd << " was a " 
            << summary << std::endl;

  // One last time for test file referencing schema rather than dtd
  std::string refSchema = 
    commonUtilities::joinPath(commonUtilities::getXmlPath("xmlBase"), 
                              "mySchemaIFile.xml");
  ret = stripAndWrite(refSchema);
  summary = ret ? std::string("FAILURE") : std::string("SUCCESS");
  std::cout << "strip and write " << refSchema << " was a " 
            << summary << std::endl;
  
  return(0);
}

// Return of 0 is good status
unsigned stripAndWrite(const std::string& fname, bool standalone) {
  XERCES_CPP_NAMESPACE_USE
  using xmlBase::Dom;

  xmlBase::XmlParser parser;

  std::string outname = fname + std::string("-stripped");

  DOMDocument* doc = parser.parse(fname.c_str());
  if (!doc) {
    std::cerr << "Parse of file " << fname << " failed" << std::endl;
    return 1;
  }

  /* DOMElement* docElt = */
  doc->getDocumentElement();
  //  Dom::stripComments(docElt);
  Dom::stripComments(doc);

  bool status = Dom::writeIt(doc, outname.c_str(), standalone);
  return (status) ? 0 : 1;
}
  
  
