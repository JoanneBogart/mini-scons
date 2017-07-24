// $Header$
// Author:  J. Bogart

#ifndef xmlBase_Dom_h
#define xmlBase_Dom_h
#include <xercesc/util/XercesDefs.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class  DOMElement;
class  DOMNode;
class  XMLLCPTranscoder;
// class XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList;
class  DOMDocument;
class  DOMComment;
XERCES_CPP_NAMESPACE_END
#include <string>
#include <iostream>
#include <vector>
#include <map>

namespace xmlBase {
  //! Base exception class for Dom
  class DomException : std::exception {
  public:
    DomException(const std::string& extraInfo = "") : std::exception(),
      m_name("DomException"), m_extra(extraInfo) {}
    virtual ~DomException() throw() {}
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

  class NullNode : public DomException {
  public:
    NullNode(const std::string& extraInfo = "") : DomException(extraInfo) 
    {m_name = "NullNode";}
    virtual ~NullNode() throw() {}
  };

  class WrongAttributeType : public DomException {
  public:
    WrongAttributeType(const std::string& extraInfo = "") : 
      DomException(extraInfo) 
    {
      m_name = "WrongAttributeType";
    }
    virtual ~WrongAttributeType() throw() {}
  };

  class WrongNodeType : public DomException {
  public:
    WrongNodeType(const std::string& extraInfo = "") : 
      DomException(extraInfo) 
    {
      m_name = "WrongNodeType";
    }
    virtual ~WrongNodeType() throw() {}
  };

  //  XERCES_CPP_NAMESPACE_USE
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMNode;
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;  
  using XERCES_CPP_NAMESPACE_QUALIFIER DOMComment;
  //! This class is just a convenient place for some static functions
  //! and a static buffer to be used as temp space for transcoding
  //! which beef up the DOM interface a bit
  class Dom {

  public: 
    /// ensure xerces is initialized
    static bool didInit();
    static bool doInit();
    /*! Find first child with given tagname.  Only immediate children
        qualify.
        \param parent    Element whose child is to be found
        \param name      name to look for (may be *, in which case
                         first child, whatever its tagname, will
                         be returned)
    */
    static DOMElement*  findFirstChildByName(const DOMElement* parent, 
                                             const char* const name);
    static DOMElement*  findFirstChildByName(const DOMElement* parent, 
                                             const std::string name);
    
    //! Return next element sibling, if any.  
    /*! DOM api doesn't provide a direct way to get next sibling which 
        is also an element.
    */
    static DOMElement* getSiblingElement(const DOMNode* child);

    //! Get first child which is an element node, if any.
    static DOMElement*  getFirstChildElement(const DOMNode* parent);

    static DOMElement* getElementById(const DOMDocument* doc,
                                     const std::string& id);

    //! Create std::string holding node name (if that makes sense
    //! for this node).  Throws NullNode exception if argument is null
    static std::string  getNodeName(const DOMNode* elt); 

    //! Create std::string holding tag name (= node name for
    //! the base DomNode). Throws NullNode exception if argument is null.
    static std::string  getTagName(const DOMElement* node);


    //! See if element has expected tag name or not. 
    //! Throws NullNode exception if argument is null
    static bool checkTagName(const DOMElement* element, 
                             const std::string& tagName);
   
    //! Fill supplied vector with all (immediate) child elements matching 
    //! requested tagname.  If tagName == "*" returns all immediate
    //! children.  By default clear vector first.
    static void getChildrenByTagName(const DOMElement* parent, 
                                     const std::string& tagName,
                                     std::vector<DOMElement*>& children,
                                     bool clear = true);

    //! Fill supplied vector with all descendant elements matching 
    //! requested tagname.  if tagNmae = "*" returns all descendant
    //! elements. By default clear vector first.
    static void getDescendantsByTagName(const DOMElement* parent, 
                                        const std::string& tagName,
                                        std::vector<DOMElement*>& children,
                                        bool clear = true);

    //! Fill supplied map with all attribute nodes, where key is
    //! attribute name.  By default clear map first.
    static void getAttributeNodeMap(const DOMNode* elt,
                                    std::map<std::string, DOMNode*>& atts,
                                    bool clear = true);

    //! Return true if arg is valid element and has specified
    //! attribute, else false
    static bool hasAttribute(const DOMNode* elt, const char* attName);

    //! Return std::string form of attribute value (standard DOM
    //! interface deals only in DOMString type).  Return null string
    //! if attribute isn't found.
    static std::string  getAttribute(const DOMElement* elt, 
                                     const char* attName);
    //! Return std::string form of attribute value (standard DOM
    //! interface deals only in DOMString type). Return null string
    //! if attribute isn't found.
    static std::string  getAttribute(const DOMElement* elt, 
                                     std::string attName);

    //! Return std::string form of attribute value (standard DOM
    //! interface deals only in DOMString type) or null if \b elt
    //! is not a DOM_Element or attribute is not found.
    static std::string  getAttribute(const DOMNode* elt, 
                                     const char* attName);
    //! Return std::string form of attribute value (standard DOM
    //! interface deals only in DOMString type) or null if \b elt
    //! is not a DOM_Element or attribute isn't found
    static std::string  getAttribute(const DOMNode* elt, 
                                     std::string attName);

    //! Return double attribute value.  Throw exception if 1) not element 
    //! node 2) attribute doesn't exist 3) value isn't really a double
    static double getDoubleAttribute(const DOMNode* elt, std::string attName);

    //! Return  doubles in user-supplied vector.  Attribute value
    //! should be a string consisting of valid ascii representation of
    //! floating point numbers, separated by blanks; else the method
    //! will throw an exception (xml::WrongAttributeType)
    //! @returns   Number of good values found
    static unsigned getDoublesAttribute(const DOMNode* elt, 
                                        std::string attName,
                                        std::vector<double>& values, 
                                        bool clear=true);

    //! Return  doubles in user-supplied vector.  Attribute value
    //! should be a string consisting of valid ascii representation of
    //! floating point numbers, separated by blanks; else the method
    //! will throw an exception (xml::WrongAttributeType)
    //! @returns   Number of good values found
    static unsigned getFloatsAttribute(const DOMNode* elt, 
                                       std::string attName,
                                       std::vector<float>& values, 
                                       bool clear=true);

    //! Return int attribute value.  Throw exception if 1) not element 
    //! node 2) attribute doesn't exist 3) value isn't really an int
    static int getIntAttribute(const DOMNode* elt, std::string attName);

    //! Return  ints in user-supplied vector.  Attribute value
    //! should be a string consisting of valid ascii representation of
    //! floating point numbers, separated by blanks; else the method
    //! will throw an exception (xml::WrongAttributeType)
    //! @returns   Number of good values found
    static unsigned getIntsAttribute(const DOMNode* elt, 
                                     std::string attName,
                                     std::vector<int>& values, 
                                     bool clear=true);

    //! Return value of a text node (the text) as a standard string; 
    //! throw an exception if it's not a text-type node (TEXT_NODE,
    //! CDATA_SECTION_NODE, COMMENT_NODE are ok)
    static std::string getText(const DOMNode* textNode);

    //! Return value of text node (the text) for text node which
    //! is child of the supplied element argument. Element should
    //! have only text content, not mixed content.
    static std::string getTextContent(const DOMElement* elt);

    // Following routines all have to do with creating, modifying,
    // or writing out a document



    //! Add attribute of type double to a DOM element, std::string att name
    static void         addAttribute(DOMElement* elt, std::string name, 
                                     double value, unsigned int precision=6);

    //! Add attribute of type \c int to a DOM element 
    static void         addAttribute(DOMElement* elt, std::string name, 
                                     int value);

    //! Add attribute of type \c unsigned \c int to a DOM element 
    static void         addAttribute(DOMElement* elt, std::string name, 
                                     unsigned int value);

    //! Add an attribute of type \c string to a DOM element 
    static void         addAttribute(DOMElement* elt, std::string name, 
                                     std::string value);

    //! Add an attribute of type \c char* to a DOM element 
    static void         addAttribute(DOMElement* elt, std::string name, 
                                     const char * const value);

    //! Create a new DOM document
    static DOMElement* makeDocument(const char* name);

    //! Add child elements of various kinds to existing element
    static DOMElement* makeChildNode(DOMElement* domNode, const char* name);

    static DOMElement* 
    makeChildNodeWithContent(DOMElement* domNode, const char* name,
                             const char* content);

    static DOMElement* makeChildNodeWithContent(DOMElement* domNode, 
                                                const char* name,
                                                unsigned int content);

    static DOMElement* makeChildNodeWithContent(DOMElement* domNode, 
                                                const char* name,
                                                int content);

    static DOMElement* makeChildNodeWithHexContent(DOMElement* domNode, 
                                                   const char* name,
                                                   unsigned int content);

    //! Add a comment after a given node
    static DOMComment* makeComment(DOMElement* domNode, const char* comment);


    //! Strip comments from supplied node and all its descendants
    static void stripComments(DOMNode* elt);

    //! Write document to specified file.  Return of "true" is good
    static bool writeIt(DOMNode* doc, const char* fname, 
                        bool standalone=false);
    // Could get rid of prefix argument for non-pretty print
    //! (Re)serialize and output a DOM node and its children
    //!    to the specified stream.
    /*! To output an entire document, invoke this routine with
        the document element as first argument.

        This routine works best for DOM representations which
        already include formatting (end-of-lines and indentation).
    */
    static void         printElement(DOMNode* elt, std::ostream& out);

    //! (Re)serialize and output a DOM node and its children
    //!    to the specified stream.
    /*! To output an entire document, invoke this routine with
        the document element as first argument.

        This routine works best for DOM representations which
        do not already include formatting (end-of-lines and indentation).
    */
    static void         prettyPrintElement(DOMNode* elt, std::ostream& out,
                                           std::string prefix);

    /** Utility to remove all children of elt (but not elt itself).
     */
    static void         prune(DOMElement* elt);

    // Make EResolver, XmlParser and XmlErrorHandler friends so they can 
    // use transcode methods
    // friend class EResolver;
    //    friend class XmlErrorHandler;
    //    friend class XmlParser;
  private:
    /// Return pointer to transcoded DOMString.  
    /** For temporary use only, such as immediately writing to some 
      output. Applications needing permanence should copy the char array.
      Passing in a null argument is a fatal error */ 

    /// Assume here we know we're not handed a DOMDocument node
    static void stripCommentsInternal(DOMNode* elt);

    /// Return pointer to transcoded XMLCh string.
    static char*        transToChar(const XMLCh* const str);
    static char*        transToChar(const XMLCh* const str, unsigned int len);


    /// Transcode \b from character string \b to XMLCh string.
    /** Passing in a null argument is a fatal error. */
    static XMLCh*       transToXMLCh(const char* const src);


    static unsigned int transBufSize;
    static char*        transBuf; /*< output buffer for \b transToChar */
    static unsigned int xmlchBufSize;
    static XMLCh*       xmlchBuf; /*< output buffer for \b transToXMLCh */
    static XERCES_CPP_NAMESPACE_QUALIFIER XMLLCPTranscoder* transcoder;
    static int          initTrans(); /*< set up buffers, make transcoder */

    static XMLCh*    xmlchStar;
    static int       s_didInit;

  };
} // end namespace
#endif
