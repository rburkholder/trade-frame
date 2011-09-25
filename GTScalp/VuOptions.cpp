// VuIndicies.cpp : implementation file
//

#include "stdafx.h"
#include "VuOptions.h"

// CVuIndicies dialog

IMPLEMENT_DYNAMIC(CVuOptions, CVuKeyValues)

CVuOptions::CVuOptions(CWnd* pParent /*=NULL*/)
	: CVuKeyValues(pParent) {

    // will need to fix this.  need to set the event before initiating request
  options = new CIQFeedOptions( m_IQFeedProvider.GetIQFeedProvider(), "ICE" );
  options->SetOnSymbolListRecieved( MakeDelegate( this, &CVuOptions::MonitorSymbolList ) );

}

CVuOptions::~CVuOptions() {
  options->SetOnSymbolListRecieved( NULL );
  delete options;
}

void CVuOptions::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVuOptions, CVuKeyValues)
END_MESSAGE_MAP()

// CVuOptions message handlers

void CVuOptions::MonitorSymbolList() {
  CRowKeyValues *row;
  for each ( const string *s in options->m_vOptionSymbols ) {
    row = AppendSymbol( *s );
    WatchSymbol( *s, row );
  }
}

