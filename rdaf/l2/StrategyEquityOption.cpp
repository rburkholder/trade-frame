/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    StrategyEquityOption.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#include <TFIQFeed/OptionChainQuery.h>

#include <TFOptions/Engine.h>

#include <TFVuTrading/TreeItem.hpp>

#include "StrategyEquityOption.hpp"

namespace Strategy {

EquityOption::EquityOption(
  const ou::tf::config::symbol_t& config
, ou::tf::TreeItem* pTreeItem
)
: Base( config, pTreeItem )
{}

EquityOption::~EquityOption() {
  if ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnQuote.Remove( MakeDelegate( this, &EquityOption::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &EquityOption::HandleTrade ) );
  }

  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }
}

void EquityOption::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );

  ou::tf::Instrument::pInstrument_t pInstrument( pPosition->GetInstrument() );
  assert( ou::tf::InstrumentType::Stock == pInstrument->GetInstrumentType() );
  const std::string& sIQFeedUnderlying( pInstrument->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

  Base::SetPosition( pPosition );

  pWatch_t pWatch = m_pPosition->GetWatch();
  pWatch->OnQuote.Add( MakeDelegate( this, &EquityOption::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &EquityOption::HandleTrade ) );

  using query_t = ou::tf::iqfeed::OptionChainQuery;
  m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
    [this,&sIQFeedUnderlying](){
      m_pOptionChainQuery->QueryEquityOptionChain(
        sIQFeedUnderlying, "pc", "", "3", "", "", "",
        []( const query_t::OptionList& list ) {
          std::cout << "supplied list for " << list.sUnderlying << " has " << list.vSymbol.size() << " options" << std::endl;
        }
      );
    }
  );
  m_pOptionChainQuery->Connect();
}

void EquityOption::HandleQuote( const ou::tf::Quote& quote ) {
  ptime dt( quote.DateTime() );
  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );
}

void EquityOption::HandleTrade( const ou::tf::Trade& trade ) {
  ptime dt( trade.DateTime() );
  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );
}

void EquityOption::SaveWatch( const std::string& sPrefix ) {
  Base::SaveWatch( sPrefix );
}

} // namespace Strategy
