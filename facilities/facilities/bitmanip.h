// $Id$
// bitmanip.h
// Author:  Sawyer Gillespie
//          University of Washington, Physics Department
//          March 1999
//          hgillesp@u.washington.edu
//  
//  Defines several inline routines to provide access to various
//  bit-patterns within various words. The functions byte_N return 
//  the Nth byte of a given word counting from low to high bits.

#ifndef _bitmanip_H__
#define _bitmanip_H__

#ifdef _MSC_VER
#pragma once
#endif

namespace bitmanip {

// template functions for getting/setting four bit words
// i is the word #
// v is the value
// s it the new value

template <class _T>
inline short int   word ( short i, const _T& v )
{
    short   shft = i*4;
    _T  mask = 0x000F << shft;
    return (v & mask) >> shft;
}

template <class _T>
void    set_word ( short i, _T& v, const _T& s )
{
    short   shft = i*4;
    _T  mask = 0x000F << shft;
    v = ((v & ~mask) | ((s << shft) & mask));
}

} // namespace bitmanip

#endif
