#pragma once

#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

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
  CPositionOptionDeltasRow( CInstrument::pInstrument_t pInstrument );
  ~CPositionOptionDeltasRow(void);
protected:

  // define variables to be viewed in the row
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EMIT_DefineVars, ~ )

  CInstrument::pInstrument_t m_pInstrument;  // smart pointer provides basic info for the model

private:
};
