/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/InstrumentData.h>
#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>

class StrategyTradeOptions {
public:

  typedef ou::tf::CProviderInterfaceBase::pProvider_t pProvider_t;

  StrategyTradeOptions( pProvider_t pExecutionProvider, pProvider_t pData1Provider, pProvider_t pData2Provider );
  ~StrategyTradeOptions(void);

//  void Start( void ); // for real time
//  void Start( const std::string& sSymbolPath );  // for simulation
  void Start( const std::string& sUnderlying, boost::gregorian::date dtOptionNearDate, boost::gregorian::date dtOptionFarDate );
  void Stop( void );

protected:
private:

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pData1Provider;
  pProvider_t m_pData2Provider;

  ou::tf::CIQFeedProvider::pProvider_t m_pData1ProviderIQFeed;
  ou::tf::CIBTWS::pProvider_t m_pData2ProviderIB;
  ou::tf::CIBTWS::pProvider_t m_pExecutionProviderIB;

  ou::tf::InstrumentData* m_pUnderlying;

  boost::gregorian::date m_dateOptionNearDate;
  boost::gregorian::date m_dateOptionFarDate;

  void LoadExistingInstruments( const std::string& sUnderlying );

  void HandleUnderlyingContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleUnderlyingContractDetailsDone( void );

  void HandleNearDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleNearDateContractDetailsDone( void );

  void HandleFarDateContractDetails( const ou::tf::CIBTWS::ContractDetails&, ou::tf::CIBTWS::pInstrument_t& );
  void HandleFarDateContractDetailsDone( void );
};

