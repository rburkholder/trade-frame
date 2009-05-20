#pragma once

  // column header
  // column alignment
  // column size
  // primitive visibleitem primitive type
  // visibleitem format string
  // wincontrol visibleitem win control type
  // IxName type used for acquiring constant index into columns

  // macro or template:  creates a structure
  // an array of structures, one structure per column

#include <boost/utility.hpp>
#include "VisibleItem.h"



// //#define MAKECOLUMN(IxName, Primitive_t, WinControl_t, WinControl, ColName, ColAlign, ColWidth, ColFormat, Blank) 
/*
#define BEGIN_MAKECOLUMN
#define MAKECOLUMN(IxName, Primitive_t, WinControl_t, WinControl, ColName, ColAlign, ColWidth, ColFormat, Blank) \
  CVisibleItem<Primitive_t, WinControl_t>( 
    WinControl, new CVisibleItemAttributes( 
#define END_MAKECOLUMN
*/
/*
header needs enumerations for each column
the column instantiation is going to use the enumeration plus an incrementer and 
  needs to match the incrementor with the enumeration to ensure they match
column factory takes a static function pointer for contructing the column

inherit? form row factory to get macro and anything other stuff, or just a header file to get the macro?
*/