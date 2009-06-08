#pragma once

#include <string>
#include <vector>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "FastDelegate.h"
using namespace fastdelegate;

#include "ProviderInterface.h"

#include "PositionOptionDeltasMacros.h"
#include "Instrument.h"

// events to update the visible row values
// event to cause model to update view
// use mixin?

// custom list view:
//  each field drawn uses own graphics buffer then copies buffer over to visible column

// need a row factory?  has the variable

class CPositionOptionDeltasRow {
  friend class CPositionOptionDeltasModel;
public:
  typedef std::vector<CPositionOptionDeltasRow*> vDeltaRows_t;

  CPositionOptionDeltasRow( 
    vDeltaRows_t::size_type ixRow,
    CProviderInterface *pDataProvider, CInstrument::pInstrument_t pInstrument 
    );
  ~CPositionOptionDeltasRow(void);
protected:

  typedef FastDelegate1<vDeltaRows_t::size_type> OnRowUpdatedHandler;
  void SetOnRowUpdated( OnRowUpdatedHandler function ) {
    OnRowUpdated = function;
  }

  // define variables to be viewed in the row
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EMIT_DefineVars, ~ )

  CInstrument::pInstrument_t m_pInstrument;  // smart pointer provides basic info for the model
  CProviderInterface *m_pDataProvider; 
  vDeltaRows_t m_ixRow;  // supplied by the model, index into it's vector

  void HandleQuote( CSymbol::quote_t quote );
  void HandleTrade( CSymbol::trade_t trade );

private:
  OnRowUpdatedHandler OnRowUpdated;
};
