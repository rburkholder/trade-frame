#pragma once

#include <vector>
#include <set>

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
  
  typedef FastDelegate1<vDeltaRows_t::size_type> OnInstrumentAddedHandler;
  void SetOnInstrumentAdded( OnInstrumentAddedHandler function ) {
    OnInstrumentAdded = function;
  }

  // queue rows to be updated 
  typedef FastDelegate<void(vDeltaRows_t::size_type)> OnQueueUpdatedRowIndexHandler;
  void SetOnQueueUpdatedRowIndex( OnQueueUpdatedRowIndexHandler function ) {
    OnQueueUpdatedRowIndex = function;
  }

protected:
  void QueueUpdatedRowIndex( vDeltaRows_t::size_type ix );

private:
  vDeltaRows_t m_vDeltaRows; // data to be viewed
  OnInstrumentAddedHandler OnInstrumentAdded;  // create blank visual row to be updated
  std::set<vDeltaRows_t::size_type> m_setUpdatedRows;  // a set of updated rows
  OnQueueUpdatedRowIndexHandler OnQueueUpdatedRowIndex;
};
