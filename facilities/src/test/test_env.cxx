#include "facilities/commonUtilities.h"
#include <iostream>
using namespace std;
using namespace facilities;

int main(){
  int errors = 0;
  string dataPath = commonUtilities::getDataPath("facilities");
  if(dataPath == ""){
    cerr<<"Unable to get data path"<<endl;
    errors++;
  }
  else {
    cout << "facilities data path is: " << dataPath << endl;
  }
  string home = commonUtilities::getEnvironment("HOME");
  cout<<"$HOME is set to "<<home<<endl;
  commonUtilities::setEnvironment("COMMONUTILITIESTESTENV","test123");
  string testSet = commonUtilities::getEnvironment("COMMONUTILITIESTESTENV");
  if(testSet != "test123"){
    cerr<<"$COMMONUTILITIESTESTENV doesn't match test123"<<endl;
    errors++;
  }
  commonUtilities::setEnvironment("COMMONUTILITIESTESTENV","noOverwrite");
  testSet = commonUtilities::getEnvironment("COMMONUTILITIESTESTENV");
  if(testSet == "noOverwrite"){
    cerr<<"$COMMONUTILITIES incorrectly overwritten"<<endl;
    errors++;
  }
  commonUtilities::setEnvironment("COMMONUTILITIESTESTENV","overwrite",true);
  testSet = commonUtilities::getEnvironment("COMMONUTILITIESTESTENV");
  if(testSet != "overwrite"){
    cerr<<"$COMMONUTILITIES incorrectly not overwritten"<<endl;
    errors++;
  }
  testSet = commonUtilities::getEnvironment("FACILITIESXMLPATH");
  string xmlPath = commonUtilities::getEnvironment("INST_DIR");
  xmlPath = commonUtilities::joinPath(xmlPath, "xml");
  xmlPath = commonUtilities::joinPath(xmlPath, "facilities");
  if (testSet == xmlPath) {
    cout << "FACILITIESXMLPATH properly set with no explicit setupEnvironment call"
         << endl;
  }
  else if(testSet != ""){
    cerr<<"FACILITIESXMLPATH incorrectly set"<<endl;
    errors++;
  }
  commonUtilities::setupEnvironment();
  testSet = commonUtilities::getEnvironment("FACILITIESXMLPATH");
  if(testSet != commonUtilities::getXmlPath("facilities")){
    cerr<<"FACILITIESXMLPATH not set to right value"<<endl;
    errors++;
  }
  return errors;
}
