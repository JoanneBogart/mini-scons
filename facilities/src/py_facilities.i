%module py_facilities
%{
#include <cstddef>
#include <string>
#include "facilities/commonUtilities.h"
%}
%include stl.i
%include cpointer.i
%include ../facilities/commonUtilities.h
