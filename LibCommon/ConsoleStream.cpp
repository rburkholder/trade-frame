#include "StdAfx.h"
#include "ConsoleStream.h"
#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
