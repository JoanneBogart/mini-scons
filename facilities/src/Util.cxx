// $Header$

#include "facilities/Util.h"

#include <cstdlib>
#include <sstream>

#include <iostream>
#include <cstdio>
#include <string.h>

#ifdef WIN32
// For Sleep( ).   Not happy about this, but can't find anything else 
// that works.  All that's needed is WinBase.h, but compile fails 
// when that's all that is included.
#include <Windows.h>
#else
#include <unistd.h>
#endif


/** @file Util.cxx 
@author J. Bogart
*/

namespace facilities {
    
  int Util::expandEnvVar(std::string* toExpand, 
                         const std::string& openDel,
                         const std::string& closeDel) {
    unsigned opLen = openDel.size();
    unsigned clLen = closeDel.size();
    int nSuccess = 0;
        
    int envStart = openDel.empty()? 0 : toExpand->find(openDel.c_str());
    while (envStart != -1) {
      int envEnd = closeDel.empty()?toExpand->size():toExpand->find(closeDel.c_str());
      
      // add  characters to account for opening delimiter
      int afterBracket = envStart + opLen;
            
      if (!( (envStart==-1) || (envEnd==-1) ))
      {
        std::string envVariable = 
          toExpand->substr(afterBracket,(envEnd-afterBracket));
        const char * path = ::getenv(envVariable.c_str());
        if (path) {
          toExpand->replace(envStart,(envEnd+clLen), path);
          if (nSuccess > -1) nSuccess++;
        }
        else {
          throw Untranslatable(envVariable);
        }
      }
      envStart = openDel.empty()?-1:toExpand->find(openDel.c_str());
    }
    return nSuccess;
  }
    
  int Util::expandEnvVarOS(std::string* toExpand) {
#ifdef WIN32 
    return expandEnvVar(toExpand, "%", "%");
#else
    return expandEnvVar(toExpand, "${", "}" );
#endif
  }

  int Util::expandEnvVarList(std::vector<std::string> toExpand,
                             std::vector<std::string> &result,
                             const std::string &delimiters) {
   int num=0;
   try {
       std::vector<std::string>::const_iterator listIt, listIt2;
       // iterate over the elements in the vector of strings
       for (listIt = toExpand.begin(); listIt != toExpand.end(); listIt++) {
           std::string tempStr = *listIt;
           num = expandEnvVar(&tempStr);
           std::vector<std::string> tempList;
           // find all the individual strings
           stringTokenize(tempStr, delimiters, tempList);
           // Save all the strings
           for (listIt2 = tempList.begin(); listIt2 != tempList.end(); listIt2++) 
               result.push_back(*listIt2);
        }
   } catch(std::exception& e) {
       throw(e);
   } catch(...) {
       throw;
   }
   return num;
   }

  const char* Util::itoa(int val, std::string &outStr) {
    // Purpose and Method:  Provide a standard routine to convert integers
    //    into std::string.  The method used depends upon the availability of
    //    the stringstream classes.  The stringstream classes are the
    //    standard, but some compilers do yet support them. 
    //    [Oct. 24, 2005. Compiler upgrades some time ago eliminated this
    //    problem, so no longer need the #ifdef

    static char outCharPtr[20];

    std::ostringstream locStream;
    locStream.str("");

    locStream << val;

    outStr = locStream.str();
    strcpy(outCharPtr, outStr.c_str());
    return outCharPtr;
  }

  const char* Util::utoa(unsigned int val, std::string &outStr, int base) {
    static char outCharPtr[20];

    std::ostringstream locStream;
    locStream.str("");
    if (base == 16) {
      locStream.fill('0');
      locStream.width(8);
      locStream.setf(std::ios::hex);
      locStream.unsetf(std::ios::dec);
    }
    locStream << val;

    outStr = locStream.str();
    strcpy(outCharPtr, outStr.c_str());
    return outCharPtr;
  }
    
  int Util::atoi(const std::string& inStr) {
    // Purpose and Method:  Provide a standard routine to convert std::strings
    //    into integers.  The method used depends upon the availability of
    //    the stringstream classes.  The stringstream classes are the
    //    standard, but some compilers do yet support them.
    //    The method used is determined by the DEFECT_NO_STRINGSTREAM
    //    macro, defined in the facilities requirements file.
    // Output: returns an integer
    //    if string cannot be converted to an integer, returns zero

    int val;

    std::istringstream locStream(inStr);

    locStream >> val;
    if (!locStream) {return 0;}
    return val;
  }  

  double Util::stringToDouble(const std::string& inStr) {
    // Extra test is because OSX Leopard converts 3. to a double (3)
    // and a string (.) while everyone else just gives a double (3.)
    double val;
    char  junk[3];
    int nItem = sscanf(inStr.c_str(), "%lg %1s", &val, junk);
    if ((nItem != 1) && (nItem != 2)) {
      throw WrongType(inStr, "double");
    }
    if (nItem == 2) {
      if (junk[0]!='.'){
	throw WrongType(inStr, "double");
      }
    }
    return val;
  }


  int Util::stringToInt(const std::string& inStr) {
    int  val;
    char junk[3];
    int nItem = sscanf(inStr.c_str(), "%d %1s", &val, junk);
    if (nItem != 1) {
      throw WrongType(inStr, "int");
    }
    return val;
  }

  unsigned int Util::stringToUnsigned(const std::string& inStr) {
    unsigned int  val;
    char junk[3];
    // %u format blithely wraps negative numbers around to positive ones, 
    // so check for that first
    if (inStr.find(std::string("-")) < inStr.size()) {
      throw WrongType(inStr, "unsigned int");
    }
    int nItem;
    if (inStr.find(std::string("0x")) < inStr.size()) {
      nItem = sscanf(inStr.c_str(), "%x %1s", &val, junk);
    }
    else {
      nItem = sscanf(inStr.c_str(), "%u %1s", &val, junk);
    }
    if (nItem != 1) {
      throw WrongType(inStr, "unsigned int");
    }
    return val;
  }

  unsigned long long Util::stringToUll(const std::string& inStr) {
    unsigned long long  val;
    char junk[3];
    // %u format blithely wraps negative numbers around to positive ones, 
    // so check for that first
    if (inStr.find(std::string("-")) < inStr.size()) {
      throw WrongType(inStr, "unsigned long long");
    }
    int nItem;
    if (inStr.find(std::string("0x")) < inStr.size()) {
      nItem = sscanf(inStr.c_str(), "%llx %1s", &val, junk);
    } 
    else {
      nItem = sscanf(inStr.c_str(), "%llu %1s", &val, junk);
    }
    if (nItem != 1) {
      throw WrongType(inStr, "unsigned long long");
    }
    return val;
  }


  void Util::stringTokenize(std::string input, const std::string& delimiters,
                            std::vector<std::string>& tokens, bool clear) {
    if (clear) tokens.clear();

    std::string::size_type j;
    while ( (j = input.find_first_of(delimiters)) != std::string::npos ) {
      if (j != 0) tokens.push_back(input.substr(0, j));
      input = input.substr(j+1);
    }
    tokens.push_back(input);
    if (tokens.back() == "") tokens.pop_back();
  }


  void Util::keyValueTokenize(std::string input, 
                              const std::string& delimiters,
                              std::map<std::string,std::string>& tokens, 
                              const std::string& pairDelimiter,
                              bool clear) {
    if (clear) tokens.clear();

    std::vector<std::string> strvec;
    stringTokenize(input,delimiters,strvec,true);
    unsigned advance = pairDelimiter.size();

    std::vector<std::string>::const_iterator input_itr = strvec.begin();
    while(input_itr!=strvec.end())
      {
	std::string current = *input_itr++;
	std::string::size_type j = current.find(pairDelimiter);
	std::string key   = current.substr(0, j);
	std::string value = current.substr(j+advance);
	tokens[key] = value;
      } 
  }
  
  std::string Util::basename(const std::string& path) {
     std::vector<std::string> names;
     stringTokenize(path, "\\/", names);
     return *(names.end() - 1);
  }


  unsigned Util::trimLeading(std::string* toTrim) {
    std::string::size_type first_char = toTrim->find_first_not_of(" \r\n\t\f");
    if (first_char == std::string::npos)
      return 0;
    *toTrim = toTrim->substr(first_char, std::string::npos);
    return first_char;
  }


  unsigned Util::trimTrailing(std::string* toTrim) {
    static const char blank=' ';
    static const char LF=0xA;    // new line
    static const char FF=0xC;    // form feed
    static const char CR=0xD;    // carriage return

    unsigned orig = toTrim->size();
    unsigned last = orig - 1;
    bool notDone = true;
    unsigned nTrimmed = 0;


    while (notDone) {
      char lastChar = (*toTrim)[last];
      switch (lastChar) {
      case blank:
      case LF:
      case FF:
      case CR:
        last--;
        nTrimmed++;
        break;
      default:
        notDone=false;
        break;
      }
    }
    if (nTrimmed)  toTrim->resize(orig - nTrimmed);

    return nTrimmed;
  }


  unsigned Util::trim(std::string* toTrim) {
    return Util::trimTrailing(toTrim)
           + Util::trimLeading(toTrim);
  }

  void Util::gsleep(unsigned milli) {
    // System routines available vary with OS
#ifdef WIN32 
    Sleep(milli);
#else
    unsigned sec = milli/((unsigned) 1000);
    unsigned micro = 1000 * (milli - 1000*sec);
    if (sec) sleep(sec);
    usleep(micro);
#endif
  }

}






