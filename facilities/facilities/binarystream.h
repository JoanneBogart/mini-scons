// $Id$
//  binarystream.h
//  Author: Sawyer Gillespie
//          UW Department of Physics
//          hgillesp@u.washington.edu
//  Class definitions for i/o strictly into a buffer (rather than some real I/O)

#ifndef binarystreams_h_
#define binarystreams_h_

#include "streamdef.h"

/* basic_binstreambuf - class definition
    The basic_binstreambuf class provides a means for sequential access
    to a block of memory. This class manages a buffer of a pre-determined
    size - which is passed in to its constructor. From that point on, the 
    standard streambuf pointers pput, pget, pstart, pend all point to the 
    appropriate locations within the allocated buffer. The size of the buffer
    is the first thing written into the buffer and will always occupy the 
    first 4 bytes (sizeof (std::streamsize)) of the buffer.
*/
template <class _Ch, class _Tr = std::char_traits<_Ch> >
#ifdef _MSC_VER
class   basic_binstreambuf : virtual public std::basic_streambuf<_Ch, _Tr> {
#else 
class   basic_binstreambuf : virtual public streambuf {
#endif
public:
    basic_binstreambuf (std::streamsize sz) : _outbuf(new _Ch[sz + sizeof(std::streamsize)/sizeof(_Ch)]), _inbuf(0) {
        memcpy(_outbuf, &sz, sizeof(std::streamsize));
        setp (_outbuf + sizeof(std::streamsize), _outbuf + sz*sizeof(_Ch) + sizeof(unsigned int));
    }
    basic_binstreambuf (const _Ch* p) : _outbuf(0), _inbuf(0) {
        std::streamsize    sz = *((const std::streamsize*)p);
        _inbuf = new _Ch[sz];
        memcpy (_inbuf, p + sizeof(std::streamsize), sz);
        setg (_inbuf, _inbuf, _inbuf + sz * sizeof(_Ch));
    }
    virtual ~basic_binstreambuf () { delete _outbuf; delete _inbuf; }

    // givebuf - hand the output buffer over to someone who can use it...
    _Ch*    givebuf () {
        _Ch*    p = _outbuf;
        _outbuf = 0;
        return p;
    }

    // return the total size of the output buffer, in terms of the standard allocation
    // unit
    size_t  outbufsize () const {
        return (_outbuf) ? (sizeof(_Ch) * (*(std::streamsize*)_outbuf) + sizeof(std::streamsize)) : 0;
    }
    
    // static method to compute the size of the buffer needed to store an object of
    // size s.
    static size_t  computesize (size_t s) {
        return s + sizeof(std::streamsize);
    }

private:
    _Ch*  _outbuf;
    _Ch*  _inbuf; 
};

/* basic_binostream - class definition
    The basic_binostream class creates and manages an instance of basic_binstreambuf
    which it uses as a storage buffer for the data which is written to the stream.
    The only constructor which is available specifies the size of the data to come. Thus
    basic_binostreams can only handle data of a pre-determined size! Any data written to
    a basic_binostream must be extracted by using basic_binistream!
*/
template <class _Ch, class _Tr = std::char_traits<_Ch> >
#ifdef _MSC_VER
class   basic_binostream : public std::basic_ostream<_Ch, _Tr>  {
#else
class   basic_binostream : public std::basic_ostream {
#endif
public:
    basic_binostream (std::streamsize sz) 
#ifdef _MSC_VER
        : std::basic_ostream<_Ch,_Tr>(_buf = new basic_binstreambuf<_Ch,_Tr>(sz)) {}
#else
    : ostream (_buf = new basic_binstreambuf<_Ch,_Tr>(sz)) {}
#endif
    virtual ~basic_binostream () { delete _buf; }

    // popbuf - returns the stored data as a void* pointer and removes it from the
    //          streambuffer object (the streambuffer is destroyed!)
    void*   popbuf (size_t& sz) {
        sz = _buf->outbufsize();
        return _buf->givebuf ();
    }

    // computesize - compute the size of the buffer needed to store an object of
    //               size s.
    static size_t   computesize (size_t s) {
        return basic_binstreambuf<_Ch,_Tr>::computesize(s);
    }

private:
    basic_binstreambuf<_Ch,_Tr>* _buf;
};

/* basic_binostream - class definition
    The basic_binistream class extracts data from a buffer created by basic_binostream.
    Note that the buffer passed to the constructor must have been created by an
    instance of basic_binostream, otherwise the results will be unreliable, if not
    fatal.
*/
template <class _Ch, class _Tr = std::char_traits<_Ch> >
#ifdef _MSC_VER
class   basic_binistream : public std::basic_istream<_Ch, _Tr> {
#else
class   basic_binistream : public istream {
#endif
public:
    basic_binistream (const _Ch* ptr)
#ifdef _MSC_VER
        : std::basic_istream<_Ch,_Tr>(_buf = new basic_binstreambuf<_Ch,_Tr>(ptr)) {}
#else
    : basic_istream (_buf = new basic_binstreambuf<_Ch,_Tr>(ptr)) {}
#endif
    virtual ~basic_binistream () { delete _buf; }

private:
    basic_binstreambuf<_Ch,_Tr>* _buf;
};

template <class _Ty, class _Ch, class _Tr> 
inline basic_binostream<_Ch, _Tr>&  operator << ( basic_binostream<_Ch,_Tr>& o, const _Ty  t ) {
    int     sz = sizeof(_Ty);
    o.write ((const _Ch*)(&t), sz);
    return o;
}

template <class _Ty, class _Ch, class _Tr>
inline basic_binistream<_Ch,_Tr>&  operator >> ( basic_binistream<_Ch,_Tr>& i, _Ty& t ) {
    int     sz = sizeof(_Ty);
    i.read ((_Ch*)(&t),sz);
    return i;
}

/*  The classes binostream, binistream, and binstreambuf define binary stream 
    implementations based upon the char datatype (using byte resolution for allocation). 
*/ 
typedef basic_binostream<char>  binostream;
typedef basic_binistream<char>  binistream;
typedef basic_binstreambuf<char> binstreambuf;

#endif