// $Header$
// Author:  J. Bogart
//
// Implementation of xmlBase::Dom, a convenient place to put static
// utilities which enhance DOM api.

#include "xmlBase/Dom.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMCharacterData.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMComment.hpp>
#include <xercesc/dom/DOMTreeWalker.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "facilities/Util.h"

#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdio>

namespace {
  XERCES_CPP_NAMESPACE_USE


  DOMTreeWalker* makeWalker(const DOMNode* node) {
    if (!node) return 0;
    DOMDocument* doc = node->getOwnerDocument();
    unsigned long whatToShow = DOMNodeFilter::SHOW_ELEMENT;

    bool expand = true;
    DOMNode* casted = const_cast<DOMNode*>(node);
    // Show only elements;  Expand entity references
    DOMTreeWalker* walk = doc->createTreeWalker(casted, whatToShow, 0, expand);
    return walk;
  }
}

  
namespace xmlBase {
  XERCES_CPP_NAMESPACE_USE
  int Dom::s_didInit = 0;

  XMLLCPTranscoder*  Dom::transcoder = 0;
  char*              Dom::transBuf = 0;

  unsigned int       Dom::transBufSize = 1000;
  XMLCh*             Dom::xmlchBuf = 0;

  unsigned int       Dom::xmlchBufSize = 200;
  XMLCh*             Dom::xmlchStar = 0;


  bool Dom::doInit() {
    if (s_didInit == 1) return true;

    try {
      XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& toCatch)
    {  // may want to redirect in Gaudi environment
      char*  charMsg = XMLString::transcode(toCatch.getMessage());
      std::string msg = std::string(charMsg);
      XMLString::release(&charMsg);
      
      std::string errMsg("Error during Xerces-c Initialization: \n");
      errMsg += " Exception message: ";
      errMsg += msg;
      std::cerr << errMsg << std::endl;
      return false;
    }
    s_didInit = 1;
    return true;
  }
  bool Dom::didInit() {
    return (s_didInit == 1);
  }

  DOMElement* Dom::findFirstChildByName(const DOMElement* parent, 
                                        const char* const name) {
    
    if (!xmlchStar) xmlchStar = XMLString::transcode("*");    
    if (!(parent)) {
      throw NullNode("from xmlBase::Dom::findFirstChildByName. Null parent arg.");
    }

    XMLCh* xmlchName = XMLString::transcode(name);

    DOMTreeWalker* walk = makeWalker(parent);
    DOMElement* child = static_cast<DOMElement*>(walk->firstChild());
    if (XMLString::equals(xmlchName, xmlchStar)) {
      XMLString::release(&xmlchName);
      return child;
    }

    if (!child) {
      XMLString::release(&xmlchName);
      return 0;
    }
    // otherwise cycle through children, looking for first with right tag name
    while (! XMLString::equals(child->getNodeName(), xmlchName)) {
      child = static_cast<DOMElement*>(walk->nextSibling());
      if (!child) {
        XMLString::release(&xmlchName);
        return 0;
      }
    }


    XMLString::release(&xmlchName);

    walk->release();
    return child;
  }

  DOMElement* Dom::findFirstChildByName(const DOMElement* parent, 
                                        const std::string name) {
    return findFirstChildByName(parent, name.c_str());
  }


  // Leave this alone for now.  Entities are not likely to cause
  // trouble here.
  DOMElement* Dom::getSiblingElement(const DOMNode* child) {
    if (!child) {
      throw NullNode("from xmlBase::Dom::getSiblingElement. Null argument");
    }

    DOMNode* sib = child->getNextSibling();

    while (sib) {
      if (sib->getNodeType() == DOMNode::ELEMENT_NODE) {
        DOMElement* eltSib = static_cast<DOMElement*> (sib);

        return eltSib;
      }
      sib = sib->getNextSibling();
    }

    return 0;
  }

  DOMElement* Dom::getFirstChildElement(const DOMNode* parent) {
    const DOMElement* elt = static_cast<const DOMElement*>(parent);
    return findFirstChildByName(elt, "*");
  }

  DOMElement* Dom::getElementById(const DOMDocument* doc, 
                                  const std::string& id) {
    XMLCh* idChars = XMLString::transcode(id.c_str());
    DOMElement* elt = doc->getElementById(idChars);
    XMLString::release(&idChars);
    return elt;
  }

  std::string Dom::getNodeName(const DOMNode* node) {
    if (!node) {
      throw NullNode("Null node argument supplied to xmlBase::Dom::getNodeName");
    }
    const XMLCh* name = node->getNodeName();
    if (!name) return std::string("");

    char* chrName = XMLString::transcode(name);

    std::string strName = std::string(chrName);
    XMLString::release(&chrName);
    return strName;
  }

  std::string Dom::getTagName(const DOMElement* elt) {
    return Dom::getNodeName(elt);
  }

  bool Dom::checkTagName(const DOMElement* element, 
                         const std::string& tagName) {
   if (!element) {
     throw NullNode("Null dom element argument to xmlBase::Dom::checkTagName");
   }
   if (tagName == std::string("*")) return true;
   XMLCh* xmlchTagName = XMLString::transcode(tagName.c_str());
   bool ret = XMLString::equals(xmlchTagName, element->getTagName());
   XMLString::release(&xmlchTagName);
   return ret;
  }
   


  void Dom::getChildrenByTagName(const DOMElement* parent, 
                                 const std::string& tagName,
                                 std::vector<DOMElement*>& children,
                                 bool clear) {
    if (!parent) {
      throw NullNode("from xmlBase::Dom::getChildrenByTagName. Null parent arg");
    }
    if (clear) children.clear();
    DOMElement* child = getFirstChildElement(parent);
    while (child != 0) {
      if (checkTagName(child, tagName)) {
        children.push_back(child);
      }
      child = getSiblingElement(child);
    }
  }


  void Dom::getDescendantsByTagName(const DOMElement* parent, 
                                    const std::string& tagName,
                                    std::vector<DOMElement*>& children,
                                    bool clear) {
    if (parent == 0) {
      throw NullNode("from xmlBase::Dom::getChildrenByTagName. Null parent arg");
    }
    if (clear) children.clear();
    XMLCh* xmlchTagName = XMLString::transcode(tagName.c_str());;
    DOMNodeList* list = 
      parent->getElementsByTagName(xmlchTagName);
    XMLString::release(&xmlchTagName);
    unsigned int nItem = list->getLength();
    for (unsigned int iItem = 0; iItem < nItem; iItem++) {
      children.push_back(static_cast<DOMElement *>(list->item(iItem)));
    }
  }

  void Dom::getAttributeNodeMap(const DOMNode* node,
                                std::map<std::string, DOMNode*>& atts,
                                bool clear) {

    if (clear) atts.clear();
    DOMNamedNodeMap* nodeMap = node->getAttributes();
    unsigned int nAtts = nodeMap->getLength();

    for (unsigned iAtt = 0; iAtt < nAtts; iAtt++) {
      DOMNode* attNode = nodeMap->item(iAtt);
      atts[xmlBase::Dom::getNodeName(attNode)] = attNode;
    }
  }

  bool Dom::hasAttribute(const DOMNode* node, const char* attName) {
    bool ret = false;
    if (node == 0) return ret;
    if (node->getNodeType() != DOMNode::ELEMENT_NODE ) return ret;

    const DOMElement* elt = static_cast<const DOMElement*>(node);
    XMLCh* xmlchAttName = XMLString::transcode(attName);
    ret = (elt->getAttributeNode(xmlchAttName) != 0);
    XMLString::release(&xmlchAttName);
    return ret;
  }

  std::string   Dom::getAttribute(const DOMElement* elt, const char* attName) {
    if (elt == 0) {
      throw NullNode("from xmlBase::Dom::getAttribute. Null argument");
    }

    XMLCh* xmlchAttName = XMLString::transcode(attName);
    const XMLCh*  attValue = elt->getAttribute(xmlchAttName);
    XMLString::release(&xmlchAttName);

    if (attValue == 0) return std::string("");

    char* chrAttValue = XMLString::transcode(attValue);
    std::string strValue = std::string(chrAttValue);
    XMLString::release(&chrAttValue);
    return strValue;
  }


  std::string   Dom::getAttribute(const DOMElement* elt, 
                                  std::string attName) {

    return getAttribute(elt, attName.c_str());
  }
 
  std::string  Dom::getAttribute(const DOMNode* elt, const char* attName) {
    if (elt == 0) {
      throw NullNode("from xmlBase::Dom::getAttribute. Null argument");
    }

    if (elt->getNodeType() != DOMNode::ELEMENT_NODE) {
      throw NullNode("from xmlBase::Dom::getAttribute. Non-element argument");
    }
    return getAttribute(static_cast<const DOMElement*>(elt), attName);
  }

  std::string  Dom::getAttribute(const DOMNode* elt, 
                            std::string attName) {
    return getAttribute(elt, attName.c_str());
  }


  double Dom::getDoubleAttribute(const DOMNode* elt, std::string attName) {
    std::string stringValue = getAttribute(elt, attName);
    
    if (stringValue == std::string("") ) {
      throw
        NullNode("from xmlBase::Dom::getDoubleAttribute.  non-existent attribute");
    }
    try {
      return facilities::Util::stringToDouble(stringValue);
    }
    catch (facilities::WrongType ex) {
      std::cerr << ex.getMsg();
      throw WrongAttributeType("from xmlBase::Dom::getDoubleAttribute");
    }
  }

  unsigned Dom::getDoublesAttribute(const DOMNode* elt, std::string attName,
                                std::vector<double>& values, bool clear) {
    unsigned nVals = 0;
    std::vector<std::string> tokens;
    if (clear) values.clear();
    std::string stringValue = getAttribute(elt, attName);
    if (stringValue.size() == 0) return nVals;

    facilities::Util::stringTokenize(stringValue, std::string(" "), tokens);

    try {
      unsigned nToken = tokens.size();
      for (unsigned iToken = 0;  iToken < nToken; iToken++) {
        values.push_back(facilities::Util::stringToDouble(tokens[iToken]));
        nVals++;
      }
    }
    catch (facilities::WrongType ex) {
      std::cerr << ex.getMsg();
      throw WrongAttributeType("from xmlBase::Dom::getDoublesAttribute");
    }
    return nVals;
  }
  unsigned Dom::getFloatsAttribute(const DOMNode* elt, std::string attName,
                                   std::vector<float>& values, bool clear) {
    unsigned nVals = 0;
    std::vector<std::string> tokens;
    if (clear) values.clear();
    std::string stringValue = getAttribute(elt, attName);
    if (stringValue.size() == 0) return nVals;

    facilities::Util::stringTokenize(stringValue, std::string(" "), tokens);

    try {
      unsigned nToken = tokens.size();
      for (unsigned iToken = 0;  iToken < nToken; iToken++) {
        values.push_back(facilities::Util::stringToDouble(tokens[iToken]));
        nVals++;
      }
    }
    catch (facilities::WrongType ex) {
      std::cerr << ex.getMsg();
      throw WrongAttributeType("from xmlBase::Dom::getDoublesAttribute");
    }
    return nVals;
  }

  int Dom::getIntAttribute(const DOMNode* elt, std::string attName) {
    std::string stringValue = getAttribute(elt, attName);
    
    if (stringValue == std::string("") ) {
      throw
        NullNode("from xmlBase::Dom::getIntAttribute.  non-existent attribute");
    }
    try {
      return facilities::Util::stringToInt(stringValue);
    }
    catch (facilities::WrongType ex) {
      std::cerr << ex.getMsg();
      throw WrongAttributeType("from xmlBase::Dom::getIntAttribute");
    }
  }

  unsigned Dom::getIntsAttribute(const DOMNode* elt, std::string attName,
                                 std::vector<int>& values, bool clear) {
    unsigned nVals = 0;
    std::vector<std::string> tokens;
    if (clear) values.clear();
    std::string stringValue = getAttribute(elt, attName);
    if (stringValue.size() == 0) return nVals;

    facilities::Util::stringTokenize(stringValue, std::string(" "), tokens);

    try {
      unsigned nToken = tokens.size();
      for (unsigned iToken = 0;  iToken < nToken; iToken++) {
        values.push_back(facilities::Util::stringToInt(tokens[iToken]));
        nVals++;
      }
    }
    catch (facilities::WrongType ex) {
      std::cerr << ex.getMsg();
      throw WrongAttributeType("from xmlBase::Dom::getIntsAttribute");
    }
    return nVals;
  }

  std::string Dom::getText(const DOMNode* textNode) {
    short int nType = textNode->getNodeType();

    if ( (nType != DOMNode::TEXT_NODE) &&
         (nType != DOMNode::COMMENT_NODE) &&
         (nType != DOMNode::CDATA_SECTION_NODE))
    {
      throw WrongNodeType("from xmlBase::Dom::getText, argument not text node");
    }
    const XMLCh* t = textNode->getNodeValue();
    if (t == 0 ) return std::string("");
    char* chrText = XMLString::transcode(t);
    std::string text = std::string(chrText);
    XMLString::release(&chrText);

    return text;
  }


  std::string Dom::getTextContent(const DOMElement* elt) {
    if (elt->getNodeType() != DOMNode::ELEMENT_NODE) {
      throw WrongNodeType("from xmlBase::Dom::getTextContent, argument not element node");
    }
    return (getText(elt->getFirstChild()));
  }

  void  Dom::addAttribute(DOMElement* elt, std::string name, 
                          double value, unsigned int precision) {
    if (elt == 0) {
      throw NullNode("from xmlBase::Dom::addAttribute.  null argument");
    }

    std::ostringstream s;
    s << std::setprecision(precision);
    s << value;

    std::string str = s.str();
    XMLCh* xmlchValue = XMLString::transcode(str.c_str());
    XMLCh* xmlchName = XMLString::transcode(name.c_str());

    try {
      elt->setAttribute(xmlchName, xmlchValue);
    }
    catch (DOMException ex) {
      char* msg = XMLString::transcode(ex.msg);
      std::cerr << "DOMException in xmlBase::Dom::addAttribute(double)" << std::endl;
      std::cerr << "Msg: " << std::string(msg) 
                << "Code: " << ex.code << std::endl;
      XMLString::release(&msg);
      XMLString::release(&xmlchName);
      XMLString::release(&xmlchValue);
      throw ex;
    }
    XMLString::release(&xmlchName);
    XMLString::release(&xmlchValue);
  }


  void  Dom::addAttribute(DOMElement* elt, std::string name, 
                                 int value) {
    if (elt == 0) {
      throw NullNode("from xmlBase::Dom::addAttribute.  Null argument");
    }

    std::ostringstream s;
    s << value;

    std::string str = s.str();

    XMLCh* xmlchValue = XMLString::transcode(str.c_str());
    XMLCh* xmlchName = XMLString::transcode(name.c_str());

    try {
      elt->setAttribute(xmlchName, xmlchValue);
    }
    catch (DOMException ex) {
      char* msg = XMLString::transcode(ex.msg);
      std::cerr << "DOMException in xmlBase::Dom::addAttribute(int)" 
                << std::endl;
      std::cerr << "Msg: " << std::string(msg) 
                << "Code: " << ex.code << std::endl;
      XMLString::release(&msg);
      XMLString::release(&xmlchName);
      XMLString::release(&xmlchValue);
      throw ex;
    }

    XMLString::release(&xmlchName);
    XMLString::release(&xmlchValue);

  }

  void  Dom::addAttribute(DOMElement* elt, std::string name, 
                                 unsigned int value) {
    if (elt == 0) {
      throw NullNode("from xmlBase::Dom::addAttribute.  Null argument");
    }

    std::ostringstream s;
    s << value;

    std::string str = s.str();

    XMLCh* xmlchValue = XMLString::transcode(str.c_str());
    XMLCh* xmlchName = XMLString::transcode(name.c_str());

    try {
      elt->setAttribute(xmlchName, xmlchValue);
    }
    catch (DOMException ex) {
      char* msg = XMLString::transcode(ex.msg);
      std::cerr << "DOMException in xmlBase::Dom::addAttribute(unsigned int)" 
                << std::endl;
      std::cerr << "Msg: " << std::string(msg) 
                << "Code: " << ex.code << std::endl;
      XMLString::release(&msg);
      XMLString::release(&xmlchName);
      XMLString::release(&xmlchValue);
      throw ex;
    }
    XMLString::release(&xmlchName);
    XMLString::release(&xmlchValue);

  }

  void  Dom::addAttribute(DOMElement* elt, std::string name, 
                                 const char* value) {

    if (elt == 0) {
      throw NullNode("from xmlBase::Dom::addAttribute.  Null argument");
    }

    XMLCh* xmlchValue = XMLString::transcode(value);
    XMLCh* xmlchName = XMLString::transcode(name.c_str());

    try {
      elt->setAttribute(xmlchName, xmlchValue);
    }
    catch (DOMException ex) {
      char* msg = XMLString::transcode(ex.msg);
      std::cerr << "DOMException in xmlBase::Dom::addAttribute(char*)" 
                << std::endl;
      std::cerr << "Msg: " << std::string(msg) << "Code: " 
                << ex.code << std::endl;
      std::cerr.flush();
      XMLString::release(&msg);
      XMLString::release(&xmlchName);
      XMLString::release(&xmlchValue);
      throw ex;
    }

    XMLString::release(&xmlchName);
    XMLString::release(&xmlchValue);

  }

  void  Dom::addAttribute(DOMElement* elt, std::string name, 
                                 std::string value) {

    if (elt == 0)
      throw NullNode("from xmlBase::Dom::addAttribute. Null argument");

    addAttribute(elt, name, value.c_str());
  }

  /* 
      Serialize a node (and any children) to the specified ostream.
      prefix is for now ignored, but see note following.
 
      The only node types which actually do get written are
           element (and its attributes and children)
           text
           comment

      NB:  For this first pass, printElement outputs all the
           supported node types just as they appeared in the
           serialization before parsing *if* the parse was
           non-validating.  If it *was* validating (or if
           the DOM representation was built programmatically
           rather than by parsing a file) then ignorable 
           white space will have been thrown away (in the
           validated case) or there won't have been any to
           begin with (programmatically-built case)
           so the printed version will be a horrific single
           line except that line breaks appearing within
           comments or text nodes will be preserved.

           Ideally would like to be able to query the DOM
           or have an argument passed in to tell us whether
           ignorable white space has been thrown away, in which
           case we should attempt to pretty print by putting
           newlines in in reasonable places and keeping track
           of a sensible indentation level.

           For now, make two different functions.  See 
           prettyPrintElement below.
  */
  void Dom::printElement(DOMNode* node, std::ostream& out) {

    if (node == 0) {
      throw NullNode("from xmlBase::Dom::printElement. Null argument");
    }

    switch(node->getNodeType()) {
    case DOMNode::ELEMENT_NODE:
      {
        // output start tag
        {
          std::string tagName = getNodeName(node);
          out << '<' << tagName;
        }
        // ...with attributes  

        typedef std::map<std::string, DOMNode*> AttMap;
        AttMap atts;
        AttMap::const_iterator it;
        getAttributeNodeMap(node, atts);

        for (it = atts.begin();
             it != atts.end(); it++) {
          std::string attName = (*it).first;
          out << ' ' << attName << '=';
          out << '"' << getAttribute(node,attName) << '"';
        }

        // iterate through children
        DOMNode* child = node->getFirstChild();
        if (child != 0) {  // there are children
          out << '>';
          while (child != 0) {
            Dom::printElement(child, out);
            child = child->getNextSibling();
          }
          // output end tag, long form
          {
            std::string endName = getNodeName(node);
            out << "</" << endName << ">";
          }
        }
        else {  // no children; use short form for the empty tag
          out << " />";
        } 
      }
      break;
    case DOMNode::TEXT_NODE:
      // just put it out as is
      {
        out << getText(node);
      }
      break;

    case DOMNode::CDATA_SECTION_NODE:
      {
        out << "<![CDATA[" << getText(node) << "]]>";
      }
      break;
      

    case DOMNode::COMMENT_NODE :
      // glast.prs doesn't have any comments (but should!)
      {
        out << "<!-- " << getText(node) << "-->";
      }
      break;
    default:
      // ignore anything else
      break;
    }
  }

  // Assume we need to do the indenting and line breaks
  void Dom::prettyPrintElement(DOMNode* node, std::ostream& out,
                               std::string prefix) {
    

    if (node == 0) {
      throw NullNode("from xmlBase::Dom::prettyPrintElement. Null argument");
    }

    out << prefix;
    switch(node->getNodeType()) {

    case DOMNode::ELEMENT_NODE:
      {
        // output start tag
        std::string tagName = getNodeName(node);
        out << '<' << tagName;

        // ...with attributes
        typedef std::map<std::string, DOMNode*> AttMap;
        AttMap atts;
        AttMap::const_iterator it;

        getAttributeNodeMap(node, atts);

        for (it = atts.begin();
             it != atts.end(); it++) {
          std::string attName = (*it).first;
          out << ' ' << attName << '=';
          out << '"' << getAttribute(node,attName) << '"';
        }

        // iterate through children
        DOMNode* child = node->getFirstChild();
        if (child != 0) {  // there are children
          out << '>' << std::endl;
          while (child != 0) {
            // new indent level
            Dom::prettyPrintElement(child, out, prefix + "  ");
            child = child->getNextSibling();
          }
          // output end tag, long form
          {
            std::string endName = getNodeName(node);
            out << prefix << "</" << endName << ">" << std::endl;
          }
        }
        else {  // no children; use short form for the empty tag
          out << " />" << std::endl;
        } 
      }
      break;
    case DOMNode::TEXT_NODE:
      // just put it out as is
      // Note this won't indent correctly if the text node
      // contains multiple lines.
      // Similarly, it's too much work to avoid sometimes putting out
      // an "extra" blank line in the vicinity of text.
      // Current code puts the extra <cr> before
      // the text node.
      {
        out << getText(node);
      }

      break;

    case DOMNode::CDATA_SECTION_NODE:
      {
        // Probably need to put opening and closing sequences in by hand..
        out << "<![CDATA[" << getText(node) << "]]>";
      }
      break;
      
    case DOMNode::COMMENT_NODE :
      // glast.prs doesn't have any comments (but should!)
      // Note this won't indent correctly if the text node
      // contains multiple lines.  Could have utility routine
      // to do this, to be called for comments and text nodes
      {
        out << "<!-- " << getText(node) << "-->" << std::endl;
      }

      break;
    default:
      // ignore anything else
      break;
    }
  }

  void Dom::prune(DOMElement* elt) {
    DOMElement* child = findFirstChildByName(elt, "*");
    while (child != 0) {
      DOMElement* sib = getSiblingElement(child);
      prune(child);
      elt->removeChild(child);
      child = sib;
    }
  }
  char *Dom::transToChar(const XMLCh* const str) {
    return transToChar(str, XMLString::stringLen(str) );
  }

  char *Dom::transToChar(const XMLCh* const str, unsigned int len) {
    // Passing null argument is a fatal error.
    // No, it's not
    //    assert(str != 0);
    if (str == 0) return 0;

    if (!transcoder) {
      int status = initTrans();
      if (!status) return 0;
    }

    // Find length of str to pass to transcode(..) rather than
    // just passing output buffer size.  This is important because 
    // (for Xerces 1.3 anyway) the transcode routine will try to read 
    // this many bytes from the input buffer, conceivably causing
    // an access violation if it's more than the actual input
    // buffer length
    
    if (len + 1 > transBufSize) { // return old buffer; allocate a bigger one
      char * tBuf = new char[len + 1];
      if (!tBuf) return 0;
      transBufSize = len + 1;
      delete [] transBuf;
      transBuf = tBuf;
    }

    bool ok;
    ok = Dom::transcoder->transcode(str, transBuf, len);
    return ( ok ? transBuf : 0);
  }


  XMLCh* Dom::transToXMLCh(const char* const src) {
    // Passing null string is a fatal error
    // No, it's not
    if (src == 0) return 0;
    if (!transcoder) {
      int status = initTrans();
      if (!status) return 0;
    }
    // as with transToChar above, find actual length of char*
    // and pass that to Xerces utility for 3rd (maxChars) argument.
    unsigned int len = strlen(src) + 1;
    if (len  > xmlchBufSize) {
      XMLCh * tBuf = new XMLCh[len];
      if (!tBuf) return 0;
      xmlchBufSize = len;
      delete [] xmlchBuf;
      xmlchBuf = tBuf;
    }

    bool ok;

    ok = transcoder->transcode(src, xmlchBuf, len);
    return (ok ? xmlchBuf : 0);
  }

  int Dom::initTrans() {
    // Need to create a dummy MemoryManager to pass to
    // makeNew LCPTranscoder function in order to please
    // Xerces3
    MemoryManager* dummyManager;
    Dom::transcoder = 
      XMLPlatformUtils::fgTransService->makeNewLCPTranscoder(dummyManager);
    if (!transcoder) return 0;   // and complain?!? Shouldn't ever happen
    transBuf = new char[transBufSize];
    if (!transBuf) {
      delete transcoder;
      delete dummyManager;
      return 0;
    }
    xmlchBuf = new XMLCh[xmlchBufSize];
    if (!xmlchBuf) {
      delete [] transBuf;
      delete transcoder;
      delete dummyManager;
      return 0;
    }
    return 1;
  }

  void Dom::stripComments(DOMNode* elt) {
    if (elt->getNodeType() == DOMNode::DOCUMENT_NODE) {
      // Might have comment children 
      DOMNode* child = elt->getFirstChild();
      while (child) {
        DOMNode* toProcess = child;
        child = child->getNextSibling();
        if (toProcess->getNodeType() == DOMNode::COMMENT_NODE) {
          elt->removeChild(toProcess);
        }
      }

      DOMDocument* doc = static_cast<DOMDocument*> (elt);
      elt = doc->getDocumentElement();
    }
    return stripCommentsInternal(elt);
  }

  void Dom::stripCommentsInternal(DOMNode* node) {
    /*
      if node has no descendants, return
      for each child node   
         if it's a comment, remove it
         if it's an element, recurse
    */
    DOMNode* child = node->getFirstChild();
    while (child) {
      DOMNode* toProcess = child;
      child = child->getNextSibling();
      if (toProcess->getNodeType() == DOMNode::COMMENT_NODE) {
        node->removeChild(toProcess);
      }
      else if (toProcess->getNodeType() == DOMNode::ELEMENT_NODE) {
        stripCommentsInternal(toProcess);
      }
    }
    return;
  }

  bool Dom::writeIt(DOMNode* doc, const char* fname, bool standalone) {
    XMLCh* tempStr = XMLString::transcode("LS");
    DOMImplementation* impl = 
      DOMImplementationRegistry::getDOMImplementation(tempStr);
    XMLString::release(&tempStr);
    //For Xerces3, implement serializer pointer as separate object 
    // from configuration pointer
    DOMLSSerializer *theSerializer = 
      ((DOMImplementationLS*)impl)->createLSSerializer();
    DOMConfiguration *tSConfig = theSerializer->getDomConfig();

    tempStr = XMLString::transcode("format-pretty-print");
    tSConfig->setParameter(tempStr, true);
    XMLString::release(&tempStr);

    XMLFormatTarget *myFormTarget;

    // Translate environment variables, if any
    std::string  f(fname);
    
    facilities::Util::expandEnvVar(&f);
    myFormTarget = new LocalFileFormatTarget(f.c_str());

    if (doc->getNodeType() == DOMNode::DOCUMENT_NODE) {
      DOMDocument* d = static_cast<DOMDocument *>(doc);
      d->setXmlStandalone(standalone);
    }

    // Apparently status = true is good
    //bool status = theSerializer->writeNode(myFormTarget, *doc);
    // Convert the XMLFormatTarget class to DOMLSOutput to act as
    // an output destination for the date using the setByteStream
    // function
    DOMLSOutput* myOutput = ((DOMImplementationLS*)impl)->createLSOutput();
    myOutput->setByteStream(myFormTarget);
    bool status = theSerializer->write(doc, myOutput);
    delete theSerializer;
    delete myFormTarget;
    myOutput->release();
    return status; 
  }

  /* Routines from configData */
    DOMElement* Dom::makeDocument(const char* name) {
      if (!doInit()) return 0;

      XMLCh* xmlchName = XMLString::transcode(name);
      DOMElement* newChild = 
        DOMImplementation::getImplementation()->createDocument()->createElement(xmlchName);
      XMLString::release(&xmlchName);
      newChild->getOwnerDocument()->appendChild(newChild);
      return newChild;
    }


    DOMElement* Dom::makeChildNode(DOMElement* domNode, const char* name) {
      XMLCh* xmlchName = XMLString::transcode(name);
      DOMElement* newChild = 
        domNode->getOwnerDocument()->createElement(xmlchName);
      domNode->appendChild(newChild);
      XMLString::release(&xmlchName);
      return newChild;
    }

    DOMElement* Dom::makeChildNodeWithContent(DOMElement* domNode, 
                                              const char* name,
                                              const char* content) {
      DOMElement* cNode = makeChildNode(domNode,name);
      if ( cNode == 0 ) return 0;
      XMLCh* xmlchContent = XMLString::transcode(content);
      cNode->setTextContent(xmlchContent);
      XMLString::release(&xmlchContent);
      return cNode;
    }

    DOMElement* Dom::makeChildNodeWithContent(DOMElement* domNode, 
                                              const char* name,
                                              unsigned int content) {
      char tmp[20];
      sprintf(tmp,"%d",content);	
      return makeChildNodeWithContent(domNode,name,tmp);
    }
    

    DOMElement* Dom::makeChildNodeWithContent(DOMElement* domNode, 
                                              const char* name,
                                              int content) {
      char tmp[20];
      sprintf(tmp,"%d",content);	
      return makeChildNodeWithContent(domNode,name,tmp);
    }
    
    DOMElement* Dom::makeChildNodeWithHexContent(DOMElement* domNode, 
                                                 const char* name,
                                                 unsigned int content) {
      char tmp[20];
      sprintf(tmp,"0x%0x", content);	
      return makeChildNodeWithContent(domNode,name,tmp);
    }

    DOMComment* Dom::makeComment(DOMElement* domNode, const char* comment) {
      XMLCh* xmlComName = XMLString::transcode(comment);
      DOMComment* theComment = 
        domNode->getOwnerDocument()->createComment(xmlComName);
      domNode->appendChild(theComment);
      XMLString::release(&xmlComName);
      return theComment; 
    }


}  // end namespace xmlBase
