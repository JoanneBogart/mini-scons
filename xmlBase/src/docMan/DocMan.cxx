//  $Header$

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include "xmlBase/docMan/DocMan.h"
#include "xmlBase/docMan/DocClient.h"

namespace xmlBase {
XERCES_CPP_NAMESPACE_USE

  DocMan* DocMan::m_self = 0;

  DocMan::DocMan() {
    m_self = this;
    m_meFirst = 0;
    m_parser = new xmlBase::XmlParser();
  }

  DocMan* DocMan::getPointer() {
    if (m_self == 0) {
      m_self = new DocMan();
    }
    return m_self;
  }

  bool DocMan::parse(const std::string& filename, const std::string& docType) {
    //    using xercesc::DOMDocument;
    //    using xercesc::DOMElement;

    DOMDocument* doc = m_parser->parse(filename.c_str(), docType);
    if (doc == 0) return false;

    if (m_meFirst != 0) m_meFirst->handleChild(doc);

    DOMElement* root = doc->getDocumentElement();

    DOMElement* child = xmlBase::Dom::getFirstChildElement(root);

    while (child != 0) {
      std::string eltName = Dom::getTagName(child);
      if (m_meFirst != 0) m_meFirst->handleChild(child);

      ClientList* list = findList(eltName);
      if (list) list->invoke(child);

      child = xmlBase::Dom::getSiblingElement(child);
    }

    // Clean up call for privileged client
    if (m_meFirst != 0) m_meFirst->handleChild(0);

    m_parser->reset();
    return true;
  }

  bool DocMan::regClient(const std::string& eltName, DocClient* client) {
    ClientList* curList = findList(eltName);
    if (curList == 0) {
      curList = new ClientList(eltName);
      m_lists.push_back(curList);
    }
    return curList->add(client);
  }

  bool DocMan::regMeFirst(DocClient* client) {
    if (m_meFirst != 0)       return false;

    m_meFirst = client;
    return true;
  }

  DocMan::ClientList* DocMan::findList(const std::string& eltName) {
    unsigned int ix;

    for (ix = 0; ix < m_lists.size(); ix++) {
      ClientList* cur = m_lists[ix];
      if (eltName.compare(cur->getName()) == 0) return cur;
    }
    return 0;
  }

  bool DocMan::ClientList::add(DocClient* client, bool front) {
    ClientsIt it = m_clients.begin();
    const std::string& clientName = client->getName();
    while (it != m_clients.end() ) {
      // can't register for the same element type more than once
      if (clientName.compare((*it)->getName()) == 0 ) return 0;
      ++it;
    }
    if (front) m_clients.insert(m_clients.begin(), client);
    else m_clients.push_back(client);

    return true;
  }

  // This is just here for completeness.  Don't need a real 
  // implementation yet because no one calls it
  bool DocMan::ClientList::remove(DocClient* client) {
    client->getName(); // just to avoid compile-time warning
    return true;
  }

  void DocMan::ClientList::invoke(DOMElement* elt) {
    ClientsIt it = m_clients.begin();
    while (it != m_clients.end() ) {
      (*it)->handleChild(elt);
      ++it;
    }
  }

  // Still need destructors
  DocMan::ClientList::~ClientList() {
    m_clients.clear();
  }

  DocMan::~DocMan() {
    ListsIt it = m_lists.begin();
    while (it != m_lists.end()) {
      delete (*it);
      ++it;
    }
    delete m_parser;
  }
}
