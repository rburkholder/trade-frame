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
 * Project: IndicatorTrading
 * Created: July 4, 2022 16:14
 */

#include "Strategy.hpp"

Strategy::Strategy(
  pPosition_t pPosition // futures
//, boost::gregorian::date dateTrading
)
: ou::tf::DailyTradeTimeFrame<Strategy>()
//, m_dateTrading( dateTrading )
, m_pPosition( pPosition)
, m_k {}, m_kMin {}, m_kMax {}
, m_dblImbalanceMean {}, m_dblImbalanceSlope {}
{
  pWatch_t pWatch = m_pPosition->GetWatch();

  m_pTSSWStochastic = std::make_unique<ou::tf::TSSWStochastic>(
    pWatch->GetQuotes(), 8, boost::posix_time::seconds( 7 ), // 8 periods of 7 seconds each
    [this]( boost::posix_time::ptime dt, double k, double min, double max ){
      m_k = k; m_kMin = min; m_kMax = max;
    }
  );

  pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
}

Strategy::~Strategy() {
  pWatch_t pWatch = m_pPosition->GetWatch();
  pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {
  TimeTick( quote );
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
}

void Strategy::HandleGoingNeutral( const ou::tf::Quote& quote ) {
}

void Strategy::HandleBellHeard( boost::gregorian::date, boost::posix_time::time_duration ) {
}

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) {
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) {
}

void Strategy::HandleAtRHClose( boost::gregorian::date, boost::posix_time::time_duration ) {
}
