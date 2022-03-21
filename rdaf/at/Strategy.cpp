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
 * File:    Strategy.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/at
 * Created: March 7, 2022 14:35
 */

 #include <chrono>

#include <rdaf/TH3.h>
#include <rdaf/TTree.h>
//#include <rdaf/TFitResult.h>

#include <OUCharting/ChartDataView.h>

#include <TFTrading/Watch.h>

#include "ConfigParser.hpp"

#include "Strategy.h"

using pWatch_t = ou::tf::Watch::pWatch_t;

Strategy::Strategy(
  const config_t config
)
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_stateTrade( ETradeState::Init )
, m_config( config )
, m_ceLongEntry( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceLongFill( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceLongExit( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
, m_ceShortEntry( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceShortFill( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceShortExit( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_bfQuotes01Sec( 1 )
{

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

  m_ceProfitLoss.SetName( "P/L" );

  m_ceExecutionTime.SetName( "Execution Time" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

}

Strategy::~Strategy() {
  //if ( m_pFile ) { // commented out to maintain consistency with hd5f manual retention in SaveWatch
  //  m_pFile->Write();
  //}
  //if ( m_threadRdaf.joinable() ) {
  //  m_prdafApp->SetReturnFromRun( true );
  //  m_threadRdaf.join(); // returns after .quit at command line
  //}
  Clear();
}

void Strategy::SetupChart() {

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::PL, &m_ceProfitLoss );

  m_cdv.Add( EChartSlot::ET, &m_ceExecutionTime );

}

void Strategy::SetPosition( pPosition_t pPosition ) {

  assert( pPosition );

  Clear();

  m_pPosition = pPosition;
  pWatch_t pWatch = m_pPosition->GetWatch();

  m_cdv.SetNames( "AutoTrade", pWatch->GetInstrument()->GetInstrumentName() );

  SetupChart();

  InitRdaf();

  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::Clear() {
  if  ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
    pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
    m_cdv.Clear();
    //m_pPosition.reset(); // need to fix relative to thread
  }
}

void Strategy::InitRdaf() {

  pWatch_t pWatch = m_pPosition->GetWatch();
  const std::string& sSymbol( pWatch->GetInstrumentName() );

  m_pTreeQuote = std::make_shared<TTree>(
    ( sSymbol + "-quotes" ).c_str(), ( sSymbol + " quotes" ).c_str()
  );
  m_pTreeQuote->Branch( "quote", &m_branchQuote, "time/D:ask/D:askvol/l:bid/D:bidvol/l" );
  if ( !m_pTreeQuote ) {
    std::cout << "problems m_pTreeQuote" << std::endl;
  }

  m_pTreeTrade = std::make_shared<TTree>(
    ( sSymbol + "-trades" ).c_str(), ( sSymbol + " trades" ).c_str()
  );
  m_pTreeTrade->Branch( "trade", &m_branchTrade, "time/D:price/D:vol/l:direction/L" );
  if ( !m_pTreeTrade ) {
    std::cout << "problems m_pTreeTrade" << std::endl;
  }

  m_pHistVolume = std::make_shared<TH3D>(
    ( sSymbol + "-h1" ).c_str(), ( sSymbol + " Volume Histogram" ).c_str(),
    m_config.nTimeBins, m_config.dblTimeLower, m_config.dblTimeUpper,
    m_config.nPriceBins, m_config.dblPriceLower, m_config.dblPriceUpper,
    m_config.nVolumeBins, m_config.nVolumeLower, m_config.nVolumeUpper
  );
  if ( !m_pHistVolume ) {
    std::cout << "problems history" << std::endl;
  }

}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  // position has the quotes via the embedded watch
  // indicators are also attached to the embedded watch

  if ( !quote.IsValid() ) {
    return;
  }

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  m_quote = quote;

  if ( m_pTreeQuote ) { // wait for initialization in thread to start
    std::time_t nTime = boost::posix_time::to_time_t( quote.DateTime() );

    m_branchQuote.time = (double)nTime / 1000.0;
    m_branchQuote.ask = quote.Ask();
    m_branchQuote.askvol = quote.AskSize();
    m_branchQuote.bid = quote.Bid();
    m_branchQuote.bidvol = quote.BidSize();

    m_pTreeQuote->Fill();
}

  m_bfQuotes01Sec.Add( dt, m_quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the alogorithm

}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );

  m_ceTrade.Append( dt, trade.Price() );
  m_ceVolume.Append( dt, trade.Volume() );

  const double mid = m_quote.Midpoint();
  const double price = trade.Price();
  const uint64_t volume = trade.Volume();

  if ( m_pTreeTrade ) { // wait for initialization in thread to start
    std::time_t nTime = boost::posix_time::to_time_t( trade.DateTime() );
    m_branchTrade.time = (double)nTime / 1000.0;
    m_branchTrade.price = price;
    m_branchTrade.vol = volume;
    if ( mid == price ) {
      m_branchTrade.direction = 0;
    }
    else {
      m_branchTrade.direction = ( mid < price ) ? volume : -volume;
    }

    m_pTreeTrade->Fill();
  }

  if ( m_pHistVolume ) {
    m_pHistVolume->Fill( m_branchTrade.time, trade.Price(), trade.Volume() );
  }

}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;

  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );

  TimeTick( bar );
}

void Strategy::EnterLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( bar.DateTime(), bar.Close(), "Long Submit" );
  m_stateTrade = ETradeState::LongSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::EnterShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( bar.DateTime(), bar.Close(), "Short Submit" );
  m_stateTrade = ETradeState::ShortSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::ExitLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( bar.DateTime(), bar.Close(), "Long Exit" );
  m_stateTrade = ETradeState::ExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::ExitShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( bar.DateTime(), bar.Close(), "Short Exit" );
  m_stateTrade = ETradeState::ExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  // DailyTradeTimeFrame: Trading during regular active equity market hours

  const std::chrono::time_point<std::chrono::system_clock> begin
    = std::chrono::system_clock::now();

  switch ( m_stateTrade ) {
    case ETradeState::Search:
      if ( m_pHistVolume ) {
        //input of this function is the time, which is the current time
        //therefore we need to find a suitable range of bins along Y-axis (remember time is stored in y-axis)
        // and find the projection of h2. the result is a 1-D hist with x-axis being price
        // and value being volume

//        std::time_t nTime = boost::posix_time::to_time_t( bar.DateTime() );
//        nTime = (double)nTime / 1000.0;

        //find the bin that the time given belongs to:
//        Int_t bin_y = m_pHistVolume->GetYaxis()->FindBin( nTime );

        //if bin_y is valid and larger than 1 then proceed, else abort (since there is not enought data)
//        if ( bin_y < 1 ) {
//        }
//        else {
          //now find projection of h2 from the beginning till now:
//          auto h2_x = m_pHistVolume->ProjectionX( "_x", 1, bin_y );

          // now that h2_x is calculated, fit a gaussian to the it (i.e volume distribution)
//          auto b = h2_x->Fit( "gaus", "S" );

          //if fit is valid proceed, else abort
//          if ( !b->IsValid() ) {
//          }
//          else {
            //finally, if price is within 1 sigma from the mean, it's a good signal and go long:
//            if ( abs( bar.Close() - b->Parameter(1)) < b->Parameter(2) ) {
//              EnterLong( bar );
//            }
//          }
//        }
      }
      break;
    case ETradeState::LongSubmitted:
      // wait for order to execute
      break;
    case ETradeState::LongExit:
/*      if ( bar.Close() < ma2 ) {
        // exit long
        ExitLong( bar );
      }*/
      break;
    case ETradeState::ShortSubmitted:
      // wait for order to execute
      break;
    case ETradeState::ShortExit:
/*      if ( bar.Close() > ma2 ) {
        // exit short
        ExitShort( bar );
      } */
      break;
    case ETradeState::ExitSubmitted:
      // wait for order to execute
      break;
    case ETradeState::Done:
      // quiescent
      break;
    case ETradeState::Init:
      // market open statistics management here
      // will need to wait for ma to load & diverge (based upon width & period)
      m_stateTrade = ETradeState::Search;
      break;
  }

  const std::chrono::time_point<std::chrono::system_clock> end
    = std::chrono::system_clock::now();

   auto delta = std::chrono::duration_cast<std::chrono::microseconds>( end - begin).count();
   //auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end - begin).count();
   m_ceExecutionTime.Append( bar.DateTime(), delta );

}

void Strategy::HandleOrderCancelled( const ou::tf::Order& ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::ExitSubmitted:
      assert( false );  // TODO: need to figure out a plan to retry exit
      break;
    default:
      m_stateTrade = ETradeState::Search;
  }
  m_pOrder.reset();
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::LongSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), m_quote.Midpoint(), "Long Fill" );
      m_stateTrade = ETradeState::LongExit;
      break;
    case ETradeState::ShortSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), m_quote.Midpoint(), "Short Fill" );
      m_stateTrade = ETradeState::ShortExit;
      break;
    case ETradeState::ExitSubmitted:
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::Done:
      break;
    default:
      assert( false ); // TODO: unravel the state mess if we get here
  }
  m_pOrder.reset();
}

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_pPosition->CancelOrders();
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_pPosition->ClosePosition();
}

void Strategy::SaveWatch( const std::string& sPrefix ) {
  m_pPosition->GetWatch()->SaveSeries( sPrefix );
}

void Strategy::CloseAndDone() {
  std::cout << "Sending Close & Done" << std::endl;
  m_pPosition->ClosePosition();
  m_stateTrade = ETradeState::Done;
}