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
 * File:    Strategy.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: April 14, 2025 20:32:29
 */

#pragma once

#include <vector>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <OUCharting/ChartDataView.h>

//#include <TFIndicators/TSEMA.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFBitsNPieces/TrackOrder.hpp>

class Strategy
: public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend ou::tf::DailyTradeTimeFrame<Strategy>;
  friend class boost::serialization::access;
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  using fConstructedWatch_t = std::function<void( pWatch_t )>;
  using fConstructWatch_t = std::function<void( const std::string sIQFeedSymbolName, fConstructedWatch_t&& )>;

  using fConstructedPosition_t = std::function<void( pPosition_t )>;
  using fConstructPosition_t = std::function<void( const std::string sIQFeedSymbolName, fConstructedPosition_t&& )>;

  using fStart_t = std::function<void()>;
  using fStop_t = std::function<void()>;

  Strategy(
    ou::ChartDataView&
  , fConstructWatch_t&&
  , fConstructPosition_t&&
  , fStart_t&&
  , fStop_t&&
  );
  ~Strategy();

  void PostProcess();

protected:
private:

  enum EChartSlot { Price, Volume, Tick, AdvDec, Ratio, PL };
  enum class ETradeState {
    Init,  // initiaize state in current market
    Neutral, // netral state prior to active search
    Search,  // looking for long or short enter
    LongSubmitted, // order has been submitted, waiting for confirmation
    LongExit,  // position exists, looking for exit
    ShortSubmitted,  // order has been submitted, waiting for confirmtaion
    ShortExit,  // position exists, looking for exit
    LongExitSubmitted, // wait for exit to complete
    ShortExitSubmitted, // wait for exit to complete
    EndOfDayCancel,
    EndOfDayNeutral,
    Done // no more action
  };

  enum class ETickHi { Neutral, UpOvr, Up, DnOvr };
  enum class ETickLo { Neutral, DnOvr, Dn, UpOvr };

  using pOrder_t = ou::tf::Order::pOrder_t;

  fConstructWatch_t m_fConstructWatch;
  fConstructPosition_t m_fConstructPosition;
  fStart_t m_fStart;
  fStop_t m_fStop;

  ETradeState m_stateTrade;
  ETickHi m_stateTickHi;
  ETickLo m_stateTickLo;

  pPosition_t m_pPosition;
  pWatch_t m_pTickJ;
  pWatch_t m_pTickL;
  pWatch_t m_pTrin;
  pWatch_t m_pAdv;
  pWatch_t m_pDec;

  double m_dblMid;
  //double m_dblLastTrin;

  struct VolumeWeightedPrice {

    double m_dblSumVolume;
    double m_dblSumVolumePrice;

    VolumeWeightedPrice()
    : m_dblSumVolumePrice {}, m_dblSumVolume {} {}

    void Add( double price, ou::tf::Price::volume_t volume ) {
      const double volume_( volume );
      m_dblSumVolume += volume_;
      m_dblSumVolumePrice += volume_ * price;
    }

    double operator()() {
      if ( 0.0 == m_dblSumVolume ) return 0.0;
      else {
        const double price( m_dblSumVolumePrice / m_dblSumVolume );
        m_dblSumVolumePrice = m_dblSumVolume = 0.0;
        return price;
      }
    }
  };

  VolumeWeightedPrice m_vwp;

  double m_dblTickJ;
  double m_dblTickL;
  //double m_dblTickLmt;

  double m_dblAdv;
  double m_dblDec;
  double m_dblAdvDecRatio;

  double m_dblEma013;
  double m_dblEma029;
  double m_dblEma050;
  double m_dblEma200;

  ou::tf::Quote m_quote;
  ou::tf::Trade m_trade;

  enum EFeature { ixEma200 = 0, ixEma050, ixEma029, ixEma013, ixTrade, ixTickj, ixTickl, ixAdvdec, nInputFeature_ };

  template<typename type>
  using rFields_t = type[ nInputFeature_ ];

  template<typename type>
  struct fields_t {
    rFields_t<type> fields;
    fields_t( const fields_t& rhs )
    {
      std::memcpy( fields, rhs.fields, nInputFeature_ * sizeof( type ) );
    }
    // todo: how to bulid an initializer?
    fields_t( const type v1, const type v2, const type v3, const type v4
            , const type v5, const type v6, const type v7, const type v8 )
    {
      fields[ ixEma200 ] = v1;
      fields[ ixEma050 ] = v2;
      fields[ ixEma029 ] = v3;
      fields[ ixEma013 ] = v4;
      fields[ ixTrade  ] = v5;
      fields[ ixTickj  ] = v6;
      fields[ ixTickl  ] = v7;
      fields[ ixAdvdec ] = v8;
    }
  };

  using vValuesDbl_t = std::vector<fields_t<double> >;
  using vValuesFlt_t = std::vector<fields_t<float> >;
  vValuesDbl_t m_vDataRaw;
  vValuesFlt_t m_vDataScaled; // LSTM prefers float?

  pOrder_t m_pOrder;

  size_t m_nEnterLong;
  size_t m_nEnterShort;

  ou::ChartDataView& m_cdv;

  ou::ChartEntryMark m_cemPosOne;
  ou::ChartEntryMark m_cemZero;
  ou::ChartEntryMark m_cemNegOne;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryVolume m_ceVolume;

  //ou::ChartEntryIndicator m_ceAsk;
  //ou::ChartEntryIndicator m_ceBid;

  ou::ChartEntryIndicator m_ceEma013;
  ou::ChartEntryIndicator m_ceEma029;
  ou::ChartEntryIndicator m_ceEma050;
  ou::ChartEntryIndicator m_ceEma200;

  ou::ChartEntryIndicator m_ceEma013_ratio;
  ou::ChartEntryIndicator m_ceEma029_ratio;
  ou::ChartEntryIndicator m_ceEma050_ratio;
  ou::ChartEntryIndicator m_ceEma200_ratio;
  ou::ChartEntryIndicator m_ceTrade_ratio;

  ou::ChartEntryIndicator m_ceTickJ_sigmoid;
  ou::ChartEntryIndicator m_ceTickL_sigmoid;
  ou::ChartEntryIndicator m_ceAdvDec_ratio;

  ou::ChartEntryShape m_ceLongEntry;
  ou::ChartEntryShape m_ceLongFill;
  ou::ChartEntryShape m_ceLongExit;
  ou::ChartEntryShape m_ceShortEntry;
  ou::ChartEntryShape m_ceShortFill;
  ou::ChartEntryShape m_ceShortExit;

  ou::ChartEntryIndicator m_ceTickJ;
  ou::ChartEntryIndicator m_ceTickL;
  //ou::ChartEntryIndicator m_ceTrin;

  ou::ChartEntryIndicator m_ceAdvDec;

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::tf::BarFactory m_bfQuotes01Sec;

  template<unsigned int n>
  void UpdateEma( const ou::tf::Price& price_, double& ema, ou::ChartEntryIndicator& cei ) {
    constexpr double seconds( n );
    constexpr double cur( 1.0 / seconds );
    constexpr double prv( 1.0 - cur );
    const double price( price_.Value() );
    if ( 0.0 == ema ) {
      ema = price;
    }
    else {
      ema = prv * ema + cur * price;
    }
    cei.Append( price_.DateTime(), ema );
    };

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleTickJ( const ou::tf::Trade& );
  void HandleTickL( const ou::tf::Trade& );
  void HandleTrin( const ou::tf::Trade& );

  void HandleAdv( const ou::tf::Trade& );
  void HandleDec( const ou::tf::Trade& );

  void CalcAdvDec( boost::posix_time::ptime );

  void HandleBarQuotes01Sec( const ou::tf::Bar& );
  void Calc01SecIndicators( const ou::tf::Bar& );

  void HandleRHTrading( const ou::tf::Trade& );

  void HandleRHTrading( const ou::tf::Bar& );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );

  void EnterLong( const ptime dt, const double tag );
  void EnterShort( const ptime dt, const double tag );

  void ExitLong( const ptime dt, const double tag );
  void ExitShort( const ptime dt, const double tag );

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleOrderFilled( const ou::tf::Order& );

  void SetupChart();
  void Start();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(Strategy, 1)

