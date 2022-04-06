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

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef DWORD COLORREF;

#ifdef RGB
#undef RGB
#endif

#ifndef WINRGB
// windows COLORREF is backwards from what ChartDir is expecting
// ChartDir:
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#else
// from WinGDI.h:
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

// we may get into problems if this file is used for windows colours as well as ChartDir colours
// or template it or type traits it or use some sort of define

// may need to create two color namespaces if used in the same include hiearchy

namespace Colour {
  enum EColour {
#include "Colour_base.h"
  };

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