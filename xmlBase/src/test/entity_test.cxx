// Compare handling of elements in the physical file passed to parser
// versus those coming from a separate file, included via entity reference

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <string>
#include <iostream>

#include "facilities/commonUtilities.h"

XERCES_CPP_NAMESPACE_USE
class XmlErrorHandler : public ErrorHandler  {
public:
  XmlErrorHandler(std::ostream& errOut=std::cerr) : m_errOut(errOut)
  {resetErrors();}
  ~XmlErrorHandler() {}
  void warning(const SAXParseException& exception);
  void error(const SAXParseException& exception);
  void fatalError(const SAXParseException& exception);
  void resetErrors();

  int getWarningCount() const {return m_nWarning;}
  int getErrorCount() const {return m_nError;}
  int getFatalCount() const {return m_nFatal;}

private: 
  int m_nWarning, m_nError, m_nFatal;
  std::ostream& m_errOut;
};                  // end class definition

// forward declaration
void Process(DOMDocument* doc, const char* eltName);


//  Start here
int main(int argc, char* argv[]) {
  facilities::commonUtilities::setupEnvironment();
  std::string infile;
  if (argc < 2) { 
    infile=facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"),"simpleDoc.xml");
  }
  else {
    infile = std::string(argv[1]);
  }

  try {
    XMLPlatformUtils::Initialize();
  }
  catch(const XMLException& toCatch)
  {  // may want to redirect in Gaudi environment
    char*  charMsg = XMLString::transcode(toCatch.getMessage());
    std::string msg = std::string(charMsg);
    XMLString::release(&charMsg);
    //        std::string msg(Dom::transToChar(toCatch.getMessage()));
    std::cerr << "Error during Xerces-c Initialization.\n"
              << "  Exception message:" << msg << std::endl;
    return 0;
  }

  const char* inChars = infile.c_str();
  XMLCh* xmlchPath = XMLString::transcode(inChars);
  XercesDOMParser* parser = new XercesDOMParser();

  XmlErrorHandler* errorHandler = new XmlErrorHandler();
  parser->setErrorHandler(errorHandler);

  // The following made no difference in the output
  //  parser->useScanner(XMLUni::fgDGXMLScanner);
  // According to documentation we shouldn't need this, but
  // can't hurt
  parser->setValidationScheme(AbstractDOMParser::Val_Auto);
  

  std::cout << "create entity reference flag has default (true) value" << 
    std::endl;
  parser->parse(xmlchPath);
  DOMDocument* doc = parser->getDocument();
  std::cout << "Document successfully parsed" << std::endl;
  std::cout << "processing local elements.. " << std::endl;
  Process(doc, "const");
  std::cout << std::endl << "processing external elements.. " << std::endl;
  Process(doc, "extConst");

  parser->reset();
  parser->setCreateEntityReferenceNodes(false);

  std::cout << std::endl << std::endl \
            << "create entity reference flag has value false" << std::endl;
  parser->parse(xmlchPath);
  DOMDocument* expandDoc = parser->getDocument();
  std::cout << "Document successfully parsed" << std::endl;
  std::cout << "processing local elements.. " << std::endl;
  Process(expandDoc, "const");
  std::cout << "processing external elements.. " << std::endl;
  Process(expandDoc, "extConst");

}

void Process(DOMDocument* doc, const char* eltName) {
  XMLCh* xmlchEltName = XMLString::transcode(eltName);
  XMLCh* xmlchName = XMLString::transcode("name");
  XMLCh* xmlchValue = XMLString::transcode("value");
  XMLCh* xmlchModified = XMLString::transcode("modified");
  if (!doc) {
    std::cerr << "invalid document " << std::endl;
    return;
  }

  // Find all elements with supplied tag name
  DOMNodeList* constList = doc->getElementsByTagName(xmlchEltName);
  unsigned int nElt = constList->getLength();
  for (unsigned int iElt = 0; iElt < nElt; iElt++) {
    try {  
      bool mismatch = false;
      DOMNode* item = constList->item(iElt);
      DOMElement* itemElt = dynamic_cast<DOMElement *>(item);
      DOMElement* byIdElt;
      std::cout << std::endl << eltName << " #" << iElt 
                << " Address as node:  " 
                << item << " and as element: " << itemElt << std::endl;
      const XMLCh* xmlchNamevalue = itemElt->getAttribute(xmlchName);
      if (XMLString::stringLen(xmlchNamevalue) > 0 ) {
        char* namevalue = XMLString::transcode(xmlchNamevalue);
        std::cout << "element has name " << namevalue << std::endl;
        byIdElt = doc->getElementById(xmlchNamevalue);
        std::cout << "Address from getElementById: " << byIdElt << std::endl;
        if (byIdElt != itemElt) {
          mismatch = true;
          std::cout << "**** Address mismatch " << std::endl << std::endl;
        }
        XMLString::release(&namevalue);
      }
      std::cout << "Modifying value attribute using DOM_Element address" << std::endl;
      itemElt->setAttribute(xmlchValue, xmlchModified);
      if (mismatch) {
        std::cout << "Modifying value attribute using looked-up address" 
                  << std::endl;
        byIdElt->setAttribute(xmlchValue, xmlchModified);

      }
    }
    catch (DOMException ex) {
      int code = ex.code;
      std::cout << "***** Processing failed for element #" << iElt 
                << " with DOMException, code = " 
                << code << std::endl << std::endl;
    }
  }
  
}




// XmlErrorHandler implementation
void XmlErrorHandler::warning(const SAXParseException&) {
  m_nWarning++;
}

void XmlErrorHandler::error(const SAXParseException& toCatch) {
  char* charSyst = XMLString::transcode(toCatch.getSystemId());
  std::string systemId(charSyst);
  XMLString::release(&charSyst);
  char* charMsg = XMLString::transcode(toCatch.getMessage());
  std::string msg(charMsg);
  XMLString::release(&charMsg);
  m_nError++;
  std::cerr << "Error at file \"" << systemId
            << "\", line " << toCatch.getLineNumber()
            << ", column " << toCatch.getColumnNumber()
            << "\n   Message: " << msg << "\n\n";
}

void XmlErrorHandler::fatalError(const SAXParseException& toCatch) {

  // getMessage returns type XMLCh*
  char* charMsg = XMLString::transcode(toCatch.getMessage());
  std::string msg(charMsg);
  XMLString::release(&charMsg);
  m_nFatal++;
  if (!(toCatch.getSystemId()) ) {
    std::cerr << "Fatal XML parse error: no such file "
              << "\n Message: " << msg << "\n\n";
  }
  else {
    char* charSyst = XMLString::transcode(toCatch.getSystemId());
    std::string systemId(charSyst);
    XMLString::release(&charSyst);
    std::cerr << "Fatal error at file \"" 
              << systemId
              << "\", line " << toCatch.getLineNumber()
              << ", column " << toCatch.getColumnNumber()
              << "\n   Message: " << msg << "\n\n";
  }
}
void XmlErrorHandler::resetErrors() {
  m_nWarning = m_nError = m_nFatal = 0;
}
