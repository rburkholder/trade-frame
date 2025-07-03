/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * Project: SP500
 * Created: April 14, 2025 20:32:29
 */

#include <algorithm>

#include <boost/log/trivial.hpp>

#include "LSTM.hpp"
#include "Strategy.hpp"
#include "HyperParameters.hpp"

Strategy::Strategy(
  ou::ChartDataView& cdv
, fConstructWatch_t&& fConstructWatch
, fConstructPosition_t&& fConstructPosition
, fStart_t&& fStart
, fStop_t&& fStop
)
: ou::tf::DailyTradeTimeFrame<Strategy>()
, m_cdv( cdv )
, m_stateTrade( ETradeState::Init )
, m_fConstructWatch( std::move( fConstructWatch ) )
, m_fConstructPosition( std::move( fConstructPosition ) )
, m_fStart( std::move( fStart ) )
, m_fStop( std::move( fStop ) )
, m_ceShortEntry( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceLongEntry( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceShortFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceLongFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceShortExit( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Red )
, m_ceLongExit( ou::ChartEntryShape::EShape::LongStop, ou::Colour::Blue )
, m_bfQuotes01Sec(  1 )
, m_dblAdv {}, m_dblDec {}, m_dblAdvDecRatio {}
, m_dblMid {}
, m_dblEma013 {}, m_dblEma029 {}
, m_dblEma050 {}, m_dblEma200 {}
, m_stateTickHi( ETickHi::Neutral ), m_stateTickLo( ETickLo::Neutral )
, m_nEnterLong {}, m_nEnterShort {}
//, m_dblLastTrin {}
, m_dblTickJ {}, m_dblTickL {}
{
  SetupChart();

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );

  m_fConstructPosition(
    "SPY",
    [this]( pPosition_t pPosition ){
      m_pPosition = pPosition;
      pWatch_t pWatch = pPosition->GetWatch();
      pWatch->OnQuote.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleQuote ) );
      pWatch->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTrade ) );
      pWatch->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "JT6T.Z",
    [this]( pWatch_t pWatch ){
      m_pTickJ = pWatch;
      m_pTickJ->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTickJ ) );
      m_pTickJ->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "LI6N.Z",
    [this]( pWatch_t pWatch ){
      m_pTickL = pWatch;
      m_pTickL->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleTickL ) );
      m_pTickL->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "II6A.Z", // advancers
    [this]( pWatch_t pWatch ){
      m_pAdv = pWatch;
      m_pAdv->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleAdv ) );
      m_pAdv->StartWatch();
      Start();
    } );

  m_fConstructWatch(
    "II6D.Z", // decliners
    [this]( pWatch_t pWatch ){
      m_pDec = pWatch;
      m_pDec->OnTrade.Add( fastdelegate::MakeDelegate( this, &Strategy::HandleDec ) );
      m_pDec->StartWatch();
      Start();
    } );

  }

Strategy::~Strategy() {
  if ( m_pDec ) {
    m_pDec->StopWatch();
    m_pDec->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleDec ) );
  }
  if ( m_pAdv ) {
    m_pAdv->StopWatch();
    m_pAdv->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleAdv ) );
  }
  if ( m_pTickJ ) {
    m_pTickJ->StopWatch();
    m_pTickJ->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTickJ ) );
  }
  if ( m_pTickL ) {
    m_pTickL->StopWatch();
    m_pTickL->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTickL ) );
  }
  if ( m_pPosition ) {
    pWatch_t pWatch = m_pPosition->GetWatch();
    pWatch->StopWatch();
    pWatch->OnQuote.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleQuote ) );
    pWatch->OnTrade.Remove( fastdelegate::MakeDelegate( this, &Strategy::HandleTrade ) );
  }
  m_bfQuotes01Sec.SetOnBarComplete( nullptr );
  m_cdv.Clear();
}

void Strategy::Start() {
  bool bOkToStart( true );
  bOkToStart &= nullptr != m_pPosition.get();
  bOkToStart &= nullptr != m_pTickJ.get();
  bOkToStart &= nullptr != m_pTickL.get();
  bOkToStart &= nullptr != m_pAdv.get();
  bOkToStart &= nullptr != m_pDec.get();
  if ( bOkToStart ) {
    m_fStart();
  }
}

void Strategy::SetupChart() {

  static const ou::Colour::EColour c_colourEma200( ou::Colour::OrangeRed );
  static const ou::Colour::EColour c_colourEma50(  ou::Colour::DarkMagenta );
  static const ou::Colour::EColour c_colourEma29(  ou::Colour::DarkTurquoise );
  static const ou::Colour::EColour c_colourEma13(  ou::Colour::Purple );
  static const ou::Colour::EColour c_colourPrice(  ou::Colour::DarkGreen );
  static const ou::Colour::EColour c_colourTickJ(  ou::Colour::Chocolate );
  static const ou::Colour::EColour c_colourTickL(  ou::Colour::MediumPurple );
  static const ou::Colour::EColour c_colourAdvDec( ou::Colour::Maroon );


  m_cemPosOne.AddMark( +1.0, ou::Colour::Black,   "+1" );
    m_cemZero.AddMark(  0.0, ou::Colour::Black, "zero" );
  m_cemNegOne.AddMark( -1.0, ou::Colour::Black,   "-1" );

  m_ceTrade.SetName( "Trade" );
  m_ceTrade.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );

  m_ceEma013.SetName( "13s ema" );
  m_ceEma013.SetColour( c_colourEma13 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma013 );

  m_ceEma029.SetName( "29s ema" );
  m_ceEma029.SetColour( c_colourEma29 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma029 );

  m_ceEma050.SetName( "50s ema" );
  m_ceEma050.SetColour( c_colourEma50 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma050 );

  m_ceEma200.SetName( "200s ema" );
  m_ceEma200.SetColour( c_colourEma200 );
  m_cdv.Add( EChartSlot::Price, &m_ceEma200 );

  m_cdv.Add( EChartSlot::Ratio, &m_cemZero );

  m_ceTrade_ratio.SetName( "Trade" );
  m_ceTrade_ratio.SetColour( c_colourPrice );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTrade_ratio );

  m_ceEma013_ratio.SetName( "13s ema" );
  m_ceEma013_ratio.SetColour( c_colourEma13 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma013_ratio );

  m_ceEma029_ratio.SetName( "29s ema" );
  m_ceEma029_ratio.SetColour( c_colourEma29 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma029_ratio );

  m_ceEma050_ratio.SetName( "50s ema" );
  m_ceEma050_ratio.SetColour( c_colourEma50 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma050_ratio );

  m_ceEma200_ratio.SetName( "200s ema" );
  m_ceEma200_ratio.SetColour( c_colourEma200 );
  m_cdv.Add( EChartSlot::Ratio, &m_ceEma200_ratio );

  m_ceTickJ_sigmoid.SetName( "TickJ" );
  m_ceTickJ_sigmoid.SetColour( c_colourTickJ );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTickJ_sigmoid );

  m_ceTickL_sigmoid.SetName( "TickL" );
  m_ceTickL_sigmoid.SetColour( c_colourTickL );
  m_cdv.Add( EChartSlot::Ratio, &m_ceTickL_sigmoid );

  m_ceAdvDec_ratio.SetName( "AdvDec" );
  m_ceAdvDec_ratio.SetColour( c_colourAdvDec );
  m_cdv.Add( EChartSlot::Ratio, &m_ceAdvDec_ratio );

  m_ceVolume.SetName( "Volume" );
  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::Tick, &m_cemZero );

  m_ceTickJ.SetName( "TickJ" );
  m_ceTickJ.SetColour( c_colourTickJ );
  m_cdv.Add( EChartSlot::Tick, &m_ceTickJ );

  m_ceTickL.SetName( "TickL" );
  m_ceTickL.SetColour( c_colourTickL );
  m_cdv.Add( EChartSlot::Tick, &m_ceTickL );

  m_ceAdvDec.SetName( "AdvDec" );
  m_ceAdvDec.SetColour( c_colourAdvDec );
  m_cdv.Add( EChartSlot::AdvDec, &m_cemZero );
  m_cdv.Add( EChartSlot::AdvDec, &m_ceAdvDec );

  m_ceProfitLoss.SetName( "P/L" );
  //m_cdv.Add( EChartSlot::PL, &m_cemZero );
  m_cdv.Add( EChartSlot::PL, &m_ceProfitLoss );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceLongFill );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );
  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceShortFill );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  m_quote = quote;
  const auto dt( quote.DateTime() );
  m_bfQuotes01Sec.Add( dt, m_quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm
}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {
  m_trade = trade;
  m_vwp.Add( trade.Price(), trade.Volume() );
  m_ceTrade.Append( trade.DateTime(), trade.Price() );
  m_ceVolume.Append( trade.DateTime(), trade.Volume() );
  TimeTick( trade );
}

void Strategy::HandleTickJ( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    const auto dt( tick.DateTime() );
    m_dblTickJ = tick.Price() / 100.0;
    m_ceTickJ.Append( dt, m_dblTickJ );  // approx normalization
  }
}

void Strategy::HandleTickL( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    const auto dt( tick.DateTime() );
    m_dblTickL = tick.Price() / 200.0;
    m_ceTickL.Append( dt, m_dblTickL );  // approx normalization

    static const double hi( +1.0 );

    switch ( m_stateTickHi ) {
      case ETickHi::Neutral:
        if ( hi <= m_dblTickL ) {
          m_stateTickHi = ETickHi::UpOvr;
        }
        break;
      case ETickHi::UpOvr:
      case ETickHi::Up:
        m_stateTickHi = ( hi < m_dblTickL ) ? ETickHi::Up : ETickHi::DnOvr;
        break;
      case ETickHi::DnOvr:
        m_stateTickHi = ( hi > m_dblTickL ) ? ETickHi::Neutral : ETickHi::UpOvr;
        break;
    }

    static const double lo( -1.0 );

    switch ( m_stateTickLo ) {
      case ETickLo::Neutral:
        if ( lo >= m_dblTickL ) {
          m_stateTickLo = ETickLo::DnOvr;
        }
        break;
      case ETickLo::DnOvr:
      case ETickLo::Dn:
        m_stateTickLo = ( lo > m_dblTickL ) ? ETickLo::Dn : ETickLo::UpOvr;
        break;
      case ETickLo::UpOvr:
        m_stateTickLo = ( lo < m_dblTickL ) ? ETickLo::Neutral : ETickLo::DnOvr;
        break;
    }
  }
}

void Strategy::HandleAdv( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    m_dblAdv = tick.Price();
    CalcAdvDec( tick.DateTime() );
  }
}

void Strategy::HandleDec( const ou::tf::Trade& tick ) {
  if ( RHTrading() ) {
    m_dblDec = tick.Price();
    CalcAdvDec( tick.DateTime() );
  }
}

void Strategy::CalcAdvDec( boost::posix_time::ptime dt ) {
  const double sum( m_dblAdv + m_dblDec );
  const double diff( m_dblAdv - m_dblDec );
  const double ratio( diff / sum );
  m_dblAdvDecRatio = ratio;
  m_ceAdvDec.Append( dt, ratio );
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  if ( RHTrading() ) {
    Calc01SecIndicators( bar );
  }
  TimeTick( bar );
}

void Strategy::HandleRHTrading( const ou::tf::Trade& trade ) {
  const auto dt( trade.DateTime() );
  const auto price( trade.Price() );
  switch ( m_stateTrade ) {
    case ETradeState::Search:
      if ( ETickHi::DnOvr == m_stateTickHi ) {
        BOOST_LOG_TRIVIAL(trace) << "ETickHi::DnOvr enter";
        m_ceShortEntry.AddLabel( dt, price, "short" );
        ++m_nEnterShort;
        m_stateTrade = ETradeState::ShortSubmitted;
      }
      else {
        if ( ETickLo::UpOvr == m_stateTickLo ) {
          BOOST_LOG_TRIVIAL(trace) << "ETickLo::UpOvr enter";
          m_ceLongEntry.AddLabel( dt, price, "long" );
          ++m_nEnterLong;
          m_stateTrade = ETradeState::LongSubmitted;
        }
      }
      break;
    case ETradeState::LongSubmitted:
      if ( ETickLo::Neutral == m_stateTickLo ) {
        BOOST_LOG_TRIVIAL(trace) << "ETickLo::Neutral exit";
        m_stateTrade = ETradeState::Search;
      }
      break;
    case ETradeState::ShortSubmitted:
      if ( ETickHi::Neutral == m_stateTickHi ) {
        BOOST_LOG_TRIVIAL(trace) << "ETickHi::Neutral exit";
        m_stateTrade = ETradeState::Search;
      }
      break;
    case ETradeState::LongExit:
      break;
    case ETradeState::ShortExit:
      break;
    case ETradeState::LongExitSubmitted:
      break;
    case ETradeState::ShortExitSubmitted:
      break;
    case ETradeState::Neutral:
      BOOST_LOG_TRIVIAL(trace) << "neutral," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::EndOfDayCancel:
      BOOST_LOG_TRIVIAL(trace) << "eod cancel," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(trace) << "eod neutral," << m_nEnterLong << ',' << m_nEnterShort;
      break;
    case ETradeState::Done:
      break;
    case ETradeState::Init:
      m_stateTrade = ETradeState::Search;
      break;
  }
}

namespace {
  // from AutoTrade/NeuralNet.cpp
  template<int coef>
  inline double binary_sigmoid( double x ) { // 0.0 .. 1.0
    constexpr double k( -coef );
    return 1.0 / ( 1.0 + std::exp( x * k ) );
  }

  // from AutoTrade/NeuralNet.cpp
  template<int coef>
  inline double bipolar_sigmoid( double x ) { // -1.0 .. +1.0 (aka tanh)
    constexpr double k( -coef );
    const double ex = std::exp( x * k );
    return ( 1.0 - ex ) / ( 1.0 + ex );
  }

  // additional activiation functions:
  // https://machinelearninggeek.com/activation-functions/
}

void Strategy::Calc01SecIndicators( const ou::tf::Bar& bar ) {

  const double vwp( m_vwp() );
  const double price( 0.0 == vwp ? bar.Close() : vwp );
  const ou::tf::Price price_( bar.DateTime(), price );

  UpdateEma< 13>( price_, m_dblEma013, m_ceEma013  );
  UpdateEma< 29>( price_, m_dblEma029, m_ceEma029  );
  UpdateEma< 50>( price_, m_dblEma050, m_ceEma050  );
  UpdateEma<200>( price_, m_dblEma200, m_ceEma200 );

  struct maxmin {
    double& max;
    double& min;

    maxmin( double& max_, double& min_, double init )
    : max( max_ ), min( min_ ) {
      max = min = init;
    }

    void test( const double value ) {
      if ( max < value ) max = value;
      else {
        if ( min > value ) min = value;
      }
    }
  };

  double max;
  double min;
  maxmin mm( max, min, m_dblEma200 );
  mm.test( m_dblEma050 );
  mm.test( m_dblEma029 );
  mm.test( m_dblEma013 );
  // on purpose: no test on price

  if ( max > min ) {

    const double range( max - min );

    struct Real {
      const double dbl;
      const float flt;
      Real( double val ): dbl( val ), flt( val ) {}
    };

    // detrend timeseries to 0.0 - 1.0
    const Real ratioEma200( ( m_dblEma200 - min ) / range );
    const Real ratioEma050( ( m_dblEma050 - min ) / range );
    const Real ratioEma029( ( m_dblEma029 - min ) / range );
    const Real ratioEma013( ( m_dblEma013 - min ) / range );

    const double ratioPrice( ( ( price - min ) / range ) * 2.0 - 1.0 ); // even scaling top and bottom
    const double sigmoidPrice( bipolar_sigmoid<3>( ratioPrice ) );
    const Real scaledPrice( sigmoidPrice * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    const double sigmoidTickJ( bipolar_sigmoid<2>( m_dblTickJ ) ); // even scaling top and bottom
    const Real realTickJ( sigmoidTickJ * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    const double sigmoidTickL( bipolar_sigmoid<2>( m_dblTickL ) ); // even scaling top and bottom
    const Real realTickL( sigmoidTickL * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    const Real fltAdvDec( m_dblAdvDecRatio * 0.5 + 0.5 ); // translate to 0.0 - 1.0

    m_ceEma200_ratio.Append( bar.DateTime(), ratioEma200.dbl );
    m_ceEma050_ratio.Append( bar.DateTime(), ratioEma050.dbl );
    m_ceEma029_ratio.Append( bar.DateTime(), ratioEma029.dbl );
    m_ceEma013_ratio.Append( bar.DateTime(), ratioEma013.dbl );

    m_ceTrade_ratio.Append( bar.DateTime(), scaledPrice.dbl );

    m_ceTickJ_sigmoid.Append( bar.DateTime(), realTickJ.dbl );
    m_ceTickL_sigmoid.Append( bar.DateTime(), realTickL.dbl );

    m_ceAdvDec_ratio.Append( bar.DateTime(), fltAdvDec.dbl );

    const fields_t<float> scaled(
      ratioEma200.flt, ratioEma050.flt, ratioEma029.flt, ratioEma013.flt
    , scaledPrice.flt
    , realTickJ.flt, realTickL.flt
    //, 0.5 // dblAdvDec use neutral mid until multi-day series tackled
    );
    m_vDataScaled.push_back( scaled );

  }

}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;
  m_pPosition->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );
}

Strategy::pLSTM_t Strategy::BuildModel( torch::DeviceType device, const HyperParameters& hp ) {

  BOOST_LOG_TRIVIAL(info)
    << "scaled vector size: "
           << m_vDataScaled.size() << "sec"
    << ',' << m_vDataScaled.size() / 60.0 << "min"
    << ',' << m_vDataScaled.size() / 3600.0 << "hr"
    ;

  BOOST_LOG_TRIVIAL(info)
           << "size of fields_t<float>: " << sizeof( fields_t<float> )
    << ',' << "size of fields_t<double>: " << sizeof( fields_t<double> )
    ;

  // references:
  //   https://github.com/pytorch/pytorch/issues/14000
  //   https://github.com/pytorch/pytorch/blob/main/tools/autograd/templates/variable_factories.h
  //   https://docs.pytorch.org/cppdocs/notes/tensor_creation.html
  //   https://docs.alcf.anl.gov/polaris/data-science/frameworks/libtorch/#linking-the-torch-libraries
  //   https://www.geeksforgeeks.org/deep-learning/long-short-term-memory-networks-using-pytorch/

  // using as a guide:
  //  https://machinelearningmastery.com/how-to-develop-lstm-models-for-time-series-forecasting/

  // Note: Y.length must be same as X.length, but Y can have different feature count

  static const size_t secondsSequence( 210 ); // duration of sample sequence
  static const size_t secondsYOffset( 30 ); // attempt prediction this far in the future
  static const size_t secondsTotal( secondsSequence + secondsYOffset ); // training + prediction

  static const size_t secondsSampleOffset( 23 );  // offset for each sample

  static const int nOutputFeature( 1 );

  static const int nInputFeature( nInputFeature_ );
  static const size_t sizeFloat( sizeof( float ) );

  static const size_t nFieldBytes( nInputFeature * sizeFloat );
  assert( nFieldBytes == sizeof( fields_t<float> ) );

  const size_t nSamples_theory( m_vDataScaled.size() / secondsSampleOffset ); // assumes integer math with truncation

  long nSamples_actual {};
  vValuesFlt_t::size_type ixDataScaled {};

  vValuesFlt_t vSourceForTensorX; // implicit 3 dimensions:  [sample index][sample size in seconds][feature list]
  std::vector<float> vSourceForTensorY; // [samples match X][1 second for prediction][last index implies 1 feature]

  {
    vValuesFlt_t::const_iterator bgnX = m_vDataScaled.begin(); // begin of input
    vValuesFlt_t::const_iterator endX( bgnX + secondsSequence ); // end of inpu

    vValuesFlt_t::const_iterator bgnY( bgnX + secondsYOffset ); // start of prediction time frame
    vValuesFlt_t::const_iterator endY( bgnY + secondsSequence ); // length same as X sequence length

    while ( m_vDataScaled.size() > ( ixDataScaled + secondsTotal ) ) {

      // append X sequence
      std::copy( bgnX, endX, std::back_inserter( vSourceForTensorX ) );

      // append Y sequence
      std::for_each(
         bgnY, endY,
         [&vSourceForTensorY]( auto& entry ){
          vSourceForTensorY.push_back( entry.fields[ ixTrade ] );
        } );

      bgnX += secondsSampleOffset;
      endX += secondsSampleOffset;
      bgnY += secondsSampleOffset;
      endY += secondsSampleOffset;
      ixDataScaled += secondsSampleOffset;
      ++nSamples_actual;
    }
    // won't match for now:
    BOOST_LOG_TRIVIAL(info) << "nSamples: " << nSamples_actual << ',' << nSamples_theory;
  }

  BOOST_LOG_TRIVIAL(info)
    << "data usage: " << ixDataScaled << ',' << m_vDataScaled.size() << ',' << ixDataScaled + secondsTotal;
  BOOST_LOG_TRIVIAL(info)
    << "input samples * (time steps in each sample): "
    << nSamples_actual
    << ',' << secondsSequence
    << '=' << '(' << vSourceForTensorX.size()
    << ','        << vSourceForTensorY.size()
           << ')'
    ;

  // note: from_blob does not manage memory, so underlying needs to be valid during lifetime of tensor

  torch::Tensor tensorX = // input
    torch::from_blob( vSourceForTensorX.data(), { nSamples_actual, secondsSequence, nInputFeature },
    torch::TensorOptions().dtype( torch::kFloat32 )
  ).to( device ); // without .clone(), data source remains in the vector
  BOOST_LOG_TRIVIAL(info) << "tensorX sizes: " << tensorX.sizes();

  torch::Tensor tensorY = // output
    torch::from_blob( vSourceForTensorY.data(), { nSamples_actual, secondsSequence, nOutputFeature },
    torch::TensorOptions().dtype( torch::kFloat32 )
  ).to( device ); // without .clone(), data source remains in the vector
  BOOST_LOG_TRIVIAL(info) << "tensorY sizes: " << tensorY.sizes();


  // https://github.com/pytorch/pytorch
  // https://docs.pytorch.org/docs/stable/torch.html
  // https://docs.pytorch.org/cppdocs/api/classat_1_1_tensor.html

  //BOOST_LOG_TRIVIAL(info) << "tensor: " << tensor;

  // building the time series, need to fix the tensor based upon:
  // https://machinelearningmastery.com/how-to-develop-lstm-models-for-time-series-forecasting/
  // https://machinelearningmastery.com/lstm-for-time-series-prediction-in-pytorch/
  // * The power of an LSTM cell depends on the size of the hidden state or cell memory,
  //   which usually has a larger dimension than the number of features in the input.

  // Hyperparameters
  const int batch_size( nSamples_actual );
  const int input_size( nInputFeature );
  const int hidden_size( nInputFeature * 9 );
  const int output_size( nOutputFeature );
  const int sequence_length( secondsSequence );
  const int num_layers( 1 );

  assert( 100 <= hp.m_nEpochs );

  const double learning_rate( hp.m_dblLearningRate ); // 0.001 is good
  const int num_epochs( hp.m_nEpochs ); // 10000 is good
  const int epoch_skip( num_epochs / 100 );

    // Instantiate the model
  pLSTM_t pModel = std::make_shared<LSTM>( input_size, hidden_size, num_layers, output_size );
  pModel->train();
  pModel->to( device );

  // Loss, optimizer, state
  torch::nn::MSELoss criterion( torch::nn::MSELossOptions().reduction( torch::kMean ) ); // loss function
  criterion->to( device );

  torch::optim::Adam optimizer( pModel->parameters(), learning_rate );

  for ( size_t epoch = 0; epoch < num_epochs; ++epoch ) {

    torch::Tensor loss;

    LSTM::lstm_state_t state( pModel->init_states( device, batch_size ) );

    torch::Tensor predictions = pModel->forward( tensorX, state );
    loss = criterion( predictions, tensorY );

    optimizer.zero_grad();
    loss.backward();
    optimizer.step();

    //std::get<0>( state ).detach();
    //std::get<1>( state ).detach();

    if ( 0 == ( ( 1 + epoch ) % epoch_skip ) ) {
      BOOST_LOG_TRIVIAL(info) << "epoch " << ( 1 + epoch ) << '/' << num_epochs << " loss: " << loss.item<float>();
    }

  }

  BOOST_LOG_TRIVIAL(info) << "training done";

  return pModel;

}

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(info) << "order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(warning) << "order " << order.GetOrderId() << " cancelled - state machine needs fixes";
      break;
    default:
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
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
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
      break;
    case ETradeState::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
      //m_stateTrade = ETradeState::Search;
      m_stateTrade = ETradeState::Neutral;
      break;
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      // figure out what labels to apply
      break;
    case ETradeState::Done:
      break;
    default:
      assert( false ); // TODO: unravel the state mess if we get here
  }
  m_pOrder.reset();
}

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "event HandleCancel," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_stateTrade = ETradeState::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration td ) { // one shot
  BOOST_LOG_TRIVIAL(trace) << "event HandleGoNeutral," << td << ',' << m_nEnterLong << ',' << m_nEnterShort;
  m_stateTrade = ETradeState::EndOfDayNeutral;
  if ( m_pPosition ) {
    m_pPosition->ClosePosition();
  }
}

