/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Process.cpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 1, 2022  21:48
 */

#include <memory>

#include <TFTrading/AcquireFundamentals.h>

#include "Process.hpp"
#include "TFTrading/Watch.h"

Process::Process( vSymbols_t& vSymbols )
: m_vSymbols( vSymbols ), m_bDone( false )
{

  m_piqfeed = ou::tf::iqfeed::IQFeedProvider::Factory();

  m_piqfeed->OnConnected.Add( MakeDelegate( this, &Process::HandleConnected ) );
  m_piqfeed->Connect();

}

void Process::HandleConnected( int ) {

  m_iterSymbols = m_vSymbols.begin();
  Lookup();
}

void Process::Lookup() {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using Fundamentals = ou::tf::Watch::Fundamentals;

  if ( m_vSymbols.end() == m_iterSymbols ) {
    m_bDone = true;
    m_cvWait.notify_one();
  }
  else {

    std::cout << "lookup " << m_iterSymbols->sSymbol << std::endl;

    pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( m_iterSymbols->sSymbol );
    pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, m_iterSymbols->sSymbol );
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );

    m_pAcquireFundamentals_live
      = ou::tf::AcquireFundamentals::Factory (
        std::move( pWatch ),
        [this]( pWatch_t pWatch ){
          const Fundamentals& fundamentals( pWatch->GetFundamentals() );
          dividend_t& dividend( *m_iterSymbols );
          dividend.sExchange = fundamentals.sExchange;
          dividend.rate = fundamentals.dblDividendRate;
          dividend.yield = fundamentals.dblDividendYield;
          dividend.amount = fundamentals.dblDividendAmount;
          dividend.nAverageVolume = fundamentals.nAverageVolume;
          dividend.dateExDividend = fundamentals.dateExDividend;
          if ( 10.0 < fundamentals.dblDividendYield ) {
            std::cout
              << fundamentals.sExchange
              << " " << fundamentals.sSymbolName
              << " div.yield=" << fundamentals.dblDividendYield
              << std::endl;
          }
          m_iterSymbols++;
          m_pAcquireFundamentals_dead = std::move( m_pAcquireFundamentals_live );
          Lookup();
        }
        );
    m_pAcquireFundamentals_live->Start();
  }
}

void Process::Wait() {
  std::unique_lock<std::mutex> lock( m_mutexWait );
  m_cvWait.wait( lock, [this]{ return m_bDone; } );
}

