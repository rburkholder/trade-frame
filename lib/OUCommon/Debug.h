/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <sstream>

// http://stackoverflow.com/questions/1149620/vc-how-to-write-to-the-output-window

// DEBUGOUT( "The value of x is " << x );

#ifdef UNICODE
#define DEBUGOUT( s )            \
{                             \
   std::wostringstream oss_;    \
   oss_ << s;                   \
/*   OutputDebugStringW( oss_.str().c_str() );*/  \
}
#else
#define DEBUGOUT( s )            \
{                             \
   std::ostringstream oss_;    \
   oss_ << s;                   \
/*   OutputDebugStringA( oss_.str().c_str() ); */ \
}
#endif // !UNICODE

// You can extend this using the __LINE__ and __FILE__ macros to give even more information

