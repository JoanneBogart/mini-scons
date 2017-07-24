// $Header$
// author:  J. Bogart

// For some obscure reason, adding the following line keeps gcc3.2
// happy.  Otherwise, when the very same file is indirectly included
// in XMLScanner.hpp below, the compile bombs
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include "xmlBase/EResolver.h"
#include "xmlBase/Dom.h"
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include "facilities/Util.h"
// #include <xercesc/parsers/DOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/internal/XMLScanner.hpp>
#include <xercesc/sax/Locator.hpp>

namespace xmlBase {
  XERCES_CPP_NAMESPACE_USE
  
  EResolver::EResolver() : m_nEntity(0) {
    m_entities.clear();
  }

  // According to the XML spec, the systemId should be a URI (see
  // http://www.ietf.org/rfc/rfc2396.txt for the really gory details).
  // I *think* that, even though $ is reserved in some contexts, it
  // can be used the way we would be likely to use it, as part of
  // the introducer for an environment variable $(stuff)
  // The algorithm is
  //    if publicId is non-null attempt to use it, in particular
  //    expand environment variable references if present
  //
  //    otherwise expand and use systemId (which must be present)  
  

  InputSource*
  EResolver::resolveEntity(XERCES_CPP_NAMESPACE_QUALIFIER XMLResourceIdentifier* xmlRI) {
    const XMLCh* publicId = xmlRI->getPublicId();
    const XMLCh* systemId = xmlRI->getSystemId();
    const XMLCh* baseURI = xmlRI->getBaseURI();
    if (publicId != 0) {
      if (XMLString::stringLen(publicId) > 0) {
        return translateEnv(publicId, baseURI); 
      }    
    }
    return translateEnv(systemId, baseURI);
  }

  InputSource* EResolver::translateEnv(const XMLCh * id, const XMLCh * baseURI)
  {
    // Make a std::string out of the spec we're passed
    char* chId = XMLString::transcode(id);  
    std::string *idStr = new std::string(chId);
    XMLString::release(&chId);
    int nSub = facilities::Util::expandEnvVar(idStr);

    m_entities.push_back(idStr);
    
    if (!nSub) return 0;     
    else if (nSub < 0) {
      std::cout << "Bad entity name " << idStr << std::endl;
      return 0;
    }

    // Otherwise something really did get translated, so make
    // a new InputSource
    XMLCh* realName = XMLString::transcode(idStr->c_str());

    // Apparently the InputSource already gets deleted by somebody else,
    // so we don't have to keep track of it
    LocalFileInputSource*  src = new LocalFileInputSource(baseURI, realName);
    XMLString::release(&realName);
    return src;

  }

  void EResolver::clean() {
    for (unsigned i = 0; i < m_entities.size(); i++) {
      delete m_entities[i];
    }
    m_entities.clear();
    m_nEntity = 0;
  }

  EResolver::~EResolver() {
    clean();
  }
}
