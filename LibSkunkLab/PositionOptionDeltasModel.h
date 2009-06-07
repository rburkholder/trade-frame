#pragma once

#include <vector>

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
  CPositionOptionDeltasModel(void);
  ~CPositionOptionDeltasModel(void);
  void Add( CInstrument::pInstrument_t pInstrument );  
protected:
private:
  std::vector<CPositionOptionDeltasRow*> m_vDeltaRows; // data to be viewed
};
