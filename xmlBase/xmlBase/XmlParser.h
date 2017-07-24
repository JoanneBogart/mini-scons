// $Header$
// Author:  J. Bogart

#ifndef xmlBase_XmlParser_h
#define xmlBase_XmlParser_h

#include "xmlBase/XmlErrorHandler.h"
 // following indirectly includes DOMDocument, DOMElement...
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <string>
#include <iosfwd>


namespace xmlBase {
  /// This class provides an interface to the Xerces DOM parser
  /// with validation turned on if the file to be parsed has a dtd.
  class EResolver;
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
  using XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser;


  class XmlParser {
  public:
    XmlParser(bool throwErrors = false);

    /// Call this method to turn on schema processing (else it's off)
    void doSchema(bool doit);
    ~XmlParser();

    /** In case we want to force use of a certain schema
     Set @a ns to 'false' to set NoNamespaceSchemaLocation
    */
    void setSchemaLocation(const std::string& loc, bool ns=true);

    /// Parse an xml file, returning document node if successful
    DOMDocument* parse(const char* const filename, 
                       const std::string& docType=std::string(""));


    /// Parse an xml file as a string, returning document node if successful
    DOMDocument* parse(const std::string& buffer,
                       const std::string& docType=std::string("") );

    /// Reset the parser so it may be used to parse another document (note
    /// this destroys old DOM)
    void reset() {m_parser->reset();}
  private:
    /// Xerces-supplied parser which does the real work
    XercesDOMParser* m_parser;
    XmlErrorHandler* m_errorHandler;
    /// Entity resolver
    EResolver*       m_resolver;    
    bool             m_throwErrors;
    bool             m_errorsOccurred;
    bool             m_doSchema;
    //    static int       s_didInit;
  };
}
#endif
