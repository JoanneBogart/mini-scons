// $Header$
// Author: J. Bogart

#ifndef  xmlBase_EResolver_h
#define  xmlBase_EResolver_h

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <vector>
#include <string>

XERCES_CPP_NAMESPACE_BEGIN 
class XercesDOMParser;
class XMLResourceIdenifier;
XERCES_CPP_NAMESPACE_END

namespace xmlBase {

  class EResolver : public XERCES_CPP_NAMESPACE_QUALIFIER XMLEntityResolver {
  public:
    EResolver();
    virtual ~EResolver();
   
    virtual XERCES_CPP_NAMESPACE_QUALIFIER InputSource* 
    resolveEntity(XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier* xmlRI);
    
    void clean();  // Return resources used during a parse
  private:
    // Might want other stuff here for bookkeeping purposes
    unsigned m_nEntity;
    std::vector<std::string*> m_entities; 

    XERCES_CPP_NAMESPACE_QUALIFIER InputSource* 
    translateEnv(const XMLCh * id, const XMLCh * baseURI);
  };

}
#endif
