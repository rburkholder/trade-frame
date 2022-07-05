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
 * File:    Strategy.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: July 4, 2022 16:14
 */

 #pragma once

 #include <TFTrading/Position.h>
 #include <TFTrading/DailyTradeTimeFrames.h>

 #include <TFIndicators/TSSWStochastic.h>

class Strategy:
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend ou::tf::DailyTradeTimeFrame<Strategy>;
public:

  using pWatch_t      = ou::tf::Watch::pWatch_t;
  using pPosition_t   = ou::tf::Position::pPosition_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using pOrder_t = ou::tf::Position::pOrder_t;

  Strategy(
    pPosition_t // futures
//  , boost::gregorian::date dateTrading
  );
  virtual ~Strategy();

  using pStrategy_t = std::unique_ptr<Strategy>;
  static pStrategy_t Factory( pPosition_t pPosition ) {
    return std::make_unique<Strategy>( pPosition );
  }

  void SetImbalance( double mean, double slope ) {
    m_dblImbalanceMean = mean;
    m_dblImbalanceSlope = slope;
  }

protected:
private:

  //boost::gregorian::date m_dateTrading; // use for setting day of market open (could be 'yesterday')

  pPosition_t m_pPosition;

  using pTSSWStochastic_t = std::unique_ptr<ou::tf::TSSWStochastic>;
  pTSSWStochastic_t m_pTSSWStochastic;

  double m_k, m_kMin, m_kMax; // stochastic parameters
  double m_dblImbalanceMean, m_dblImbalanceSlope;

  void HandleQuote( const ou::tf::Quote& quote );

  // one shot inherited states:
  void HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleCancel( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration );
  void HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration );

  void HandleRHTrading( const ou::tf::Quote& quote );
  void HandleGoingNeutral( const ou::tf::Quote& quote );
  void HandleAfterRH( const ou::tf::Quote& quote ) {};

};