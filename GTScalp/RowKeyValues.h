#pragma once

#include "stdafx.h"
#include "DeltaMarker.h"
#include "IQFeedSymbol.h"

#include <vector>
#include <string>
using namespace std;

class CRowKeyValues {
public:

  CRowKeyValues( int ix, CListCtrl *pListCtrl, const string &name );
  virtual ~CRowKeyValues(void);

  void HandleSymbolFundamental( IQFeedSymbol * );
  void HandleSymbolSummary( IQFeedSymbol * );
  void HandleSymbolUpdate( IQFeedSymbol * );

  const char* GetFullName() { return m_sFullName.c_str(); };
  CDeltaMarkerBase *GetMarker( int ix ) {
    return m_vMarkers[ ix ];
  };

protected:

  string    m_sFullName; 
  CDMString *m_psName;
  CDMDouble *m_pdPrevClose;
  CDMDouble *m_pdNetChange;
  CDMInt    *m_piBidSize;
  CDMDouble *m_pdBid;
  CDMInt    *m_piTradeSize;
  CDMDouble *m_pdTrade;
  CDMInt    *m_piAskSize;
  CDMDouble *m_pdAsk;
  CDMDouble *m_pdHigh;
  CDMDouble *m_pdLow;
  CDMInt    *m_piVolume;

  int m_ix;  // row index of CListView to which this row is assigned
  typedef CDeltaMarkerBase *LPDELTAMARKERBASE;
  std::vector<LPDELTAMARKERBASE> m_vMarkers;  // vector is used so more elements can be added, then renumbered
  CListCtrl *m_pListCtrl;

  LVSETINFOTIP m_InfoTip;

private:
};
