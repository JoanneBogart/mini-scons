// -*- C++ -*-
//
// error.h,v 1.1.1.1 1994/04/18 18:12:32 burnett Exp
//
// This file is a part of the MC++ Event Generator Toolkit.
// Copyright (C) 1992 Leif L"onnblad, Anders Nilsson, Mike Seymour.
//
// This file declares some error recovery functions.
//

#ifndef _ERROR_H_
#define _ERROR_H_


#include <strstream>

extern void WARNING(const char *);
extern void FATAL(const char *);

#define FATAL_MACRO(output)\
do{std::ostrstream message; \
   message <<__FILE__<<":"<<__LINE__<<": "<<output<<'\0';\
::FATAL(message.str()); }while(0)

#define WARNING_MACRO(output)\
do{std::ostrstream message; \
   message <<__FILE__<<":"<<__LINE__<<": "<<output<<'\0';\
::WARNING(message.str()); message.freeze(false); }while(0)


#endif // _ERROR_H_

