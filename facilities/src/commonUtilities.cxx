#include "facilities/commonUtilities.h"
#include <iostream>
#include <algorithm>
#if defined(SCons) || defined(HEADAS)
#include <sstream>
#include <fstream>
#include <set>
#include <vector>
#include <cstring>
#include "config.h"
#endif

#ifdef ScienceTools
#include <stdexcept>
#endif

#if defined(__APPLE__)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

#ifndef WIN32
#include <sys/stat.h>         // added to check for dir. existence
#if !defined(SCons) && !defined(HEADAS)
extern char **environ;
#endif
#endif

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return std::strcmp(s1, s2) < 0;
  }
};

namespace facilities {
#ifdef SCons
  class MyInternal {
  public:
    bool m_hasSupersede;
    std::set<const char*, ltstr> m_superSet;
    std::vector<std::string> m_superPkgs;
    std::string m_instDir;
    std::string m_baseDir;
    
    MyInternal();
    std::string getSconsPackageRoot(const std::string &package);
    
  };
  MyInternal* commonUtilities::myInternal = 0;
  // Constructor saves values for INST_DIR, BASE_DIR
  // If supersede area exists, read file from supersede area
  // containing list of active supersede packages and save in std::set
  MyInternal::MyInternal() {
    m_superPkgs.clear();
    m_superSet.clear();
    m_instDir = commonUtilities::getEnvironment("INST_DIR");
    m_baseDir = commonUtilities::getEnvironment("BASE_DIR");
    m_hasSupersede =  (m_instDir != m_baseDir);
    if (!m_hasSupersede) return;

    // Otherwise read $INST_DIR/data/supersede.pkgList,
    // store packages found in myInternal->superPkgs;
    std::string superPath = 
      commonUtilities::joinPath(commonUtilities::joinPath(m_instDir, "data"), 
                                "supersede.pkgList");
    std::ifstream fp(superPath.c_str());
    if (fp.is_open()) {
      char buf[100];
      char* ln = &buf[0];
      while (fp.good()) {
        fp.getline(buf, 100);
        std::string lnCopy(ln);
        if (fp.gcount() ==  0 ) continue;
        if ((*ln == '#') || (*ln == ' ') || (*ln == 0)) continue;
        m_superPkgs.push_back(std::string(ln));
      }
      for (int i = 0; i < m_superPkgs.size(); i++ ) {
        m_superSet.insert(m_superPkgs[i].c_str());
      }
      fp.close();
    } else {
      std::cerr << "Unable to open " << superPath << std::endl;
    }
  }
  // Scons package root is base dir unless the package is an
  // active package in supersede area
  std::string MyInternal::getSconsPackageRoot(const std::string &package) {
    std::string rt = m_baseDir;
    if (m_hasSupersede) { // check if we're a supersede pkg
      if (m_superSet.find(package.c_str()) != m_superSet.end()) rt = m_instDir;
    }
    return rt;
  }
#endif
  std::string commonUtilities::getPackagePath(const std::string &package){
#ifdef SCons
    return joinPath(commonUtilities::getPackageRoot(package), package);
#else
#ifdef HEADAS
    return joinPath(commonUtilities::getPackageRoot(package), package);
#else
    return commonUtilities::getPackageRoot(package);
#endif
#endif
  }

  std::string commonUtilities::getDataPath(const std::string &package){

#ifdef SCons
    if (commonUtilities::myInternal == 0) {
      commonUtilities::setupEnvironment();
    }
    std::string packageRoot = 
      commonUtilities::myInternal->getSconsPackageRoot(package);
    std::string dataPath = joinPath(joinPath(packageRoot, "data"), package);

    return  dataPath;
#else
#ifdef HEADAS
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string dataPath = joinPath(packageRoot, "refdata");
    dataPath = joinPath(dataPath, "fermi");
    return joinPath(dataPath, package);
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "data");
#endif
#endif
  }

  std::string commonUtilities::getJobOptionsPath(const std::string &package) {
#ifdef SCons
    if (commonUtilities::myInternal == 0) {
      commonUtilities::setupEnvironment();
    }

    std::string packageRoot = 
      commonUtilities::myInternal->getSconsPackageRoot(package);

    std::string joPath = joinPath(joinPath(packageRoot, "jobOptions"), package);
    return joPath;
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "src");
#endif
  }

  std::string commonUtilities::getXmlPath(const std::string &package){
#ifdef SCons
    if (commonUtilities::myInternal == 0) {
      commonUtilities::setupEnvironment();
    }

    std::string packageRoot = 
      commonUtilities::myInternal->getSconsPackageRoot(package);
    std::string xmlPath = joinPath(joinPath(packageRoot, "xml"), package);

    return xmlPath;
#else
#ifdef HEADAS
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string xmlLocation = joinPath(packageRoot, "xml");
    xmlLocation = joinPath(xmlLocation, "fermi");
    return joinPath(xmlLocation, package);
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "xml");
#endif
#endif
  }

  std::string commonUtilities::getPfilesPath(const std::string &package){
#ifdef SCons
    if (commonUtilities::myInternal == 0) {
      commonUtilities::setupEnvironment();
    }

    std::string packageRoot = 
      commonUtilities::myInternal->getSconsPackageRoot(package);
    std::string pfilesLocation = joinPath(packageRoot, "syspfiles");
    return pfilesLocation;
#else
#ifdef HEADAS
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string pfilesLocation = joinPath(packageRoot, "syspfiles");
    return pfilesLocation;
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "pfiles");
#endif
#endif
  }

  void commonUtilities::setEnvironment(const std::string &name, const std::string &value, bool overwrite){
    if(getenv(name.c_str())==NULL || overwrite){
#ifdef WIN32
      _putenv((name+"="+value).c_str());
#else
      setenv(name.c_str(), value.c_str(),1);
#endif
    }
  }

  std::string commonUtilities::getEnvironment(const std::string &name){
    const char *env = getenv(name.c_str());
    std::string toReturn;
    if(env!=NULL)
      toReturn = env;
    return toReturn;
  }

  std::string commonUtilities::getPackageRoot(const std::string &package){
    std::string packageRoot;
#ifdef SCons
    return commonUtilities::myInternal->getSconsPackageRoot(package);

#else
#ifdef HEADAS
    const char *env = getenv("FERMI_INST_DIR");
    if(env != NULL)
      packageRoot = env;
#else
    std::string upperCase=package;
    transform(upperCase.begin(),upperCase.end(),upperCase.begin(),(int(*)(int)) toupper);
    upperCase=upperCase+"ROOT";
    const char *env = getenv(upperCase.c_str());
    if(env!=NULL)
      packageRoot = env;
#endif
#endif
    return packageRoot;
  }

  std::string commonUtilities::joinPath(const std::string &first, const std::string &second){
#ifdef WIN32
    return first+"\\"+second;
#else
    return first+"/"+second;
#endif
  }
#ifdef SCons
  bool commonUtilities::pathFound(const std::string& path) {
    // For the time being, don't even try for Windows since
    // it fails even when it shouldn't
#ifdef WIN32
    return true;
#else
    // Check to see if rel path exists first for INST_DIR
    // if that fails, try BASE_DIR

    //std::fstream filestr;
    //if (filestr.is_open()) {  // all is well, just close the file
    //  filestr.close();
    //  return true;
    //}

    struct stat buf;
    int ret = stat(path.c_str(), &buf);
    if (ret == 0) {
      //std::cout << "This path ok: " << path << std::endl;
      return true;
    } else {
      //std::cout << "For path " << path << " stat was " << ret << std::endl;
      return false;
    }
#endif
  }
#endif

  // Several things are accomplished here.  Only the first is required
  // for CMT builds.  All others are for one or both of SCons, HEADAS
  // 1. set up package-specific environment variables like MYPKGXMLPATH
  // 2. obf load path used by OnboardFilter
  //        SCons, only if obf external is used
  // 3. extFiles stuff EXTFILESSYS, TIMING_DIR ScienceTools & GlastRelease
  //                   PARAMFILESROOT          GlastRelease
  // 4. caldb    Used for SCons and HEADAS builds of ScienceTools
  void commonUtilities::setupEnvironment(){
#if defined(SCons)
    if (myInternal == 0 ) myInternal = new MyInternal();
#endif
#if defined(SCons) || defined(HEADAS)
    std::stringstream packages;
    packages << PACKAGES;
    while(!packages.eof()){
      std::string package, packageUpper;
      packages>>std::ws>>package>>std::ws;
      // 'package' may be of form myPkg-xx-yy-zz, in which case
      // must strip off suffix
      std::string nameOnly = package;
      // If there is a suffix it will be 9 characters long
      int dashPos = package.size() - 9;
      if (dashPos > 0) {
        if (package[dashPos] == '-') {
          if (std::string("0123456789").find(package[dashPos + 1])
              != std::string::npos) {
            nameOnly = std::string(package, 0, dashPos);
          }
        }
      }
      packageUpper = nameOnly;
      transform(packageUpper.begin(), packageUpper.end(), packageUpper.begin(), (int(*)(int)) toupper);
      setEnvironment(packageUpper+"XMLPATH", getXmlPath(nameOnly));
      setEnvironment(packageUpper+"DATAPATH", getDataPath(nameOnly));
#ifdef GlastRelease
      setEnvironment(packageUpper+"JOBOPTIONSPATH",getJobOptionsPath(nameOnly));
#endif
    }
#else  // Not HEADAS; not SCons
    if(environ!=NULL){
      int counter=0;
      while(environ[counter]!=NULL){
	std::string env = environ[counter];
	std::string::size_type pos = env.find_first_of('=');
	if(pos != std::string::npos){
	  env = env.substr(0, pos);
	  pos = env.rfind("ROOT");
	  if(pos != std::string::npos && pos+4 == env.size()){
	    env = env.substr(0, pos);
	    if(env != ""){
	      setEnvironment(env+"XMLPATH", getXmlPath(env));
	      setEnvironment(env+"DATAPATH", getDataPath(env));
	      setEnvironment(env+"PFILESPATH", getPfilesPath(env));
              setEnvironment(env+"JOBOPTIONSPATH", getJobOptionsPath(env));
	    }
	  }
	}
	counter++;
      }
    }
    return;    // all done for CMT builds
#endif

    //  If obf external and SCons build, set OBFLDPATH
#if defined(SCons)
#ifdef REL_OBFLDPATH
    std::string obfldPath(REL_OBFLDPATH);
    setEnvironment("OBFLDPATH", 
                   joinPath(getEnvironment("GLAST_EXT"), obfldPath));
#endif
#endif

    // EXTFILES and related environment variables
#ifndef HEADAS
#ifdef extFiles
    setEnvironment("EXTFILESSYS", joinPath(joinPath(getEnvironment("GLAST_EXT"), "extFiles"), extFiles));
    setEnvironment("TIMING_DIR", joinPath(joinPath(joinPath(getEnvironment("GLAST_EXT"), "extFiles"), extFiles), "jplephem"));
#endif
#ifdef GlastRelease
  // Define environment variable PARAMFILESROOT used by Gaudi to find
  // particleTable.txt.  This value will be overridden if it's set
  // in job options
  // PARMAFILESROOT should be $EXTFILESSYS/gaudi
  std::string tmp = 
    commonUtilities::joinPath(commonUtilities::getEnvironment("EXTFILESSYS"),
                              "gaudi");
    commonUtilities::setEnvironment("PARAMFILESROOT", tmp);
#endif

#else   // HEADAS defined
    setEnvironment("TIMING_DIR", joinPath(getPackageRoot("timeSystem"), "refData"));
#endif


    // caldb. 
#ifdef ScienceTools
    std::string calDbData = getDataPath("caldb");
    if (calDbData == "") {
       throw std::runtime_error("CALDB environment variable not set properly.");
    }
    setEnvironment("CALDB", calDbData);
    setEnvironment("CALDBCONFIG", 
                   joinPath(joinPath(joinPath(calDbData, "software"), 
                                     "tools"), "caldb.config"));
    setEnvironment("CALDBALIAS", 
                   joinPath(joinPath(joinPath(calDbData, "software"), 
                                     "tools"), "alias_config.fits"));

  // catalogProducts and diffuseModels
#ifdef catalogProducts
    setEnvironment("CATALOGPRODUCTSPATH", joinPath(joinPath(getEnvironment("GLAST_EXT"), "catalogProducts"), catalogProducts));
#endif

#ifdef diffuseModels
    setEnvironment("DIFFUSEMODELSPATH", joinPath(joinPath(getEnvironment("GLAST_EXT"), "diffuseModels"), diffuseModels));
#endif

#endif
  }
}

