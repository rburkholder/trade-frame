/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "SimulationSymbol.h"

#include "TFHDF5TimeSeries/HDF5TimeSeriesContainer.h"
#include "TFHDF5TimeSeries/HDF5IterateGroups.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// sDirectory needs to be available on instantiation to enable signal availability
SimulationSymbol::SimulationSymbol( 
  const std::string &sSymbol, 
  pInstrument_cref pInstrument, 
  const std::string &sGroup
  ) 
: Symbol<SimulationSymbol>(pInstrument), m_sDirectory( sGroup )
{
  // this is dealt with in the SimulationProvider, but we don't have a .Remove
  //m_OnTrade.Add( MakeDelegate( &m_simExec, &CSimulateOrderExecution::NewTrade ) );
}

SimulationSymbol::~SimulationSymbol(void) {
  // we don't yet have a .Remove for this in SimulationProvider yet.
  //m_OnTrade.Remove( MakeDelegate( &m_simExec, &CSimulateOrderExecution::NewTrade ) );

}

void SimulationSymbol::StartTradeWatch( void ) {
  if ( 0 == m_trades.Size() ) {
    try {
      std::string sPath( m_sDirectory + "/trades/" + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
      HDF5TimeSeriesContainer<Trade> tradeRepository( dm, sPath );
      HDF5TimeSeriesContainer<Trade>::iterator begin, end;
      begin = tradeRepository.begin();
      end = tradeRepository.end();
      m_trades.Resize( end - begin );
      tradeRepository.Read( begin, end, &m_trades );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopTradeWatch( void ) {
}

void SimulationSymbol::StartQuoteWatch( void ) {
  if ( 0 == m_quotes.Size() ) {
    try {
      std::string sPath( m_sDirectory + "/quotes/" + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
      HDF5TimeSeriesContainer<Quote> quoteRepository( dm, sPath );
      HDF5TimeSeriesContainer<Quote>::iterator begin, end;
      begin = quoteRepository.begin();
      end = quoteRepository.end();
      m_quotes.Resize( end - begin );
      quoteRepository.Read( begin, end, &m_quotes );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopQuoteWatch( void ) {
}

void SimulationSymbol::StartGreekWatch( void ) {
  if ( ( 0 == m_greeks.Size() ) && ( m_pInstrument->IsOption() ) )  {
    try {
      std::string sPath( m_sDirectory + "/greeks/" + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
      HDF5TimeSeriesContainer<Greek> greekRepository( dm, sPath );
      HDF5TimeSeriesContainer<Greek>::iterator begin, end;
      begin = greekRepository.begin();
      end = greekRepository.end();
      m_greeks.Resize( end - begin );
      greekRepository.Read( begin, end, &m_greeks );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopGreekWatch( void ) {
}

void SimulationSymbol::StartDepthWatch( void ) {
}

void SimulationSymbol::StopDepthWatch( void ) {
}

void SimulationSymbol::HandleQuoteEvent( const DatedDatum &datum ) {
  m_OnQuote( dynamic_cast<const Quote &>( datum ) ); 
}

void SimulationSymbol::HandleTradeEvent( const DatedDatum &datum ) {
  m_OnTrade( dynamic_cast<const Trade &>( datum ) );  
}

void SimulationSymbol::HandleGreekEvent( const DatedDatum &datum ) {
  m_OnGreek( dynamic_cast<const Greek &>( datum ) );  
}

} // namespace tf
} // namespace ou
