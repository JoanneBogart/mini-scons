/// Test program for IFile facility within xmlBase package. 

#include "xmlBase/IFile.h"

#include "facilities/commonUtilities.h"

#include <string>
#include <iostream>


void lookFor(xmlBase::IFile* ifile, const char* section, const char* item);

int main() {
  facilities::commonUtilities::setupEnvironment();
  std::string filename = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "myIFile.xml");
  xmlBase::IFile* ifile = 0;

  ifile = new xmlBase::IFile(filename.c_str());
  // fetch some stuff  

  if (ifile) {

    lookFor(ifile, "section1", "section1-val1");
    lookFor(ifile, "section1", "section1-val2");
    lookFor(ifile, "subsection","subsectionItem");
    lookFor(ifile, "section2","subsectionItem");
    lookFor(ifile, "section2","bad-int");
  }
  else std::cout << "Unable to read file " << ifile << std::endl;


  // Now do it again for IFile referencing schema rather than dtd
  std::string filename2 = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "mySchemaIFile.xml");

  std::cout << std::endl << std::endl;
  std::cout << "And for my next trick:  process an IFile using XML Schema.";
  std::cout << std::endl;
  std::cout << 
    "This file should be flagged bad; it doesn't follow schema content model." 
            << std::endl << std::endl;

  xmlBase::IFile* ifile2 = 0;

  ifile2 = new xmlBase::IFile(filename2.c_str());
  // fetch some stuff  

  if (ifile2) {

    lookFor(ifile2, "section1", "section1-val1");
    lookFor(ifile2, "section1", "section1-val2");
    lookFor(ifile2, "subsection","subsectionItem");
    lookFor(ifile2, "section2","subsectionItem");
    lookFor(ifile2, "section2","bad-int");
  }
  else std::cout << "Unable to read file " << ifile2 << std::endl;
  
  return(0);
}

void lookFor(xmlBase::IFile* ifile, const char* section, const char* item) { 
  bool haveItem = ifile->contains(section, item);
  std::cout << "Item   " << item << "   in section   " << section;

  if (haveItem) {
    std::cout << " was found." << std::endl;
    // try writing  in various formats
    const char* charRep = ifile->getString(section, item);
    std::cout << "..as a string = " << charRep << std::endl;

    try {
      int intRep = ifile->getInt(section, item);
      std::cout << "..as an int = " << intRep << std::endl;
    }
    catch (xmlBase::IFileException toCatch) {
      std::cout << "**ERROR** int conversion failed "
                << std::endl << std::endl;
    }

    try {
      double doubleRep = ifile->getDouble(section, item);
      std::cout << "..as a double = " << doubleRep << std::endl;
    }
    catch (xmlBase::IFileException toCatch) {
      std::cout << " **ERROR** double conversion failed " 
                << std::endl << std::endl;
    }
    
  }
  else std::cout << " was NOT found." << std::endl;
}
