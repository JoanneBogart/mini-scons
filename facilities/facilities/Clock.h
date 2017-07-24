// $Id$
//
//  Original Author: Sawyer Gillespie
//                   hgillesp@u.washington.edu
//

#ifndef _H_CLOCK
#define _H_CLOCK 1

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// includes
#include <cmath>

// forward declarations
typedef double ClockWord;

// class Clock
//  This is a general class for representing a clock value in terms
//  of some integral number of counts (ie. CPU cycles).
//
class Clock 
{
  public:
      Clock ()
	: m_time(0), m_freq(2.E7) 
      {}

      virtual ClockWord count (double t)
      {
	m_time += t;
	return m_time; 
      }

      virtual ClockWord count (unsigned int c)
      {
	m_time += c * 1./m_freq; 
	return m_time; 
      }

      virtual void reset ()
      {
	m_time = 0; 
      }

      double freq () const
      {
	return m_freq; 
      }

      virtual unsigned int cycles ()
      {
	return static_cast<unsigned>(ceil(m_time / m_freq)); 
      }

      virtual double elapsed ()
      {
	return m_time; 
      }

  private:
      ClockWord m_time; //  current elapsed time	in clock cycles
      double m_freq;    //  frequency in Hz
};

#endif
