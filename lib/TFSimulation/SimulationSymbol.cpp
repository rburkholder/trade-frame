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

#include <TFTrading/MacroStrand.h>

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include "SimulationSymbol.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

// sDirectory needs to be available on instantiation to enable signal availability
SimulationSymbol::SimulationSymbol(
  const std::string& sSymbol
, pInstrument_cref pInstrument
, const std::string& sGroup
, const std::string& sFileName
)
: Symbol<SimulationSymbol>( pInstrument )
, m_sDirectory( sGroup )
, m_sFileName( sFileName )
{}

SimulationSymbol::~SimulationSymbol() {
}

void SimulationSymbol::StartTradeWatch() {
  if ( 0 == m_trades.Size() ) {
    try {
      std::string sPath( m_sDirectory + Trades::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO, m_sFileName );
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

void SimulationSymbol::StopTradeWatch() {
}

void SimulationSymbol::StartQuoteWatch() {
  if ( 0 == m_quotes.Size() ) {
    try {
      std::string sPath( m_sDirectory + Quotes::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO, m_sFileName );
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

void SimulationSymbol::StopQuoteWatch() {
}

void SimulationSymbol::StartGreekWatch() {
  if ( ( 0 == m_greeks.Size() ) && ( m_pInstrument->IsOption() ) )  {
    try {
      std::string sPath( m_sDirectory + Greeks::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO, m_sFileName );
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

void SimulationSymbol::StopGreekWatch() {
}

void SimulationSymbol::StartDepthByMMWatch() {
  if ( 0 == m_depths_mm.Size() )  {
    try {
      std::string sPath( m_sDirectory + DepthsByMM::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO, m_sFileName );
      HDF5TimeSeriesContainer<DepthByMM> depthRepository( dm, sPath );
      HDF5TimeSeriesContainer<DepthByMM>::iterator begin, end;
      begin = depthRepository.begin();
      end = depthRepository.end();
      m_depths_mm.Resize( end - begin );
      depthRepository.Read( begin, end, &m_depths_mm );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopDepthByMMWatch() {
}

void SimulationSymbol::StartDepthByOrderWatch() {
  if ( 0 == m_depths_order.Size() )  {
    try {
      std::string sPath( m_sDirectory + DepthsByOrder::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO, m_sFileName );
      HDF5TimeSeriesContainer<DepthByOrder> depthRepository( dm, sPath );
      HDF5TimeSeriesContainer<DepthByOrder>::iterator begin, end;
      begin = depthRepository.begin();
      end = depthRepository.end();
      m_depths_order.Resize( end - begin );
      depthRepository.Read( begin, end, &m_depths_order );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopDepthByOrderWatch() {
}

void SimulationSymbol::HandleQuoteEvent( const DatedDatum &datum ) {
  const Quote& quote( dynamic_cast<const Quote &>( datum ) );
  STRAND_CAPTURE( (m_OnQuote( quote )), quote )
}

void SimulationSymbol::HandleTradeEvent( const DatedDatum &datum ) {
  const Trade& trade( dynamic_cast<const Trade &>( datum ) );
  STRAND_CAPTURE( (m_OnTrade( trade )), trade )
}

void SimulationSymbol::HandleGreekEvent( const DatedDatum &datum ) {
  const Greek& greek( dynamic_cast<const Greek &>( datum ) );
  STRAND_CAPTURE( (m_OnGreek( greek )), greek )
}

void SimulationSymbol::HandleDepthByMMEvent( const DatedDatum &datum ) {
  const DepthByMM& md( dynamic_cast<const DepthByMM &>( datum ) );
  STRAND_CAPTURE( (m_OnDepthByMM( md )), md )
}

void SimulationSymbol::HandleDepthByOrderEvent( const DatedDatum &datum ) {
  const DepthByOrder& md( dynamic_cast<const DepthByOrder &>( datum ) );
  STRAND_CAPTURE( (m_OnDepthByOrder( md )), md )
}


} // namespace tf
} // namespace ou
