// $Header$
//  Author:  J. Bogart

#ifndef xmlBase_XmlErrorHandler_h
#define xmlBase_XmlErrorHandler_h

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <iostream>
#include <string>

namespace xmlBase {
  XERCES_CPP_NAMESPACE_USE

  //! Exception class for XmlParser, XmlErrorHandler
  class ParseException : std::exception {
  public:
    ParseException(const std::string& extraInfo = "") : std::exception(),
      m_name("ParseException"), m_extra(extraInfo) {}
    virtual ~ParseException() throw() {}
    virtual std::string getMsg() {
      std::string msg = m_name + ": " + m_extra;
      return msg;}
    virtual const char* what() {
      return m_extra.c_str();
    }
  protected: 
    std::string m_name;
  private:
    std::string m_extra;
  };


  /// This class handles errors during parsing of an xml file.
  /// By default output will go to cerr, but if @a throwErrors is
  /// set to true an exception of type xml::ParseException will be
  /// thrown instead
  class XmlErrorHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler  {
  public:
    // Constructor, destructor have little to do.
    XmlErrorHandler(bool throwErrors = false) : m_throwErrors(throwErrors)
      {resetErrors();}
    ~XmlErrorHandler() {}
    // The following methods all override pure virtual methods
    // from ErrorHandler base class.
    /// Keep count of warnings seen
    void warning(const SAXParseException& exception);
    /// Output row, column of parse error and increment counter
    void error(const SAXParseException& exception);
    /// Output row, column of fatal parse error and increment counter
    void fatalError(const SAXParseException& exception);
    /// Clear counters
    void resetErrors();

    // "get" methods
    int getWarningCount() const {return m_nWarning;}
    int getErrorCount() const {return m_nError;}
    int getFatalCount() const {return m_nFatal;}

  private: 
    int m_nWarning;
    int m_nError;
    int m_nFatal;
    bool m_throwErrors;
  };                  // end class definition
}                     // end namespace xml

#endif
