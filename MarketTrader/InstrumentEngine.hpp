/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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

/*
 * File:    InstrumentEngine.hpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/24 16:58:15
 */

#pragma once

#include <TFTrading/ComposeInstrument.hpp>

#include <TFIQFeed/Provider.h>
#include <TFInteractiveBrokers/IBTWS.h>

namespace ou {
namespace tf {
namespace engine {

class Instrument {
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using fInstrument_t = ou::tf::ComposeInstrument::fInstrument_t;

  Instrument( pProvider_t pExec, pProvider_t pData );
  ~Instrument();

  void Compose( const std::string&, fInstrument_t&& );

protected:
private:

  using pProviderIBTWS_t = ou::tf::ib::TWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;

  pProvider_t m_pExec;
  pProvider_t m_pData;

  pProviderIBTWS_t m_pIB;
  pProviderIQFeed_t m_pIQ;

  std::unique_ptr<ou::tf::ComposeInstrument> m_pComposeInstrument;

};

} // namespace engine
} // namespace tf
} // namespace ou