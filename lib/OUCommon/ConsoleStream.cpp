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

#include "stdafx.h"
#include "ConsoleStream.h"
#include <stdexcept>

namespace ou {

CConsoleStream::CConsoleStream(void) {
  // http://www.cplusplus.com/reference/iostream/streambuf/setp.html
  // http://blogs.awesomeplay.com/elanthis/archives/2007/12/10/444/
  setp( buf, buf + BufSize );
}

CConsoleStream::~CConsoleStream(void) {
}

//streamsize CConsoleStream::xsputn (const char_type* s, streamsize n) {
//  if ( NULL != OnNewString ) OnNewString( s, n );
//  return n;
//}

int CConsoleStream::sync() {
  if ( NULL != OnNewString ) OnNewString( pbase(), (int) ( pptr() - pbase() - 1 ) ); // assumes CR at end
  if ( NULL != OnFlushString ) OnFlushString();
  setp( pbase(), epptr() );
//  if ( NULL != OnFlushString ) OnFlushString();
  return 0;
}

int CConsoleStream::overflow(int_type meta) {
  throw std::runtime_error( "ConsoleStream overflow" );
}

} // ou