#pragma once

#include <vector>
#include <set>
#include <string>

#include "FastDelegate.h"
using namespace fastdelegate;

#include "ProviderInterface.h"

#include "PositionOptionDeltasRow.h"

/* 
  In the future if this is generic enough or code can be factored out, 
  might try model<row structure class> for enabling reuse of mapping 
  row data to column in CListCtrl

  controller<model<row structure class> > might also be doable
  */

// Use CustomDraw to do each row field so custom effects are achieveable

class CPositionOptionDeltasModel {
public:
  typedef CPositionOptionDeltasRow::vDeltaRows_t vDeltaRows_t;

  CPositionOptionDeltasModel(void);
  ~CPositionOptionDeltasModel(void);
  void Add( CProviderInterface *pDataProvider, CInstrument::pInstrument_t pInstrument );  
  
  typedef FastDelegate1<vDeltaRows_t::size_type> OnInstrumentCountChangedHandler;
  void SetOnInstrumentCountChanged( OnInstrumentCountChangedHandler function ) {
    OnInstrumentCountChanged = function;
  }

  // queue rows to be updated, PositionOptionDeltasRow uses this
  typedef FastDelegate<void(vDeltaRows_t::size_type)> OnQueueUpdatedRowIndexHandler;
  void SetOnQueueUpdatedRowIndex( OnQueueUpdatedRowIndexHandler function ) {
    OnQueueUpdatedRowIndex = function;
  }

  // update the changed row in GUI
  typedef FastDelegate1<vDeltaRows_t::size_type> OnRowUpdatedHandler;
  void SetOnRowUpdated( OnRowUpdatedHandler function ) {
    OnRowUpdated = function;
  }

  const std::string& GetItem( vDeltaRows_t::size_type ixRow, int ixItem );

  void CheckForUpdatedRows( void );

protected:
  void QueueUpdatedRowIndex( vDeltaRows_t::size_type ix );

private:
  vDeltaRows_t m_vDeltaRows; // data to be viewed
  OnInstrumentCountChangedHandler OnInstrumentCountChanged;  // create blank visual row to be updated
  OnRowUpdatedHandler OnRowUpdated;
  std::set<vDeltaRows_t::size_type> m_setUpdatedRows;  // a set of updated rows
  OnQueueUpdatedRowIndexHandler OnQueueUpdatedRowIndex;
};
