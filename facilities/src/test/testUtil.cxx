/// @file testUtil.cxx
#include <string>
#include <iostream>
#include "facilities/Util.h"
/** Miniscule program to test a couple modes of Util::expandEnvVar
 *  Caller should have an environment variable SRC with some
 *  sensible definition.
 */
int main(int, char**) {
  std::string name = std::string("${GLAST_EXT}/xerces");
  std::string nameAgain = std::string("${GLAST_EXT}/xerces");
  std::string oDelim = std::string ("${");
  std::string cDelim = std::string ("}");
  int ntrans;

  std::cout << "Test expandEnvVar: " << std::endl;
  try {
    ntrans = facilities::Util::expandEnvVar(&name, oDelim, cDelim);
    std::cout << "Translated name (right delimiters) is " 
              << name << std::endl;  
    ntrans = facilities::Util::expandEnvVar(&nameAgain);
    std::cout << "Translated name (wrong delimiters) is " 
              << nameAgain << std::endl;  
    if (!ntrans) {
      ntrans = facilities::Util::expandEnvVarOS(&nameAgain);
      std::cout << "Translated name using default delimiters for OS, got "
                << nameAgain << std::endl;
    }
  }
  catch (std::exception& err) {
    std::cout << "Caught standard exception; what() returns " 
              << err.what() << std::endl;
  }



  // Process running this must have environment variable SRC
  std::string multi = std::string("$(FACILITIESROOT)/$(SRC)");

  try {
    ntrans = facilities::Util::expandEnvVar(&multi);

    std::cout << "Translated name is " << multi << std::endl;
    std::cout << ntrans << " variables were translated." << std::endl;
  }
  catch (std::exception& err) {
    std::cout << "Caught standard exception; what() returns " 
              << err.what() << std::endl;
  }

  std::cout << std::endl << "Test expandEnvVarList" << std::endl;
  std::vector<std::string> fileCol;
  fileCol.push_back("$(GLAST_EXT),$(SRC)");
  fileCol.push_back("$(SRC)");
  std::vector<std::string> expandedCol;
  try {
      facilities::Util::expandEnvVarList(fileCol, expandedCol);
      std::cout << "After expanding env variables, the resulting vector has "
                << expandedCol.size() << " entries" << std::endl;
      std::vector<std::string>::const_iterator it;
      for (it = expandedCol.begin(); it != expandedCol.end(); it++) 
          std::cout << *it << std::endl;

  } catch(std::exception& err) {
      std::cout << "Caught standard exception;  what() returns "
                << err.what() << std::endl;
  } catch(...) {
      std::cout << "Caught undefined exception" << std::endl;
  }

  std::cout << std::endl << "Test itoa " << std::endl;
  // Test the new itoa routine
  std::string intStr;
  facilities::Util::itoa(12, intStr);
  std::cout << "My String is " << intStr << std::endl;

  // ..and the even newer utoa routine
  std::string unsignedStr;
  unsigned int u=0xffffffff;
  facilities::Util::utoa(u, unsignedStr);
  std::cout << "Result from utoa for 0xffffffff is: " 
            << unsignedStr << std::endl;

  u = 64;
  unsignedStr.clear();
  facilities::Util::utoa(u, unsignedStr, 16);
  std::cout << "Result from utoa (base 16 output) for (base 10) " << u
            << " is: " << unsignedStr << std::endl;

  // basename & stringTokenize
  std::string unixname("/a/path/myUnixFile.txt");
  std::string wname("C:\\a\\longer\\path\\myWindowsFile.txt");

  std::vector<std::string> tokens;
  unsigned int i;

  std::cout << std::endl << "Test stringTokenize and basename" << std::endl;
  facilities::Util::stringTokenize(unixname, "/", tokens);

  std::cout << "Processing string " << unixname << std::endl;
  for (i = 0; i < tokens.size(); i++) {
    std::cout << "Token " << i << " is: " << tokens[i] << std::endl;
  }

  std::cout << "basename is " << facilities::Util::basename(unixname) << std::endl;

  facilities::Util::stringTokenize(wname, "\\", tokens);

  std::cout << "Processing string " << wname << std::endl;
  for (i = 0; i < tokens.size(); i++) {
    std::cout << "Token " << i << " is: " << tokens[i] << std::endl;
  }

  std::cout << "basename is " << facilities::Util::basename(wname) << std::endl;

  //test keyValueTokenize to map routine
  std::cout << std::endl << "Test keyValueTokenize " << std::endl;
  std::string input1("apple=green,lemon=yellow,blueberry=blue");
  std::cout << "Input string: '" << input1 << "'" << std::endl;
  std::map<std::string,std::string> maptokens;
  facilities::Util::keyValueTokenize(input1,",",maptokens,"=", false);
  std::map<std::string,std::string>::const_iterator tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }

  std::cout <<"appending to previous map:"<<std::endl;
  std::string input2("apple2/green2,lemon2/yellow2,blueberry2/blue2");
  std::cout << "New string is '" << input2 << "'" << std::endl;
  facilities::Util::keyValueTokenize(input2,",",maptokens,"/",false);
  tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }
  
  std::cout <<"clearing the map first:"<<std::endl;
  facilities::Util::keyValueTokenize(input2,",",maptokens,"/",true);
  tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }

  std::cout << "Use a multi-character pairDelimiter argument " << std::endl;
  std::string input3("apple2:=green2 lemon2:=yellow2 blueberry2:=blue2");
  std::cout << "input is: '" << input3 << "'" << std::endl;
  facilities::Util::keyValueTokenize(input3," ",maptokens,":=");
  tokens_itr=maptokens.begin();
  while (tokens_itr!=maptokens.end()) {
    std::cout << "Token key " <<(*tokens_itr).first  << " and value: " 
	      << (*tokens_itr).second << std::endl;
    tokens_itr++;
  }


  // Test stringToDouble routine
  std::cout << std::endl << "Test stringToDouble " << std::endl;
  std::string okDouble("3.14159");
  std::string badDouble("3.garbage56");
  std::string okDouble2("3.");

  double result = -1;

  try {
    result = facilities::Util::stringToDouble(okDouble);
    std::cout << "Converted (string) " << okDouble << " to (double) " 
              << result << std::endl;
  }
  catch (std::exception& err) {
    std::cout << "Caught standard exception; what() returns " 
              << err.what() << std::endl;
  }

  try {
    result = facilities::Util::stringToDouble(badDouble);
    std::cout << "Converted (string) " << badDouble << " to (double) " 
              << result << std::endl;
  }
  catch (std::exception& err) {
    std::cout << "Caught standard exception; what() returns " 
              << err.what() << std::endl;
  }
  try{
    result = facilities::Util::stringToDouble(okDouble2);
    std::cout << "Converted (string) " << okDouble2 << " to (double) " 
	      << result << std::endl;
  }
  catch (std::exception& err){
    return 1;
  }

  // Test stringToInt and stringToUnsigned routines
  std::cout << std::endl << "Test stringToInt " << std::endl;

  std::string okInt("33550");
  std::string badInt1("3garbage56");
  std::string badInt2("garbage356");
  std::string negInt("-23");

  int intResult = -1;
  unsigned unResult = 0;

  try {
    intResult = facilities::Util::stringToInt(okInt);
    std::cout << "Converted (string) " << okInt << " to (int) " 
              << intResult << std::endl;
  }
  catch (std::exception& err) {
    std::cout << "Caught standard exception; what() returns " 
              << err.what() << std::endl;
  }

  try {
    intResult = facilities::Util::stringToInt(badInt1);
    std::cout << "Converted (string) " << badInt1 << " to (int) " 
              << intResult << std::endl;
  }
  catch (std::exception& ex) {
    std::cout << "Caught std::exception, what() returns " 
              << ex.what() << std::endl;
  }

  try {
    intResult = facilities::Util::stringToInt(badInt2);
    std::cout << "Converted (string) " << badInt2 << " to (int) " 
              << intResult << std::endl;
  }
  catch (std::exception& ex) {
    std::cout << "Caught std::exception, what() returns " 
              << ex.what() << std::endl;
  }

  std::cout << "Attempt to convert " << okInt << " to unsigned " << std::endl;
  try {
    unResult = facilities::Util::stringToUnsigned(okInt);
    std::cout << "Converted (string) " << okInt << " to (unsigned) " 
              << unResult << std::endl;
  }
  catch (std::exception& ex) {
    std::cout << "Caught std::exception, what() returns " 
              << ex.what() << std::endl;
  }

  std::cout << "Attempt to convert " << negInt << " to unsigned " << std::endl;
  try {
    unResult = facilities::Util::stringToUnsigned(negInt);
    std::cout << "Converted (string) " << negInt << " to (unsigned) " 
              << unResult << std::endl;
  }
  catch (std::exception& ex) {
    std::cout << "Caught std::exception, what() returns " 
              << ex.what() << std::endl;
  }

  // Try out trimLeading method
  {
    std::cout << std::endl << "Test trimLeading " << std::endl;
    std::string string0;
    std::string string1("  starts with 2 blanks");
    std::string string2("\nstarts with newline");
    std::string string3("no leading whitespace");
    
    unsigned nTrimmed = facilities::Util::trimLeading(&string0);
    std::cout << "Trimmed " << nTrimmed << " from string0; has new value : "
	      << "*BOS*" << string0 << "*EOS*" << std::endl;

    nTrimmed = facilities::Util::trimLeading(&string1);
    std::cout << "Trimmed " << nTrimmed << " from string1; has new value : "
	      << "*BOS*" << string1 << "*EOS*" << std::endl;
    
    nTrimmed = facilities::Util::trimLeading(&string2);
    std::cout << "Trimmed " << nTrimmed << " from string2; has new value : "
	      << "*BOS*" << string2 << "*EOS*" <<  std::endl;
    
    nTrimmed = facilities::Util::trimLeading(&string3);
    std::cout << "Trimmed " << nTrimmed << " from string3; has new value : "
	      << "*BOS*" << string3 << "*EOS*" << std::endl;
  }


  // Try out trimTrailing method
  {
    std::cout << std::endl << "Test trimTrailing " << std::endl;
    std::string string0;
    std::string string1("ends with 2 blanks  ");
    std::string string2("ends with newline\n");
    std::string string3("no trailing whitespace");
    
    unsigned nTrimmed = facilities::Util::trimLeading(&string0);
    std::cout << "Trimmed " << nTrimmed << " from string0; has new value : "
	      << string0 << "*EOS*" << std::endl;

    nTrimmed = facilities::Util::trimTrailing(&string1);
    std::cout << "Trimmed " << nTrimmed << " from string1; has new value : "
	      << string1 << "*EOS*" << std::endl;
    
    nTrimmed = facilities::Util::trimTrailing(&string2);
    std::cout << "Trimmed " << nTrimmed << " from string2; has new value : "
	      << string2 << "*EOS*" <<  std::endl;
    
    nTrimmed = facilities::Util::trimTrailing(&string3);
    std::cout << "Trimmed " << nTrimmed << " from string3; has new value : "
	      << string3 << "*EOS*" << std::endl;
  }

  // sleep
  std::cout << "About to sleep .5 seconds..zzz" << std::endl;
  facilities::Util::gsleep(500);
  std::cout << "my, that was refreshing.  Now for 10.5 seconds...zzz"
            << std::endl;
  facilities::Util::gsleep(10500);
  std::cout << " Ah! " << std::endl;

  return 0;
}
