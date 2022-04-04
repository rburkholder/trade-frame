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

// https://indico.cern.ch/event/697389/contributions/3062036/attachments/1712790/2761904/Support_for_SIMD_Vectorization_in_ROOT_ROOT_Workshop_2018.pdf
// https://www.intel.com/content/www/us/en/develop/documentation/advisor-user-guide/top/analyze-cpu-roofline.html
// https://stackoverflow.com/questions/52653025/why-is-march-native-used-so-rarely

 #include <chrono>

// no longer use iostream, std::cout has a multithread contention problem
//   due to it being captured to the gui, and is not thread safe
 #include <boost/log/trivial.hpp>

 #include <boost/lexical_cast.hpp>

#include <rdaf/TH2.h>
#include <rdaf/TTree.h>
#include <rdaf/TFile.h>

#include <OUCharting/ChartDataView.h>

#include <TFTrading/Watch.h>

#include <TFVuTrading/TreeItem.hpp>

#include "ConfigParser.hpp"

#include "Strategy.h"

using pWatch_t = ou::tf::Watch::pWatch_t;

Strategy::Strategy(
  const config_t config
, TreeItem* pTreeItem
, pFile_t pFile
)
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_pTreeItemSymbol( pTreeItem )
, m_pFile( pFile )
, m_bChangeConfigFileMessageLatch( false )
, m_stateTrade( ETradeState::Init )
, m_config( config )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_bfQuotes01Sec( 1 )
{
  assert( m_pFile );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceVolume.SetName( "Volume" );

  m_ceSkewness.SetName( "Skew" );

  m_ceProfitLoss.SetName( "P/L" );

  m_ceExecutionTime.SetName( "Execution Time" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

}

Strategy::~Strategy() {
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

  m_cdv.Add( EChartSlot::Skew, &m_ceSkewness );

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

  pWatch->RecordSeries( false );
  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::LoadHistory( TClass* tc ) {

  BOOST_LOG_TRIVIAL(info) << "  load: " << tc->GetName();

  if ( 0 == strcmp( ( m_config.sSymbol + "_quotes" ).c_str(), tc->GetName() ) ) {
    TTree* pQuotes = dynamic_cast<TTree*>( tc );
  }

  if ( 0 == strcmp( ( m_config.sSymbol + "_trades" ).c_str(), tc->GetName() ) ) {
    TTree* pTrades = dynamic_cast<TTree*>( tc );
  }

  if ( 0 == strcmp( ( m_config.sSymbol + "_h1" ).c_str(), tc->GetName() ) ) {
    TH2D* pH1 = dynamic_cast<TH2D*>( tc );
  }
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
    ( sSymbol + "_quotes" ).c_str(), ( sSymbol + " quotes" ).c_str()
  );
  m_pTreeQuote->Branch( "quote", &m_branchQuote, "time/D:ask/D:askvol/l:bid/D:bidvol/l" );
  if ( !m_pTreeQuote ) {
    BOOST_LOG_TRIVIAL(error) << "problems m_pTreeQuote";
  }
  m_pTreeQuote->SetDirectory( m_pFile.get() );

  m_pTreeTrade = std::make_shared<TTree>(
    ( sSymbol + "_trades" ).c_str(), ( sSymbol + " trades" ).c_str()
  );
  m_pTreeTrade->Branch( "trade", &m_branchTrade, "time/D:price/D:vol/l:direction/L" );
  if ( !m_pTreeTrade ) {
    BOOST_LOG_TRIVIAL(error) << "problems m_pTreeTrade";
  }
  m_pTreeTrade->SetDirectory( m_pFile.get() );

  m_pHistVolume = std::make_shared<TH2D>(
    ( sSymbol + "_h1" ).c_str(), ( sSymbol + " Volume Histogram" ).c_str(),
    m_config.nPriceBins, m_config.dblPriceLower, m_config.dblPriceUpper,
    m_config.nTimeBins, m_config.dblTimeLower, m_config.dblTimeUpper
  );
  if ( !m_pHistVolume ) {
    BOOST_LOG_TRIVIAL(error) << "problems history";
  }
  m_pHistVolume->SetDirectory( m_pFile.get() );

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
    m_pHistVolume->Fill( trade.Price(), m_branchTrade.time, trade.Volume() );
  }

}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;

  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );

  TimeTick( bar );
}

/*
  // strip off fractional seconds
  boost::posix_time::ptime dtQuote
    = quote.DateTime()
    - boost::posix_time::time_duration( 0, 0, 0, quote.DateTime( ).time_of_day().fractional_seconds() );

  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, quantity, price );

  m_pOrder->SetGoodTillDate( dtQuote + boost::posix_time::seconds( 30 ) );
  m_pOrder->SetTimeInForce( ou::tf::TimeInForce::GoodTillDate );
*/

void Strategy::EnterLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( bar.DateTime(), bar.Close(), "Long Submit" );
  m_stateTrade = ETradeState::LongSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::EnterShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( bar.DateTime(), bar.Close(), "Short Submit" );
  m_stateTrade = ETradeState::ShortSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ExitLong( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( bar.DateTime(), bar.Close(), "Long Exit Submit" );
  m_stateTrade = ETradeState::LongExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ExitShort( const ou::tf::Bar& bar ) {
  m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 100 );
  m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( bar.DateTime(), bar.Close(), "Short Exit Submit" );
  m_stateTrade = ETradeState::ShortExitSubmitted;
  m_pPosition->PlaceOrder( m_pOrder );
  ShowOrder( m_pOrder );
}

void Strategy::ShowOrder( pOrder_t pOrder ) {
  m_pTreeItemOrder = m_pTreeItemSymbol->AppendChild(
      "Order "
    + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() )
    );
}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  // DailyTradeTimeFrame: Trading during regular active equity market hours

  const std::chrono::time_point<std::chrono::system_clock> begin
    = std::chrono::system_clock::now();

  bool bTriggerEntry( false );
  double skew( 0.0 );

  if ( m_pHistVolume ) {

    auto n = m_pHistVolume->GetEntries();
    if ( n < 1000 ) {
    }
    else {

      std::time_t nTime = boost::posix_time::to_time_t( bar.DateTime() );
      nTime = (double)nTime / 1000.0;

      //find the bin that the time given belongs to:
      Int_t bin_y = m_pHistVolume->GetYaxis()->FindBin( nTime );

      if ( bin_y < 1 ) {
        if ( !m_bChangeConfigFileMessageLatch ) {
          BOOST_LOG_TRIVIAL(warning) << m_config.sSymbol << ", need to adjust lower time in config file";
          m_bChangeConfigFileMessageLatch = true;
        }
      }
      else {
        //now find projection of h1 from the beginning till now:
        auto h1_x = m_pHistVolume->ProjectionX( ( m_config.sSymbol + "_px" ).c_str(), 1, bin_y );
        double skew_ = h1_x->GetSkewness( 1 );

        switch ( fpclassify( skew_ ) ) {
            case FP_INFINITE:
              skew_ = 0.0;
              break;
            case FP_NAN:
              skew_ = 0.0;
              break;
            case FP_NORMAL:
              break;
            case FP_SUBNORMAL:
              skew_ = 0.0;
              break;
            case FP_ZERO:
              break;
            default:
              break;
        }

        if ( 0.1 < skew_ ) {
          bTriggerEntry = true;
        }
        skew = skew_;
      }
    }
    m_ceSkewness.Append( bar.DateTime(), skew );
  }

  switch ( m_stateTrade ) {
    case ETradeState::Search:
      if ( bTriggerEntry ) {
        BOOST_LOG_TRIVIAL(info) << m_pPosition->GetInstrument()->GetInstrumentName() << " entry with skew: " << skew;
        EnterLong( bar );
      }
      break;
    case ETradeState::LongSubmitted:
      // wait for order to execute
      break;
    case ETradeState::LongExit:
      {
        double pl = m_pPosition->GetUnRealizedPL();
        if ( 20.0 < pl ) {
          ExitLong( bar ); // profit
        }
        else {
          //if ( ( -5.0 > pl ) && ( skew ) ) {
          //if ( -5.0 > pl ) {
          //  ExitLong( bar ); // stop loss
          //}
        }
      }
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
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
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
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
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
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Fill" );
      m_stateTrade = ETradeState::LongExit;
      break;
    case ETradeState::ShortSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Fill" );
      m_stateTrade = ETradeState::ShortExit;
      break;
    case ETradeState::LongExitSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Exit Fill" );
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
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
  // RecordSeries has been set to false
  m_pPosition->GetWatch()->SaveSeries( sPrefix );
}

void Strategy::CloseAndDone() {
  std::cout << "Sending Close & Done" << std::endl;
  m_pPosition->ClosePosition();
  m_stateTrade = ETradeState::Done;
}