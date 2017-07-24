//	$Id$
#ifndef FIFO_h
#define FIFO_h 1

#ifdef _MSC_VER
# pragma warning(disable: 4786)
#endif

#include <vector>
#include <cassert>

//## Class: FIFO; Parameterized Class
//	Implements the FIFO class. This is not a readout
//	component, but a standard First-In-First-Out storage
//	class.

using std::vector;

template <class _T>
class FIFO : private vector<_T>  //## Inherits: _T
{
  public:
    FIFO (unsigned int depth = 0)   
	: vector<_T>(), m_depth(depth)    
    {
	if (depth > 0)	reserve ( depth );

	// if the depth of the fifo is fixed, then reserve enough space to contain it entirely
	// FIFO operates in two modes: one for a fixed depth,
	// and another for a non-fixed depth (unlimited)
    }

    FIFO (const FIFO& f)
	: vector<_T>(f), m_depth(f.m_depth)
    {
    }


      //	Push x into the FIFO. Returns true if x was successfully
      //	stored, false if not (FIFO was full).
      bool push (const _T& x)
      {
	  if ((m_depth != 0) && (size() >= m_depth))	return false;

	  push_back( x );

	  return true;
      }

      //	Returns the current front of the FIFO buffer, if it
      //	exists. Caller must perform range checking to see that
      //	the FIFO is not empty prior to this call.
      _T pop ()
      {
	  assert(size()>0);
	  _T	value (*begin());
	  erase(begin());
	  
	  // limited depth mode
	  
	  return value;
      }

      _T&   front   () { return vector<_T>::front(); }
      _T&   back    () { return vector<_T>::back(); }

      const _T&   front   () const	{ return vector<_T>::front(); }
      const _T&   back    () const	{ return vector<_T>::back(); }

      bool  empty   () const	{ return (size() == 0); }

      //	Get the maximum size of this FIFO.
      unsigned int maxSize () const
      {
	  return m_depth;
      }

      //	Return the current size of the FIFO buffer.
      unsigned int size () const
      {
	  return vector<_T>::size();
      }

      void clear ()
      {
	 vector<_T>::clear();
      }

      //	Writes the contents of the FIFO to the output stream.
      void printOn (std::ostream& out = cout)
      {
	  short i = 1;
	  for (vector<_T>::reverse_iterator it = rbegin(); it != rend(); ++it) {
	      out << (*it)();
	      if ((i % 8) == 0)	out << "\n";
	      else  out << "\t";
	      i++;
	  }
      }

      bool  full () const { return size() == m_depth; }

      bool  avail () const { return size() < m_depth; }

  private: 

      //	maximum depth for the fifo
      unsigned int m_depth;
};

#endif
