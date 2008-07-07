#include "StdAfx.h"
#include "IQFeedOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// TODO:  convert OnNewResponse over to IQFeedRetrial

CIQFeedOptions::CIQFeedOptions(CIQFeedProvider *pProvider, const char *szSymbol) 
: CIQFeedRetrieval( pProvider ) 
{
  OpenPort();
  string s;
  s = "OEA,";
  s += szSymbol;
  s += ";";
  m_bLookingForDetail = true;
  m_pPort->SendToSocket( s.c_str() );
}

CIQFeedOptions::~CIQFeedOptions(void) {

  typedef string* pString;

  while ( !m_vOptionSymbols.empty() ) {
    pString &s = m_vOptionSymbols.back();
    delete s;
    m_vOptionSymbols.pop_back();
  }
}

void CIQFeedOptions::AddOptionSymbol( const char *s, unsigned short cnt ) {
  string *_s = new string( s, cnt );
  m_vOptionSymbols.push_back( _s );
}

void CIQFeedOptions::OnNewResponse( const char *szLine ) {
  if ( !m_bLookingForDetail ) {
    if ( 0 == strcmp( szLine, "!ENDMSG!" ) ) {
      ClosePort();
    }
  }
  else {
    char *szSubStr = (char*) szLine;
    char *ixLine = (char*) szLine;
    unsigned short cnt = 0;

    while ( 0 != *ixLine ) {
      if ( ':' == *ixLine ) {
        if ( 0 != cnt ) {
          AddOptionSymbol( szSubStr, cnt );
          cnt = 0;
        }
        // switch from calls to puts
      }
      else {
        if ( ',' == *ixLine ) {
          if ( 0 != cnt ) {
            AddOptionSymbol( szSubStr, cnt );
            cnt = 0;
          }
        }
        else {
          // add to outstanding string
          if ( 0 == cnt ) {
            szSubStr = ixLine;
          }
          cnt++;
        }
      }
      ixLine++;
    }
    if ( 0 != cnt ) {
      if ( ' ' != *szSubStr ) {
        AddOptionSymbol( szSubStr, cnt );
        cnt = 0;
      }
    }
    if ( NULL != OnSymbolListReceived ) OnSymbolListReceived();
    m_bLookingForDetail = false;
  }
}
