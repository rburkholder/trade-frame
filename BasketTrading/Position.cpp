/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include <TFTimeSeries/TimeSeries.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Position.h"

ManagePosition::ManagePosition( const std::string& sName, const ou::tf::Bar& bar ) 
  : ou::tf::DailyTradeTimeFrame<ManagePosition>(),
  m_sName( sName ),
  m_bToBeTraded( false ), m_barInfo( bar ), 
  m_dblFundsToTrade( 0 ), m_bfTrades( 60 ),
  m_bCountBars( false ), m_nRHBars( 0 ),
  m_bSetOpen( false ), m_dblOpen( 0.0 ),
  m_stateTrading( TSWaitForEntry ), m_nAttempts( 0 )
{
  m_bfTrades.SetOnBarComplete( MakeDelegate( this, &ManagePosition::HandleBar ) );
}

ManagePosition::~ManagePosition(void) {
}

void ManagePosition::Start( void ) {

  assert( 0.0 != m_dblFundsToTrade );
  m_nSharesToTrade = CalcShareCount( m_dblFundsToTrade );

  assert( 0 != m_pPosition.get() );

  std::cout << "Position: starting for " << m_nSharesToTrade << " " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;

  m_pPosition->GetDataProvider()->AddQuoteHandler( m_pPosition->GetInstrument(), MakeDelegate( this, &ManagePosition::HandleQuote ) );
  m_pPosition->GetDataProvider()->AddTradeHandler( m_pPosition->GetInstrument(), MakeDelegate( this, &ManagePosition::HandleTrade ) );

}

void ManagePosition::Stop( void ) {
  m_pPosition->GetDataProvider()->RemoveQuoteHandler( m_pPosition->GetInstrument(), MakeDelegate( this, &ManagePosition::HandleQuote ) );
  m_pPosition->GetDataProvider()->RemoveTradeHandler( m_pPosition->GetInstrument(), MakeDelegate( this, &ManagePosition::HandleTrade ) );
}

void ManagePosition::HandleQuote( const ou::tf::Quote& quote ) {
  if ( quote.IsValid() ) {
    m_quotes.Append( quote );
    TimeTick( quote );
  }
}

void ManagePosition::HandleBar( const ou::tf::Bar& bar ) {
  m_bars.Append( bar );
  m_nRHBars++;
  // *** step in to state to test last three bars to see if trade should be entered
  TimeTick( bar );
}

void ManagePosition::HandleTrade( const ou::tf::Trade& trade ) {
  if ( m_bSetOpen ) { 
    m_dblOpen = trade.Price();
    m_bSetOpen = false;
    std::cout << trade.DateTime() << ": Open " << m_pPosition->GetInstrument()->GetInstrumentName() << "@" << trade.Price() << std::endl;
  }
  m_trades.Append( trade );
  m_bfTrades.Add( trade );
}

void ManagePosition::SaveSeries( const std::string& sPrefix ) {

  std::string sPathName;

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );

  if ( 0 != m_quotes.Size() ) {
    sPathName = sPrefix + "/quotes/" + m_pPosition->GetInstrument()->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Quotes> wtsQuotes( dm, true, true );
    wtsQuotes.Write( sPathName, &m_quotes );
    ou::tf::HDF5Attributes attrQuotes( dm, sPathName, ou::tf::InstrumentType::Stock );
    attrQuotes.SetProviderType( m_pPosition->GetDataProvider()->ID() );
  }

  if ( 0 != m_trades.Size() ) {
    sPathName = sPrefix + "/trades/" + m_pPosition->GetInstrument()->GetInstrumentName();
    ou::tf::HDF5WriteTimeSeries<ou::tf::Trades> wtsTrades( dm, true, true );
    wtsTrades.Write( sPathName, &m_trades );
    ou::tf::HDF5Attributes attrTrades( dm, sPathName, ou::tf::InstrumentType::Stock );
    attrTrades.SetProviderType( m_pPosition->GetDataProvider()->ID() );
  }

  //SetSignificantDigits?
  //SetMultiplier?
}

ou::tf::DatedDatum::volume_t ManagePosition::CalcShareCount( double dblFunds ) {
  return ( static_cast<ou::tf::DatedDatum::volume_t>( dblFunds / m_barInfo.Close() ) / 100 ) * 100;  // round down to nearest 100
}

void ManagePosition::HandleBellHeard( void ) {
  m_bSetOpen = true;
  m_bCountBars = true;
}

void ManagePosition::HandleCancel( void ) {
  std::cout << "EOD Cancel " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  m_pPosition->CancelOrders();
}

void ManagePosition::HandleGoNeutral( void ) {
  std::cout << "EOD Close " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  m_pPosition->ClosePosition();
}

void ManagePosition::HandleRHTrading( const ou::tf::Bar& bar ) {
  /*
  switch ( m_stateTrading ) {
  case TSWaitForEntry:
    if ( 5 <= m_nRHBars ) { // skip the two bars surrounding the open
      ou::tf::Bars::const_reference bar1( m_bars.Ago( 2 ) );
      ou::tf::Bars::const_reference bar2( m_bars.Ago( 1 ) );
      ou::tf::Bars::const_reference bar3( m_bars.Ago( 0 ) );
      if ( m_dblOpen < bar.Open() ) { // test for rising from open
        if ( 
//          ( bar1.Open() < bar2.Open() ) && 
//          ( bar2.Open() < bar3.Open() ) && 
          ( bar1.High() < bar2.High() ) && 
          ( bar2.High() < bar3.High() ) && 
          ( bar1.Low() < bar2.Low() ) && 
          ( bar2.Low() < bar3.Low() ) && 
          ( bar1.Close() < bar3.Close() ) 
          ) { 
            // open a long position
            m_nAttempts++;
            m_dblStop = bar1.Low();
            m_dblStopDifference = bar3.High() - bar1.Low();
            std::cout << bar.DateTime() << ": Long " << m_nSharesToTrade << " " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
            m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, m_nSharesToTrade );
            m_stateTrading = TSMonitorLong;
        }
      }
      else {
        if ( m_dblOpen > bar.Open() ) { // test for falling from open
          if ( 
//            ( bar1.Open() > bar2.Open() ) && 
//            ( bar2.Open() > bar3.Open() ) && 
            ( bar1.High() > bar2.High() ) && 
            ( bar2.High() > bar3.High() ) && 
            ( bar1.Low() > bar2.Low() ) && 
            ( bar2.Low() > bar3.Low() ) && 
            ( bar1.Close() > bar3.Close() ) 
            ) { 
              // open a short position
              m_nAttempts++;
              m_dblStop = bar1.High();
              m_dblStopDifference = bar1.High() - bar3.Low();
              std::cout << bar.DateTime() << ": Short " << m_nSharesToTrade << " " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
              m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, m_nSharesToTrade );
              m_stateTrading = TSMonitorShort;
          }
        }
      }
    }
    break;
  }
  */
}

// 2014/01/16 possible mechanism:  high volatility, low volatility, on rising enter, trade on ema slow/fast crossing

void ManagePosition::HandleRHTrading( const ou::tf::Quote& quote ) {
  // todo: calculate pivot. when pivot reached, put in stop.
  if ( !m_bSetOpen ) {  // wait for opening trade
    switch ( m_stateTrading ) {
      case TSWaitForEntry:
        if ( ( quote.Bid() > m_dblOpen ) && ( quote.Ask() > m_dblOpen ) ) {
          std::cout << quote.DateTime() << ": Long " << m_nSharesToTrade << " " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
          m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, m_nSharesToTrade );
          ++m_nAttempts;
          m_stateTrading = TSMonitorLong;
        }
        else {
          if ( ( quote.Bid() < m_dblOpen ) && ( quote.Ask() < m_dblOpen ) ) {
            std::cout << quote.DateTime() << ": Short " << m_nSharesToTrade << " " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
            m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, m_nSharesToTrade );
            ++m_nAttempts;
            m_stateTrading = TSMonitorShort;
          }
        }
        break;
      case TSMonitorLong:
          if ( ( quote.Bid() < m_dblOpen ) && ( quote.Ask() < m_dblOpen ) ) {
            if ( 10 < m_nAttempts ) {
              std::cout << quote.DateTime() << ": too many attempts for " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
              m_pPosition->CancelOrders();
              m_pPosition->ClosePosition();
              m_stateTrading = TSNoMore;
            }
            else {
              std::cout << quote.DateTime() << ": Rvs To Short " << m_nSharesToTrade << " " << m_pPosition->GetInstrument()->GetInstrumentName() << ", attempt " << m_nAttempts << std::endl;
              m_pPosition->CancelOrders();
              m_pPosition->ClosePosition();
              m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, m_nSharesToTrade );
              ++m_nAttempts;
              m_stateTrading = TSMonitorShort;
            }
          }
        break;
      case TSMonitorShort:
        if ( ( quote.Bid() > m_dblOpen ) && ( quote.Ask() > m_dblOpen ) ) {
            if ( 10 < m_nAttempts ) {
              std::cout << quote.DateTime() << ": too many attempts for " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
              m_pPosition->CancelOrders();
              m_pPosition->ClosePosition();
              m_stateTrading = TSNoMore;
            }
            else {
              std::cout << quote.DateTime() << ": Rvs To Long " << m_nSharesToTrade << " " << m_pPosition->GetInstrument()->GetInstrumentName() << ", attempt " << m_nAttempts << std::endl;
              m_pPosition->CancelOrders();
              m_pPosition->ClosePosition();
              m_pPosition->PlaceOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, m_nSharesToTrade );
              ++m_nAttempts;
              m_stateTrading = TSMonitorLong;
            }
        }
        break;
      case TSNoMore:
        break;
    }
  }
  /*
  // need parabolic stop, use trailing stop for now
  // also need over all risk management of 3% loss of total investment
  switch ( m_stateTrading ) {
  case TSMonitorLong:
    if ( quote.Ask() < m_dblStop ) {
      std::cout << quote.DateTime() << ": Long Stop (" << m_nAttempts << ") " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      m_pPosition->CancelOrders();
      m_pPosition->ClosePosition();
      m_nRHBars = 3;
      m_stateTrading = ( 3 > m_nAttempts ) ? TSWaitForEntry : TSNoMore;
    }
    else {
//      m_dblStop = std::max<double>( m_dblStop, quote.Ask() - m_dblStopDifference );
    }
    break;
  case TSMonitorShort:
    if ( quote.Bid() > m_dblStop ) {
      std::cout << quote.DateTime() << ": Short Stop (" << m_nAttempts << ") " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
      m_pPosition->CancelOrders();
      m_pPosition->ClosePosition();
      m_nRHBars = 3;
      m_stateTrading = ( 3 > m_nAttempts ) ? TSWaitForEntry : TSNoMore;
    }
    else {
//      m_dblStop = std::min<double>( m_dblStop, quote.Bid() + m_dblStopDifference );
    }
    break;
  }
  */
}
