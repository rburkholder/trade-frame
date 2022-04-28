/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

namespace ou {

// http://en.wikipedia.org/wiki/Color#cite_note-0

#ifdef RGB
#undef RGB
#endif

using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned long;
using COLORREF = DWORD;

// ====

// TODO: create namespace for windows, if required

//#ifndef WINRGB
  // windows COLORREF is backwards from what ChartDir is expecting
  // ChartDir:
//#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
//#else
  // from WinGDI.h:
//#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
//#endif

// ====

// a namespace for regular use, I believe this is compatible with ChartDir
namespace Colour {

  #define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
  enum EColour {
    #include "Colour_base.h"
  };

// ====

// a namespace for wxWidgets compatible colours
namespace wx {
#ifdef RGB
#undef RGB
#endif

  #define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

  enum EColour {
    #include "Colour_base.h"
  };

} // namespace wx
} // namespace Colour
} // namespace ou