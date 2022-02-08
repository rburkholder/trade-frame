#include "StdAfx.h"
#include "VuSymbolsOfInterest.h"

IMPLEMENT_DYNAMIC(CVuSymbolsOfInterest, CVuKeyValues)

CVuSymbolsOfInterest::CVuSymbolsOfInterest(CWnd* pParent /*=NULL*/) {

  static string sSymbols[] = {
    _T( "ICE" ),
    _T( "GOOG" ),
    _T( "CME" ),
    _T( "" )
  };

  BOOL b = true;

  int i = 0;
  CRowKeyValues *row;
  while ( 0 != sSymbols[i].length() ) {
    row = AppendSymbol( sSymbols[i] );
    WatchSymbol( sSymbols[i], row );
    i++;
  }

}

CVuSymbolsOfInterest::~CVuSymbolsOfInterest(void) {
}

void CVuSymbolsOfInterest::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVuSymbolsOfInterest, CVuKeyValues)

END_MESSAGE_MAP()


// CVuSymbolsOfInterest message handlers
