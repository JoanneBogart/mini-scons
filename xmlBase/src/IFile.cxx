// $Header$


#include "xmlBase/IFile.h"
#include "xmlBase/XmlParser.h"
#include "xmlBase/Dom.h"
#include "facilities/Util.h"                // for expandEnvVar
#include <xercesc/dom/DOMDocument.hpp>

#include <sstream>
#include <vector>
#include <cstdio>
#include <string>
#include <cctype>

#define FATAL_MACRO(output) std::cerr << output;throw(IFileException(output))

// globals

namespace xmlBase {

XERCES_CPP_NAMESPACE_USE

#define LEADING  1
#define ALL      2
#define TRAILING 4


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void IFile::printOn (std::ostream& out) {
    for (iterator section_map=begin(); section_map!=end(); ++section_map) {
      IFile_Section& section = *(*section_map).second;
      out << "\n[" <<  (*section_map).first << "]\n";
      
      for( IFile_Section::iterator item_map = section.begin();
           item_map != section.end(); ++item_map){
        IFile_Item& item = *(*item_map).second;
        out << (*item_map).first << " = "  << (item.mystring()) << "\n";
      }
    }
  }
  
  void IFile::print(){printOn(std::cout); std::cout.flush();}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int IFile::stricmp (const char *str1, const char *str2)
    {
      while ( *str1  &&  *str2  &&  toupper(*str1)==toupper(*str2) )
      {
        str1++;
        str2++;
      }
      
      return (toupper(*str1) - toupper(*str2));
    }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void IFile::stripBlanks (char *str1, const char *str2, int flags)
    {
      if (flags & ALL)
      {
        while ( *str2 )
        {
          if (*str2==' '  ||  *str2=='\t')
            str2++;
          else
            *str1++ = *str2++;
        }
        *str1=0;
      }
      else
      {
        if (flags & LEADING)
          while ( *str2  &&  (*str2==' '  ||  *str2=='\t'))
            str2++;
        
        strcpy (str1, str2);
        
        if (flags & TRAILING)
        {
          str1 += strlen (str1);
          
          do str1--; while (*str1==' '  ||  *str1=='\t');
          *++str1 = 0;
        }
      }
    }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile::~IFile ()
    {
      iterator it = begin();
      while (it != end())
        delete (*it++).second;
      
    }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile_Section::~IFile_Section()
    {
      iterator it = begin();
      while (it != end())
        delete (*it++).second;
    }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile::IFile (const DOMDocument* doc)  
    {
      // check that argument is non-null
      if (doc == 0) {
        //  FATAL_MACRO("Attempt to construct IFile from null DOMDocument");
        std::cerr << "Attempt to construct IFile from null DOMDocument" 
                  << std::endl;
        std::cerr.flush();
        exit(1);
      }
      // If so, call service to do the actual work
      domToIni(doc);
    }
  
  IFile::IFile (const DOMElement* doc)  
    {
      // check that argument is non-null
      if (doc == 0) {
        //        FATAL_MACRO("Attempt to construct IFile from null DOMElement");
        std::cerr << "Attempt to construct IFile from null DOMDocument" 
                  << std::endl;
        std::cerr.flush();
        exit(1);
      }
      // If so, call service to do the actual work
      domToIni(doc);
    }
  

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile::IFile (const char *filename) 
    {   
      using facilities::Util;

      XmlParser parser;
      
      parser.doSchema(true);

      std::string filenameStr = filename;
      Util::expandEnvVar(&filenameStr);

      // What if this fails (e.g., file doesn't exist or is not 
      // well-formed)?? How to report it?
      DOMDocument* doc = parser.parse(filenameStr.c_str());
      
      // Check it's a good doc.  
      if (doc == 0) {
        std::cerr << "Attempt to construct IFile from null DOMDocument" 
                  << std::endl;
        std::cerr.flush();
        exit(1);
        //   FATAL_MACRO("Attempt to construct IFile from null DomDocument");
      }
      
      // If so, initialize IFile from it
      domToIni(doc);
    }

  // Work of constructor minus parsing
  void IFile::domToIni(const DOMDocument* doc) {
    DOMElement*  root = doc->getDocumentElement();
    
    // Now invoke element version to do the work
    domToIni(root);
  }        

  void IFile::domToIni(const DOMElement* root) {
    // Done this way, any child elements which are *not* sections
    // will simply be ignored.  Another strategy would be to look 
    // at all children and complain if any are not sections
    std::vector<DOMElement*> sections;
    Dom::getChildrenByTagName(root, "section", sections);
    unsigned int nChild = sections.size();

    for (unsigned int iChild = 0; iChild < nChild; iChild++) {
      addSection(sections[iChild]);
    }
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void IFile::addSection(const DOMElement* section)  {
    std::string tagName = Dom::getTagName(section);

    if (tagName.compare("section") ) {
      std::string errorString = 
        "Expecting tagName==section, found " +  tagName;
      FATAL_MACRO(errorString);
    }
    
    // start a section
    std::string sectName = Dom::getAttribute(section, "name");
    IFile_Section* curSection = new IFile_Section(sectName);
    (*this)[curSection->title()]=curSection;
    
    std::vector<DOMElement*> children;

    Dom::getChildrenByTagName(section, "*", children);

    unsigned int nChild = children.size();
    for (unsigned int iChild = 0; iChild < nChild; iChild++) {
      DOMElement* child = children[iChild];
      std::string tagName = Dom::getTagName(child);
      if (!(tagName.compare("section")) ) {
        addSection(child);
      }
      else if (!(tagName.compare("item")) ) {
        std::string  itemName = Dom::getAttribute(child, "name");
        std::string itemValue = Dom::getAttribute(child, "value");
        
        // Make the new item
        IFile_Item* newItem = 
          new IFile_Item(itemName, itemValue);
        // Add it to the section map
        (*curSection)[newItem->title()]= newItem;
      }
      else {
        std::string errorString = "unexpected tag in initialization:" 
          + tagName;
        FATAL_MACRO(errorString);
      }      // end if..else
    }     // end for
    
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool IFile::contains (const char *section, const char *item)
    {
      return (IFile::_getstring (section, item, 0) != 0);
    }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  const char *IFile::_getstring(const char *sectionname, const char *itemname, 
                                int failFlag)
    {
      char     hitem[1000], hsection[1000];
      IFile_Item    *item = 0;
      IFile_Section *section =0;
      
      stripBlanks (hitem,    itemname,    ALL);
      stripBlanks (hsection, sectionname, ALL);
      
      const_iterator entry = find(std::string(hsection));
      
      if (entry  != end() )      {
        section = (*entry).second;
        
        IFile_Section::const_iterator it = section->find(std::string(hitem));
        item = (it != section->end() ) ?item = (*it).second :  0;
        
      }
      
      if (item != 0)   {
#ifdef DEBUG
        INFO ("getstring: [" << hsection << "]" << hitem << ": ->" << 
              (item->string()) << "<-");
#endif
        return item->mystring().c_str();
      }
      else if (failFlag)
      {
        if (section == 0) {
          std::string errorString =
            std::string("cannot find section [") + sectionname + "]";
          FATAL_MACRO (errorString);
        }
        else {
          std::string errorString =
            std::string("cannot find item '") + itemname +  "' in section [" 
            + sectionname + "]";
          FATAL_MACRO (errorString);
        }
        return 0;
      }
      
      else   return 0;
    }
  
  // getting data from [section]item, exiting when not found
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  const char *IFile::getString (const char *section, const char *item)
    {
      return _getstring(section, item);
    }
  
  // setting data in [section]
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void IFile::setString(const char *sectionname, const char *itemname, 
                        const char* newString)
    {
      char     hitem[1000], hsection[1000];
      IFile_Item    *item =0;
      IFile_Section *section=0;
      
      stripBlanks (hitem,    itemname,    ALL);
      stripBlanks (hsection, sectionname, ALL);
      
      iterator it = find(std::string(hsection));
      
      if ( it != end() )      {
        section = (*it).second;
        
        if (section->contains(hitem) )
          item = section->lookUp(hitem);
      }
      
      if (item)	item->mystring()=newString;
    }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double IFile::getDouble (const char *section, const char *item)
    {
      //      double    hf;
      std::string hilf (IFile::_getstring (section, item));

      try  {
        return facilities::Util::stringToDouble(hilf);
      }
      catch (facilities::WrongType ex) {
        std::cerr << ("from xmlBase::IFile::getDouble  ") << std::endl;
          //        FATAL_MACRO
        std::cerr << ex.getMsg() << std::endl;
        throw(IFileException(" "));
      }
    }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int IFile::getInt (const char *section, const char *item)
    {
      //      int       hf;
      std::string hilf (IFile::_getstring (section, item));

      try  {
        return facilities::Util::stringToInt(hilf);
      }
      catch (facilities::WrongType ex) {
        std::cerr << ("from xmlBase::IFile::getInt  ") << std::endl;
        std::cerr << ex.getMsg() << std::endl;
        throw(IFileException(" "));
      }
    }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int IFile::getBool (const char *section, const char *item)
    {
      std::string hilf (IFile::_getstring (section, item));
      
      if      (hilf == "yes")
        return (1);
      else if (hilf == "true")
        return (1);
      else if (hilf == "1")
        return (1);
      else if (hilf == "no")
        return (0);
      else if (hilf == "false")
        return (0);
      else if (hilf == "0")
        return (0);
      else {
        std::string errorString("[");
        errorString += section + std::string("]") + item + " = \'" + hilf 
          + "\' is not boolean";
        FATAL_MACRO (errorString);
        return (0);
      }
    }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile::intVector IFile::getIntVector (const char *section, const char *item)
    {
      intVector iv;
      char buffer[1024];
      
      strncpy(buffer, IFile::_getstring(section,item), sizeof(buffer) -1);
      if ( strlen(buffer) >= sizeof(buffer) ) {
        FATAL_MACRO("string returned from _getstring is too long");
        return iv;
      }
      
      char *vString = strtok(buffer,"}");
      vString = strtok(buffer,"{");
      
      char *test = strtok(vString,",");
      while (test != NULL) {
        iv.push_back(atoi(test));
        test = strtok((char*)NULL,",");
      }
      if (iv.size() <= 0) {
        std::string hilf (buffer);
        std::string errorString("[");
        errorString += section + std::string("]")  + item + " = \'"
          + hilf + "\' is not an integer vector";
        FATAL_MACRO (errorString);

      }
      return (iv);
    }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile::doubleVector IFile::getDoubleVector (const char *section, 
                                              const char *item)
    {
      doubleVector dv;
      char buffer[1024];
      
      strncpy(buffer, IFile::_getstring(section,item), sizeof(buffer) );
      if (strlen(buffer) >= sizeof(buffer) ) {
        FATAL_MACRO("string from _getstring() too long");
        return dv;
      }
      char *vString = strtok(buffer,"}");
      vString = strtok(buffer,"{");
      
      char *test = strtok(vString,",");
      while (test != NULL) {
        dv.push_back(atof(test));
        test = strtok((char*)NULL,",");
      }
      if (dv.size() <= 0) {
        std::string hilf (buffer);
        std::string errorString("[");
        errorString += section + std::string("]")  + item + " = \'"
                               + hilf + "\' is not an double vector";
        FATAL_MACRO (errorString);
      }
      return (dv);
    }
  
  // getting data from [section]item, with def. values provided
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int IFile::getInt (const char *section, const char *item, int defValue) {
    return ( contains(section, item) )? getInt( section, item ):defValue;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int IFile::getBool (const char *section, const char *item, int defValue)
    {
      return ( contains(section, item) )? getBool( section, item ):defValue;
    }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double IFile::getDouble(const char *section, const char *item, 
                          double defValue)   {
    return ( contains(section, item) )? getDouble( section, item ):defValue;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  const char *IFile::getString (const char *section, const char *item, 
                                const char *defValue) {
    return ( contains(section, item) )? getString( section, item ):defValue;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile::intVector IFile::getIntVector (const char *section, const char *item, 
                                        intVector defValues)    {
    return (contains(section, item))? getIntVector(section, item) 
      : defValues;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  IFile::doubleVector IFile::getDoubleVector (const char *section, 
                                              const char *item, 
                                              doubleVector defValues)  {
    return (contains(section, item))? getDoubleVector(section, item):defValues;
  }
} //   end namespace xmlBase

