// $Header$
// author:  J. Bogart

#include "xmlBase/XmlErrorHandler.h"
#include "xmlBase/Dom.h"
#include "facilities/Util.h"
#include <string>

namespace xmlBase {
  using XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException;
  using XERCES_CPP_NAMESPACE_QUALIFIER XMLString;

  void XmlErrorHandler::warning(const SAXParseException&) {
    m_nWarning++;
  }

  void XmlErrorHandler::error(const SAXParseException& toCatch) {
    char* charSyst = XMLString::transcode(toCatch.getSystemId());
    std::string systemId(charSyst);
    //    std::string systemId(Dom::transToChar(toCatch.getSystemId()));
    XMLString::release(&charSyst);
    char* charMsg = XMLString::transcode(toCatch.getMessage());
    std::string msg(charMsg);
    XMLString::release(&charMsg);

    std::string line;
    facilities::Util::itoa((int)toCatch.getLineNumber(), line);
    std::string col;
    facilities::Util::itoa((int)toCatch.getColumnNumber(), col);

    m_nError++;

    std::string errMsg("Error at file \"");
    errMsg += systemId + "\", line " + line + ",column " + col;
    errMsg += "\n Message: " + msg;
    if (m_throwErrors) {
      throw ParseException(errMsg);
    } else {
      std::cerr << errMsg << "\n\n";
    }
  }

  void XmlErrorHandler::fatalError(const SAXParseException& toCatch) {

    // getMessage returns type XMLCh*

    char* charMsg = XMLString::transcode(toCatch.getMessage());
    std::string msg(charMsg);
    XMLString::release(&charMsg);

    m_nFatal++;

    if (!(toCatch.getSystemId()) ) {
      std::string errMsg("Fatal XML parse error: no such file \n Message: ");
      errMsg += msg;
      if (m_throwErrors) {
        throw ParseException(errMsg);
      } else {
        std::cerr << errMsg << "\n\n";
      }
    }
    else {
      char* charSyst = XMLString::transcode(toCatch.getSystemId());
      std::string systemId(charSyst);
      XMLString::release(&charSyst);

      std::string line; 
      facilities::Util::itoa((int)toCatch.getLineNumber(), line);
      std::string col; 
      facilities::Util::itoa((int)toCatch.getColumnNumber(), col);
      std::string errMsg("Fatal error at file \"");
      errMsg += systemId + "\", line " + line + ",column " + col;
      errMsg += "\n Message: " + msg;

      if (m_throwErrors) {
        throw ParseException(errMsg);
      } else {
        std::cerr << errMsg << "\n\n";
      }
    }
  }
  void XmlErrorHandler::resetErrors() {
    m_nWarning = m_nError = m_nFatal = 0;
  }
}
