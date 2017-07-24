// $Id$
//  bpkt_streams.h
//  Author: Sawyer Gillespie
//          UW Department of Physics
//          hgillesp@u.washington.edu
//  Class definitions for packetized binary file i/o

#ifndef binpackeiostream_h_
#define binpackeiostream_h_

#include <fstream>

// kluge around incompatibilities w. egcs compiler:
#ifndef _MSC_VER    

// DISABLED CODE =======================================
#if 0

#include <iostream>
#include <string>

template <class Ch>
class char_traits : public string_char_traits<Ch> {
public:
    typedef int int_type;

    static Ch   eof () { return EOF; }
    static bool not_eof (const int_type& a) { return (a != EOF); }
    static int_type  to_int_type (Ch a) { return static_cast<int_type>(a); }
    static bool eq_int_type (const int_type& a, const int_type& b) { return a == b; }
    static Ch   to_char_type (const int_type& a) { return static_cast<char>(a); }  
};
typedef streambuf basic_streambuf;
typedef istream basic_istream;
typedef ostream basic_ostream;

#endif  
// DISABLED CODE =======================================

namespace bpkt {
    typedef std::ostream    ostream;    // this just maps bpkt::XXX to std::XXX
    typedef std::ofstream   ofstream;
    typedef std::istream    istream;
    typedef std::ifstream   ifstream;
    typedef std::streambuf  streambuf;
}

#else  // !defined _MSC_VER

namespace bpkt {

/* class basic_streambuf:
        class which packetizes data into binary packets before writing to a given
        stream. buffers all data until a sync () call is made, at which time the
        size of the buffer as well as the buffer itself is moved into the output stream.
        input occurs by first reading in the packet size, then pulling the entire
        packet from the stream. A flush on the input end will simply scuttle the buffer
        and re-initialize.
*/
template <class _Ch, class _Tr = std::char_traits<_Ch> >
#ifdef _MSC_VER
class   basic_streambuf :  public std::basic_streambuf<_Ch, _Tr> {
#else
class   basic_streambuf :  public streambuf {
#endif
public:
    basic_streambuf (std::ostream* o) : _out(o), _in(0), _maxinbuf(0) 
    {
        setbuf (new _Ch[STD_ALLOC_SIZE], STD_ALLOC_SIZE);
    }
    basic_streambuf (std::istream* i) : _out(0), _in(i), _maxinbuf(0) 
    {
        setbuf (new _Ch[STD_ALLOC_SIZE], STD_ALLOC_SIZE);
    }

    virtual ~basic_streambuf () { 
        delete _in; 
        delete _out; 
        delete eback();
        setg (0,0,0);
        delete pbase();
        setp (0,0);
    }

protected:

#ifndef _MSC_VER
    typedef _Tr::int_type int_type;
#endif

    // streambuf - derived class overloads

    // overflow - handle an overflow condition by expanding the output buffer
    //            by a standard amount
    virtual int overflow    ( int_type nCh = _Tr::eof() ) {
        // make a new (larger) buffer area
        std::streamsize nsz = epptr() - pbase() + STD_ALLOC_SIZE; 
        _Ch*    p = new _Ch[ nsz ];

        // error check
        if (!p) return _Tr::eof ();

        // copy over old data & destroy previous buffer
        std::streamsize osz = pptr() - pbase ();
        memcpy (p, pbase(), pptr() - pbase());   
        delete pbase();

        // set up the new buffer area
        setp (p, p + nsz);
        pbump (osz);    
        
        // 'consume' the nCh character
        if (!_Tr::eq_int_type(_Tr::eof(), nCh)) {
            *pptr() = _Tr::to_char_type(nCh);
            pbump (sizeof(_Tr::char_type));
        }

        return nCh;
    }

    // underflow - need to read in next packet from input
    virtual int_type    underflow () {
        if ((!_in) || (!_in->good()))   return _Tr::eof (); 

        // check to see if we already have data
        if (gptr() != egptr())  return _Tr::to_int_type (*gptr());

        // find out the size of the packet
        std::streamsize s;
        _in->read ((_Ch*)(&s), sizeof(std::streamsize));
        if (!_in->good())   return _Tr::eof ();

        // make sure there is adequate storage...
        if (s > _maxinbuf) {
            delete eback();
            _Ch*    p = new _Ch[s];
            setg (p, p, p);
            _maxinbuf = s;
        }

        // read next packet from the input stream
        _in->read (eback(), s);
        setg (eback(), eback(), eback() + s);

        if (gptr() != egptr())  return _Tr::to_int_type (*gptr());
        return _Tr::eof ();    
    }


    // sync - flush the buffer - default behavior is to just flush the buffer
    virtual int sync () {
        if (_out) {
            *((std::streamsize*)pbase()) = pptr() - pbase() - sizeof(std::streamsize);
            if (_out->good ()) {
                _out->write (pbase(), pptr()-pbase());
                setp (pbase(), epptr());    // reset the buffer
                pbump (sizeof(std::streamsize));
            } else return -1;
        } else {
            // flush the input
            setg (eback(), egptr(), egptr());
        }
        return 0;
    }

    // setbuf - set the buffer : if this is going out, account for the size at the
    //          beginning
    virtual
#ifdef _MSC_VER
    std::basic_streambuf<_Ch,_Tr>*  
#else
    std::streambuf*
#endif  
    setbuf (_Ch* p, std::streamsize s) {
        if (_out) {
            setp (p, p + s);
            pbump (sizeof(std::streamsize));
        } else if (_in) {
            setg (p, p, p);
            _maxinbuf = s;
        }
        return this;
    }

private:
    enum { STD_ALLOC_SIZE = 128 };  // standard allocation size

    std::istream*  _in;    // input stream
    std::ostream*  _out;   // output stream

    std::streamsize _maxinbuf;  // keep track of the maximum input buffer size...
};

/* basic_ostream - class definition
    The basic_ostream class acts as an ostream which assigns a new
    basic_ofstream object to its buffer, which is a basic_streambuf
    streambuffer.
*/
template <class _Ch, class _Tr = std::char_traits<_Ch> >
#ifdef _MSC_VER
class   basic_ostream : public std::basic_ostream<_Ch, _Tr>  {
#else
class   basic_ostream : public ostream {
#endif
public:
#ifdef _MSC_VER
    basic_ostream (std::basic_ostream<_Ch,_Tr>* o)
        : std::basic_ostream<_Ch,_Tr>
#else
    basic_ostream (std::basic_ostream* o)  
        : ostream
#endif
        (_buf = new basic_streambuf<_Ch,_Tr>(o))
    {}

    virtual ~basic_ostream () { delete _buf; }

private:
    basic_streambuf<_Ch,_Tr>* _buf;
};

/* basic_istream - class definition
    The basic_istream class acts as an istream which assigns a new
    basic_istream object to its buffer, which is a basic_streambuf
    streambuffer.
*/
template <class _Ch, class _Tr = std::char_traits<_Ch> >
#ifdef _MSC_VER
class   basic_istream : public std::basic_istream<_Ch, _Tr> {
#else
class   basic_istream : public istream {
#endif
public:
#ifdef _MSC_VER
    basic_istream (std::basic_istream<_Ch,_Tr>* i)
        : std::basic_istream<_Ch,_Tr>
#else
    basic_istream (std::basic_istream* i)
        : istream
#endif
        (_buf = new basic_streambuf<_Ch,_Tr>(i))
    {}

    virtual ~basic_istream () { delete _buf; }

private:
    basic_streambuf<_Ch,_Tr>* _buf;
};

template <class _Ty, class _Ch, class _Tr> 
inline basic_ostream<_Ch, _Tr>&  operator << ( basic_ostream<_Ch,_Tr>& o, const _Ty  t ) {
    int     sz = sizeof(_Ty);
    o.write ((const _Ch*)(&t), sz);
    return o;
}

template <class _Ty, class _Ch, class _Tr>
inline basic_istream<_Ch,_Tr>&  operator >> ( basic_istream<_Ch,_Tr>& i, _Ty& t ) {
    int     sz = sizeof(_Ty);
    i.read ((_Ch*)(&t),sz);
    return i;
}

/*
    Define file stream classes which use binary packets.
*/
template <class _Ch, class _Tr = std::char_traits<_Ch> >
class   basic_ofstream : public basic_ostream <_Ch, _Tr> {
public:
    basic_ofstream (const char* fname)
        : basic_ostream<_Ch, _Tr> 
#ifdef _MSC_VER
        (new std::basic_ofstream<_Ch,_Tr>(fname, std::ios::binary))
#else
        (new std::ofstream (fname, std::ios::binary))
#endif
    {}
};

template <class _Ch, class _Tr = std::char_traits<_Ch> >
class   basic_ifstream : public basic_istream <_Ch, _Tr> {
public:
    basic_ifstream (const char* fname)
        : basic_istream<_Ch, _Tr> 
#ifdef _MSC_VER
        (new std::basic_ifstream<_Ch,_Tr>(fname, std::ios::binary))
#else
        (new std::ifstream (fname, std::ios::binary)) 
#endif
    {}
};

/*  The classes binostream, binistream, and binstreambuf define binary stream 
    implementations based upon the char datatype (using byte resolution for allocation). 
*/ 

typedef basic_ostream<char, std::char_traits<char> >    ostream;
typedef basic_istream<char, std::char_traits<char> >    istream;
typedef basic_ofstream<char, std::char_traits<char> >   ofstream;
typedef basic_ifstream<char, std::char_traits<char> >   ifstream;
typedef basic_streambuf<char, std::char_traits<char> >  streambuf;

}   // namespace bpkt

#endif  // !defined _MSC_VER

#endif  // binpacketbuf_h_




