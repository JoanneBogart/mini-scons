// $Header$

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "facilities/Timestamp.h"

namespace facilities {
  const double Timestamp::julian1970 = 2440587.5;
  const int    Timestamp::secPerDay =  (24*60*60);
  const double Timestamp::inverseNano = 1000 * 1000 * 1000;
  const int    Timestamp::inverseNanoInt = 1000 * 1000 * 1000;
  const long int Timestamp::maxInt = 0x7fffffff;
  
  Timestamp::TZOffset Timestamp::s_tz;

  // return current time (resolution of 1 second)
  Timestamp::Timestamp() : m_nano(0) {
    m_time = time(0);
  }

  // time specified as seconds since 1970 in the timezone corresponding
  // to tzOffset (measured in seconds) west of GMT.  So, for example, 
  // a local time PST should have tzOffset = 28800; PDT would be 25200;
  // EST is 18000 and so forth. This offset can be read from the
  // system variable __timezone
  Timestamp::Timestamp(long int seconds, int nano, int tzOffset)
    : m_time((time_t) seconds), m_nano(nano)
  {
    if  ((nano >= inverseNanoInt)  || (nano < 0) || (seconds < 0))
      throw BadTimeInput("facilities::Timestamp bad nano argument");
    seconds += tzOffset;
  }

  // time specified as Julian date
  Timestamp::Timestamp(double julian) {
    double secs;
    secs = (julian - julian1970) * secPerDay;

    if ((fabs(secs) > maxInt) || (secs < 0) )
      throw BadTimeInput("Julian time not in range [1970, 2037]");

    m_time = (long int) secs;

    // In case time is negative, truncation will go the "wrong way".
    // Want m_time always <= secs
    if (m_time > secs) m_time--;
    m_nano = (int) ((secs - m_time)/inverseNano);
  } 

  // Time specified as string
  Timestamp::Timestamp(const std::string& str, int tzOffset)
    : m_nano(0) {
    m_time = toBinary(str);
    m_time += tzOffset;
  }

  // Time specified with independent fields
  Timestamp::Timestamp(int year, int month, 
                       int day, int hour, 
                       int minute, int second,
                       int nano) :  m_nano(nano) {
    struct tm fields;
    // check input   
    // for now don't bother checking whether, e.g., someone
    // specified April 31
    if ((month < 1 ) || (month > 12) || (day < 1) || (day > 31) ||
        (hour < 0) || (hour > 23) || 
        (minute < 0) || (minute > 59) || 
        (second < 0 ) || (second >= 60) ||
        (year < 1970) || (year > 2037) || 
        (nano < 0 ) || (nano >= inverseNanoInt) ) 
      throw BadTimeInput("facilities::Timestamp Bad subfield");

    fields.tm_year = year - 1900;
    fields.tm_mon = month - 1;
    fields.tm_mday = day;
    fields.tm_hour = hour;
    fields.tm_min = minute;
    fields.tm_sec = (long int) second;
    fields.tm_wday = -1;
    fields.tm_yday = -1;
    
    // let system figure out whether daylight savings time is in effect
    fields.tm_isdst = 0;

    //    m_time = timegm(&fields);
    m_time = mktime(&fields) - Timestamp::s_tz.m_tzseconds;
  }

  std::string Timestamp::getString(bool withLeap) const {
    std::string str;

    toString(m_time, str);
    if (!withLeap) return str;

    // Otherwise try to account for leap seconds in an especially ugly way:
    // decrease second count so that string representation comes out right

    int decr = getAdjustment();
    Timestamp* temp = new Timestamp(str);
    *temp = Timestamp(temp->getClibTime() - decr, temp->getNano());
    str = temp->getString(false);
    delete temp;
    return str;
  }

  double  Timestamp::getJulian() const {
    double julian = (m_time +  m_nano/inverseNano)/secPerDay;
    julian += julian1970;
    return julian;
  }


  time_t Timestamp::toBinary(const std::string& strTime) {
    // Break out fields
    struct tm fields;

    unsigned int pos;
    unsigned int oldPos = 0;

    // Three delimiter characters may occur in ascii time representation.
    // First is hyphen.
    char  delim = '-';

    pos = strTime.find(delim, oldPos);

    // Must have two occurrences of hyphen delimiter
    if (pos >= strTime.size()) return 0;

    fields.tm_year = atoi((strTime.substr(oldPos, pos)).c_str()) - 1900;
    if ((fields.tm_year < 70) || (fields.tm_year > 137)) 
      throw BadTimeInput("facilities::Timestamp  bad year");
    oldPos = pos + 1;
    pos = strTime.find(delim, oldPos);

    // Should have at least two occurrences of delim
    if (pos >= strTime.size()) 
      throw BadTimeInput("Bad string format for facilities::Timestamp");

    fields.tm_mon = atoi((strTime.substr(oldPos, pos)).c_str()) - 1;
    if ((fields.tm_mon < 0) || (fields.tm_mon > 11)) 
      throw BadTimeInput("facilities::Timestamp bad month");

    // day
    oldPos = pos + 1;

    // A space separates time from date (if time is present at all)
    delim = ' ';
    pos = strTime.find(delim, oldPos);

    fields.tm_mday = atoi((strTime.substr(oldPos, pos)).c_str());

    if ((fields.tm_mday < 1) || (fields.tm_mday > 31))
    throw BadTimeInput("facilities::Timestamp bad day of month");

    // Remaining fields in string representation default to 0.
    fields.tm_hour = fields.tm_min = fields.tm_sec = 0;

    if (pos < strTime.size() ) {   // more fields to process
      delim = ':';
      oldPos = pos + 1;

      pos = strTime.find(delim, oldPos);

      fields.tm_hour = atoi((strTime.substr(oldPos, pos)).c_str());
      if ((fields.tm_hour > 23) || (fields.tm_hour < 0)) 
      throw BadTimeInput("facilities::Timestamp bad hour");

      
      if (pos < strTime.size() ) {
        oldPos = pos + 1;
        pos = strTime.find(delim, oldPos);
        fields.tm_min = atoi((strTime.substr(oldPos, pos)).c_str());
        if ((fields.tm_min > 59) || (fields.tm_hour < 0))
          throw BadTimeInput("facilities::Timestamp bad minutes");

        if (pos < strTime.size() ) {
          oldPos = pos + 1;
          pos = strTime.find(delim, oldPos);
          fields.tm_sec = atoi((strTime.substr(oldPos, pos)).c_str());
          if ((fields.tm_sec > 59) || (fields.tm_hour < 0)) 
            throw BadTimeInput("facilities::Timestamp bad seconds");
        }
      }
    }

    fields.tm_wday = -1;
    fields.tm_yday = -1;
    fields.tm_isdst = 0;
    return mktime(&fields)  - Timestamp::s_tz.m_tzseconds;
  }
  
  void Timestamp::toString(time_t bin, std::string& strTime) {
    struct tm * fields = gmtime(&bin);

    strTime.resize(0);
    
    char buf[20];
    char* bufPtr = &buf[0];
    sprintf(buf, "%i", fields->tm_year + 1900);
    strTime += bufPtr; strTime +=  "-";
    sprintf(buf, "%02i", fields->tm_mon +1);
    strTime += bufPtr; strTime +=  "-";
    sprintf(buf, "%02i", fields->tm_mday);
    strTime += bufPtr; strTime +=  " ";
    sprintf(buf, "%02i", fields->tm_hour);
    strTime += bufPtr; strTime +=  ":";
    sprintf(buf, "%02i", fields->tm_min);
    strTime += bufPtr; strTime +=  ":";
    sprintf(buf, "%02i", fields->tm_sec);
    strTime += bufPtr;
  }


  Timestamp::TZOffset::TZOffset() {
    struct tm fields;

    // Set it up for Jan 1, 1970 at 12:00
    fields.tm_year = 70;
    fields.tm_mon = 0;
    fields.tm_mday = 1;
    fields.tm_hour = 12;
    fields.tm_min = 0;
    fields.tm_sec = 0;
    fields.tm_isdst = 0;

    m_tzseconds = mktime(&fields) - 12*60*60;
    m_isDst = fields.tm_isdst;
  }

  /* Returns number of leap seconds since MET till our value */
  int Timestamp::getAdjustment() const {
    int adjust = 0;
    if (*this > Timestamp("2006-01-01", 0)) adjust++;
    if (*this > Timestamp("2009-01-01", 0)) adjust++;
    if (*this > Timestamp("2012-07-01", 0)) adjust++;
    if (*this > Timestamp("2015-07-01", 0)) adjust++;

    return adjust;
  }
}






