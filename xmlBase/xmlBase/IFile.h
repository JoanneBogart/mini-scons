// $Header$
// adapted from Bonn, originally written by Ruediger Gross-Hardt
// hardy@servax.iskp.uni-bonn.de

// converted to use STL map

// User: Suson      Date: 6/29/98
// Added getIntVector, getDoubleVector

#ifndef xmlBase_IFile_h
#define xmlBase_IFile_h

#include <cstdio>
#include <iostream>

#include <string>
#include <map>
#include <vector>

#include <xercesc/util/XercesDefs.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMElement;
class DOMDocument;
XERCES_CPP_NAMESPACE_END

// typedef XERCES_CPP_NAMESPACE_QUALIFIER DOMElement DomElement;
// typedef XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument DomDocument;

namespace xmlBase {
  XERCES_CPP_NAMESPACE_USE
  class IFileException {
  public:
    IFileException(std::string err) : msg(err) {}
    std::string msg;
  };
  class IFile_Item 
    {
      // basic item, should be private to IFile
      friend class IFile_Section;
      friend class IFile;
    public:
      IFile_Item(){};
      //  IFile_Item(const char *line, int lineNumber);
      IFile_Item(std::string name, std::string value):itemname(name), 
        itemstring(value){}
      std::string& title()  { return itemname;    };
      std::string& comment(){ return itemcomment; };
      std::string& mystring() { return itemstring;  };
      
    private:
      std::string itemname;
      std::string itemstring;
      std::string itemcomment;
    };
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  class IFile_Section : 
    public std::map<std::string, IFile_Item*, std::less<std::string> > {
    // section, a map of items
    friend class IFile;
    public:
    IFile_Section(){};
    IFile_Section( std::string name):sectionname(name){}
    ~IFile_Section();
    std::string& title() { return sectionname; };
    private:
    
    bool contains(std::string name){ return find(name)!= end(); }
    
    IFile_Item* lookUp(std::string name){
      iterator it = find(name);
      return (it==end())? 0 : (*it).second;
    }
    std::string sectionname;
  };
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  class IFile : 
    protected std::map<std::string, IFile_Section*, std::less<std::string> > 
    {
      // IFile is a map of sections
      
    public:
      IFile (const char *filename);
      IFile (const DOMDocument* instrument); 
      // or do we want document element rather than DomDocument as arg?
      IFile (const DOMElement* instrument);
      
      virtual ~IFile ();
      
      typedef std::vector<int> intVector;
      typedef std::vector<double> doubleVector;
      
      virtual bool   contains   (const char *section, const char *item);
      // check, whether section and item is contained in IFile
      
      virtual int	    getInt	  (const char *section, const char *item);
      virtual double  getDouble  (const char *section, const char *item);
      virtual int	    getBool	  (const char *section, const char *item);
      virtual const char *getString  (const char *section, const char *item);
      virtual intVector getIntVector  (const char *section, const char *item);
      virtual doubleVector getDoubleVector (const char *section, 
                                            const char *item);
      // get data from [section]item
      
      virtual int	 getInt  (const char *section, const char *item, 
                              int defValue);
      virtual double getDouble (const char *section, const char *item, 
                                double defValue);
      virtual int    getBool (const char *section, const char *item, 
                              int defValue);
      virtual const char *getString  (const char *section, const char *item, 
                                      const char *defValue);
      virtual intVector getIntVector (const char *section, const char *item, 
                                      intVector defValues);
      virtual doubleVector getDoubleVector (const char *section, 
                                            const char *item,
                                            doubleVector defValues);
      // get data from [section]item, providing a default value, if not present
      
      void setString(const char *section, const char *item, 
                     const char* newString);
      
      void print(); // print contents to cout
      //   virtual void	 printOn    (HOStream& out = herr);
      virtual void        printOn	  (std::ostream& out = std::cout );
      // print IFile on stream

      /// check for and replace an environment variable within a string
      /// Soon to become obsolete -- maybe is by now
      //      static void       extractEnvVar(std::string *fileStr);
      
    protected:
      friend class IFile_Section;
      friend class IFile_Item;
      
      static void	stripBlanks  (char *str1, const char *str2, int flags);
      static int	stricmp      (const char *str1, const char *str2);


      // helper functions (some should be private/protected)
      
      IFile() {};
    private:
      // May want to change & to *, to be more in line with the way
      // Xerces has implemented the DOM interface
      void       addSection(const DOMElement* elt);
      void       domToIni(const DOMDocument* doc);
      void       domToIni(const DOMElement* doc);
      virtual const char *_getstring  (const char *section, const char *item,
                                       int failIfNotFoundFlag=1);
      // internal function, that does the work
    };
}  // end namespace xml 
#endif

