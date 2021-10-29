/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

//
// Ensure that the underlying contract has been built with IB first.
//

#include <string>
#include <sstream>
#include <stdexcept>

#include <boost/date_time/gregorian/gregorian.hpp>

#include <OUCommon/Delegate.h>

#include <TFTrading/DBOps.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option {  // Option

// handle more than one request simultaneously?  if not, then set flag to capture

// returns number of options retrieved
class PopulateOptions {
public:

  typedef ou::tf::ib::TWS::pInstrument_t pInstrument_t;
  typedef ou::tf::ib::TWS::pProvider_t pProvider_t;

  PopulateOptions( ou::tf::DBOps& session, pProvider_t pProvider );
  ~PopulateOptions( void );

  typedef FastDelegate1<pInstrument_t&> OnInstrumentBuilt_t;
  void SetOnInstrumentBuiltHandler( OnInstrumentBuilt_t function ) {
    OnInstrumentBuilt = function;
  }

  typedef FastDelegate1<unsigned int> OnPopulateCompleteHandler_t;
  void SetOnPopulateCompleteHandler( OnPopulateCompleteHandler_t function )  {
    OnPopulateComplete = function;
  }

  void Populate( const std::string& sUnderlying, boost::gregorian::date expiry, bool bCall, bool bPut );

protected:
private:

  Contract m_contract; // re-usable, persistant contract scratchpad
  ou::tf::DBOps& m_session;
  pProvider_t m_pProvider;

  bool m_bActive;

  unsigned int m_cntInstruments;

  OnPopulateCompleteHandler_t OnPopulateComplete;
  OnInstrumentBuilt_t OnInstrumentBuilt;

  void HandleOptionContractNotFound( void );
  void HandleOptionContractDetails( const ContractDetails& details, pInstrument_t& pInstrument );
  void HandleOptionContractDetailsDone( void );

};

} // namespace option
} // namespace tf
} // namespace ou

