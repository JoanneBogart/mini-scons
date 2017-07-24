#ifndef _COMMON_UTILITIES_H_
#define _COMMON_UTILITIES_H_

#include <string>
/** @file commonUtilities.h
    @author N. Golpayegani

    This file declares the class commonUtilities for basic access to environment variables
*/

namespace facilities {
#ifdef SCons
  // See commonUtilities.cxx for definition of this class. It has information
  // and tools used only by commonUtilities functions
  class MyInternal;
#endif

  /// Class used to obtain various file paths and set environment variables
  class commonUtilities{
  public:
    /** Function to return the data location of a given package 
	@param pacakge String representing a package
    */
    static std::string getDataPath(const std::string &package);
    /** Function to return the job options location of a given package 
	@param pacakge String representing a package
    */
    static std::string getJobOptionsPath(const std::string &package);
    /** Function to return the xml location of a given package
	@param package String representing the name of a package
    */
    static std::string getXmlPath(const std::string &package);
    /** Function to return the pfiles location of a package
	@param package String represnting the name of a package
    */
    static std::string getPfilesPath(const std::string &package);
    /** Function to get the top level path of a package
	@param package String representing the name of a package
    */
    static std::string getPackagePath(const std::string &package);
    /** Function to set an environment variable
	@param name String representing the name of the environment variable
	@param value String representing the value to assign to the environment variable
	@param overwrite Boolean if set to true will overwrite an existing environment variable
    */
    static void setEnvironment(const std::string &name, const std::string &value, bool overwrite=false);
    /** Function that returns the value of an environment variable
	@param name String representing the name of an environment variable to be retrieved
    */
    static std::string getEnvironment(const std::string &name);
    static std::string joinPath(const std::string &first, const std::string &second);
#ifdef SCons
    static bool pathFound(const std::string &path);
#endif
    static void setupEnvironment();
  private:
    static std::string getPackageRoot(const std::string &package);
#ifdef SCons    
    static  MyInternal* myInternal;
#endif
  };
}

#endif
