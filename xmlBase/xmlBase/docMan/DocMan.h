// $Header$
#ifndef xmlBase_DocMan_h
#define xmlBase_DocMan_h

#include <string>
#include <vector>
#include "xmlBase/XmlParser.h"

typedef XERCES_CPP_NAMESPACE_QUALIFIER DOMElement DomElement;

namespace xmlBase {

  //!  DocClient is a pure virtual class which client should inherit
  //!  from (or it should own some object which does); it will provide
  //!  the handler.
  class DocClient;

  //! DocMan allows different clients to share a single xml document.
  /*! Clients may sign up to handle particular child elements (direct
      children only of the root element).  Then when the DocMan object
      (there is only one: it's a singleton) is asked to parse a file
      it 
        - invokes Xerces to build the DOM
        - calls back each registered client for first-generation 
          children of the root element
        - resets the parser, which releases the memory for the 
          DOM and readies the parser to parse another document
      DocMan can also be used as a base class for document managers
      which know something about a particular docType.
  */
  class DocMan {
  public: 
    //! Implements singleton.
    static DocMan* getPointer(); 

    //! \b parse not only parses and creates DOM; it also calls back
    //! registered clients.
    /*! Return \b false if something goes wrong with the parse */
    virtual bool parse(const std::string& filename, 
                       const std::string& docType=std::string(""));

    virtual bool regClient(const std::string& eltName, DocClient* client);

  protected:
    DocMan();
    virtual ~DocMan();

    //! Register privileged client; only available to derived classes
    bool regMeFirst(DocClient* client);

    //! Nested class to keep track of clients for one element type
    class ClientList {
    public:
      ClientList(const std::string& eltName) : m_eltName(eltName) {}

      ~ClientList();
      //! Add a client to list.  
      /*! return indicates success or failure (e.g. duplicate).  By
          default new client is added to back of list, but if \a front
          is set to \b true client will go to the front. */
      bool add(DocClient* client, bool front=false);

      //! Remove a client; return indicates success or failure (e.g.,
      //! client not found)
      bool remove(DocClient* client);

      //! which list are we?
      const std::string& getName() {return m_eltName;}

      //! call back each client in turn
      void invoke(DomElement* elt);
    private:
      std::string m_eltName;
      std::vector<DocClient*> m_clients;
      typedef std::vector<DocClient*>::const_iterator ClientsIt;
    };       // end nested class ClientList

    ClientList* findList(const std::string& eltName);
  private:
    static DocMan* m_self;
    std::vector<ClientList*> m_lists;
    typedef std::vector<ClientList*>::const_iterator ListsIt;
    XmlParser* m_parser;
    DocClient* m_meFirst;
  };    //end DocMan class
}
#endif
