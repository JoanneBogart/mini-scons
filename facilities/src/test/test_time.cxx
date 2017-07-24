// $Header$
/** @file test_meta.cxx
    Sample program to exercise calibration metadata database services
*/

#include <string>
#include <iostream>
#include "facilities/Timestamp.h"


int main(int, char**) {
  using facilities::Timestamp;
  using facilities::BadTimeInput;


  try {
    long int       zero = 0;

    Timestamp unixCreation(zero);
    //    Timestamp gmttest("1970-01-01 00:00");
    long int        aTime = 4000;

    Timestamp aTimestamp(aTime);
    facilities::Timestamp   cur;

    std::cout << "unix creation time is " << unixCreation.getString()
              << std::endl;


    std::cout << "aTimestamp is " << aTimestamp.getString()
              << std::endl;

    std::cout << "cur time (GMT)  is " << cur.getString()
              << std::endl;

    std::string missionStartString("2001-1-1 00:00");

    Timestamp   missionStart(missionStartString);

    std::cout << "Supplied string: " << missionStartString << std::endl;
    std::cout << "Retrieved: " << missionStart.getString() << std::endl;

    std::string PDTString("2005-4-4 12:25");
    Timestamp   PDTTime(PDTString, 25200);

    std::cout << "Supplied PDT string: " << PDTString << std::endl;
    std::cout << "Retrieved: " << PDTTime.getString() << std::endl;
    std::string leap3("2013-02-02 01:00");
    Timestamp leap3TS(leap3);
    std::cout << "Starting with string ='" + leap3 << std::endl;
    std::cout << "Make timestamp object; output again as string: ";
    std::cout << leap3TS.getString() << std::endl;
    std::cout << "and now invoke getString with leap seconds arg:" << std::endl;
    std::cout << leap3TS.getString(true) << std::endl;

    std::string tooEarly("1770-12-17 12:00");
    Timestamp  tooEarlyTime(tooEarly);
    std::cout << "Should not have been able to create timestamp " 
              << "with string representation" 
              << tooEarlyTime.getString() << std::endl;

  }
  catch (std::exception& e) {
    std::cout << "Exception message:  " << e.what() << std::endl;
  //  catch (const BadTimeInput e) {
  //    std::cout << "Exception message:  " << e.complaint << std::endl;
  }

  return 0;

}

