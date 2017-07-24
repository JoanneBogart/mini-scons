// $Header$
// Author: J. Bogart

#include <iostream>   // for endl, cerr,...
#include "xmlBase/XmlParser.h"
#include "xmlBase/EResolver.h"
#include "xmlBase/Dom.h"
#include "facilities/Util.h"
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/util/PlatformUtils.hpp>

namespace {
  XERCES_CPP_NAMESPACE_USE
  bool  checkDocType(const DOMDocument* doc, const std::string docType) {
    bool ret = false;
    const DOMDocumentType* typeDecl = doc->getDoctype();
    if (typeDecl != 0) {
      const XMLCh* name = typeDecl->getName();
      XMLCh* transDocType = XMLString::transcode(docType.c_str());
      ret = XMLString::equals(name, transDocType);
      XMLString::release(&transDocType);

    }
    return ret;
  }
}



namespace xmlBase {
  XERCES_CPP_NAMESPACE_USE

  XmlParser::XmlParser(bool throwErrors) : m_throwErrors(throwErrors),
                                           m_doSchema(false) {
   // no-op if init was already done
    if (!Dom::doInit()) {
      if (m_throwErrors) {
        throw ParseException("Error during Xerces-c initializaation \n");
      } else {
        return;
      }
    }

    m_parser = new XercesDOMParser();

    m_errorHandler = new XmlErrorHandler(throwErrors);

    // According to documentation we shouldn't need this, but
    // just in case..
    m_parser->setValidationScheme(AbstractDOMParser::Val_Auto);
    /*
    m_parser->setDoNamespaces(true);
    m_parser->setDoSchema(true);
    m_parser->setValidationSchemaFullChecking(true);
    */

    m_resolver = new EResolver();
    m_parser->setXMLEntityResolver(m_resolver);
    m_parser->setErrorHandler(m_errorHandler);

    // Don't keep entity reference nodes.  We don't use them
    // and they can cause confusion
    m_parser->setCreateEntityReferenceNodes(false);
    // Have to leave this line out for now since it causes weirdness
    // with DOMDocument::getElementById

    // As long as we don't need to re-serialize, we can forget about
    // ingnorable white space and save a bit of memory.
    m_parser->setIncludeIgnorableWhitespace(false);
  }
  void XmlParser::doSchema(bool doit) {
    m_doSchema = doit;  // just to keep a record of what we think we're doing

    //    m_parser->setValidationScheme(AbstractDOMParser::Val_Always);
    m_parser->setDoNamespaces(doit);
    m_parser->setDoSchema(doit);
    m_parser->setValidationSchemaFullChecking(doit);
  }

  void XmlParser::setSchemaLocation(const std::string& loc, bool ns) {
    if (ns) m_parser->setExternalSchemaLocation(loc.c_str());
    else m_parser->setExternalNoNamespaceSchemaLocation(loc.c_str());
    return;
  }

  XmlParser::~XmlParser() {
    delete m_errorHandler;
    delete m_resolver;
    // delete m_parser;  temporary, until we can figure out why there 
    // are sometimes problems while freeing this piece of memory.
  }
 
  DOMDocument* XmlParser::parse(const char* const filename, 
                                const std::string& docType) {
    XERCES_CPP_NAMESPACE_USE
    // Reset from any previous parse
    m_errorsOccurred = false;
    m_resolver->clean();

    // translate environment variables, if any
    std::string fname(filename);
    int nExpand = facilities::Util::expandEnvVar(&fname);
    if (nExpand < 0) {
      std::string errMsg("xmlBase::XmlParser::parse ");
      errMsg += "Filename arg. contained untranslatable env. variables";
      if (m_throwErrors) {
        throw 
          ParseException(errMsg);
      } else {
        std::cerr << errMsg << std::endl;
        return 0;
      }
    }
    
    // parse file
    try {
      //      XMLCh* filenameXMLCh = Dom::transToXMLCh(filename);
      XMLCh* filenameXMLCh = XMLString::transcode(fname.c_str());
      LocalFileInputSource fileSource(filenameXMLCh);
      m_parser->parse(fileSource);
      XMLString::release(&filenameXMLCh);
    }
    catch   (const XMLException& e) {
      char* charMsg = XMLString::transcode(e.getMessage());
      std::string msg = std::string(charMsg);
      XMLString::release(&charMsg);
      std::string errMsg("xmlBase::XmlParser::parse ");
      errMsg += "Error occurred while parsing file ";
      errMsg += fname;
      m_errorsOccurred = true;
      m_parser->reset();
      if (m_throwErrors) {
        errMsg += "  ";
        errMsg += msg;
        throw ParseException(errMsg);
      }
      else {
        std::cerr << errMsg << std::endl
                  << "Message: " << msg << std::endl;
        return 0;
      }

    }

    if (m_errorHandler->getFatalCount() + m_errorHandler->getErrorCount()) {
      // Put out message
      m_parser->reset();  // may be used to parse new input
      return 0;
    }
    if (docType != std::string("")) { // check actual docType matches requested
      bool ok = checkDocType(m_parser->getDocument(), docType);

      if (ok) return m_parser->getDocument();

      // Either there was no docType at all or it didn't match
      m_parser->reset();
      return 0;
    }

    return m_parser->getDocument();
    // if ok return document node; else null
  }

  // this is the string version, very stupidly mostly copied from above. 
  DOMDocument* XmlParser::parse(const std::string& input, 
                                const std::string& docType) {
  XERCES_CPP_NAMESPACE_USE
     
  // parse file
    m_errorsOccurred = false;
    try {
      char fakeid =99;
      XMLCh* buffer = XMLString::transcode(input.c_str());
      //      XMLCh* buffer = Dom::transToXMLCh(input.c_str());

      unsigned int byteCount = sizeof(XMLCh) * input.length();  

      MemBufInputSource source((const unsigned char*)buffer, byteCount, 
                               &fakeid, false);
      m_parser->parse(source);
      XMLString::release(&buffer);
    }
    catch   (const XMLException& e) {
      char* charMsg = XMLString::transcode(e.getMessage());
      std::string msg = std::string(charMsg);
      XMLString::release(&charMsg);
      m_errorsOccurred = true;
      m_parser->reset();

      std::string errMsg("xmlBase::XmlParser::parse ");
      errMsg += 
        "An error occurred while parsing MemBufInputSource\n  Message: ";
      if (m_throwErrors) {
        throw ParseException(errMsg);
      } else {
        std::cerr << errMsg << std::endl;
        return 0;
      }
      //      std::string msg(Dom::transToChar(e.getMessage()));
      std::cerr << 
        "An error occurred while parsing MemBufInputSource\n  Message: " <<
        msg << std::endl;
      return 0;
    }

    if (m_errorHandler->getFatalCount() + m_errorHandler->getErrorCount()) {
      // Put out message
      m_parser->reset();  // may be used to parse new input
      return 0;
    }

    if (docType != std::string("")) { // check actual docType matches requested
      bool ok = checkDocType(m_parser->getDocument(), docType);

      if (ok) return m_parser->getDocument();

      // Either there was no docType at all or it didn't match
      m_parser->reset();
      return 0;
    }

    return m_parser->getDocument();
    // if ok return document node; else null


  }

}     // end namespace
