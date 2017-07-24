// $Header$
#ifndef xmlBase_DocClient_h
#define xmlBase_DocClient_h

// #include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XercesDefs.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMNode;
XERCES_CPP_NAMESPACE_END

namespace xmlBase {
  //! Pure virtual class whose only purpose is to provide a 
  //! handler for clients of DocMan
  class DocClient {
  public:
    DocClient() {}
    virtual ~DocClient() {}

    //! Must be overridden by derived classes
    virtual void handleChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* node)=0;  

    //! Give DocMan a way to identify clients and detect duplicates; 
    //! must be overridden by derived classes
    virtual const std::string& getName()=0;
  };
}

#endif
