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
 * File:    OptionTracker.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: March 21, 2022 23:15
 */

#pragma once

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFOptions/Option.h>

class OptionTracker {
public:

  using pOption_t = ou::tf::option::Option::pOption_t;

  OptionTracker(
    pOption_t pOption_
  , ou::ChartEntryShape& ceBullCall, ou::ChartEntryShape& ceBullPut
  , ou::ChartEntryShape& ceBearCall, ou::ChartEntryShape& ceBearPut
  );

  OptionTracker( const OptionTracker& );
  OptionTracker( OptionTracker&& );

  ~OptionTracker();

  void Emit(
    ou::tf::Trade::volume_t& volCallBuy, ou::tf::Trade::volume_t& volCallSell
  , ou::tf::Trade::volume_t& volPutBuy,  ou::tf::Trade::volume_t& volPutSell
  );

  ou::ChartDataView* GetDataViewChart() { return &m_dvChart; }

  void SaveWatch( const std::string& sPrefix );

protected:
private:

  bool m_bActive;
  pOption_t m_pOption;

  ou::tf::Trade::volume_t m_volCallBuy;
  ou::tf::Trade::volume_t m_volCallSell;
  ou::tf::Trade::volume_t m_volPutBuy;
  ou::tf::Trade::volume_t m_volPutSell;

  ou::ChartEntryShape& m_ceBullCall;
  ou::ChartEntryShape& m_ceBullPut;
  ou::ChartEntryShape& m_ceBearCall;
  ou::ChartEntryShape& m_ceBearPut;

  ou::ChartEntryIndicator m_ceTrade;

  ou::ChartEntryVolume m_ceVolumeUp;
  ou::ChartEntryVolume m_ceVolumeDn;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;

  ou::ChartEntryIndicator m_ceSpread;

  ou::ChartDataView m_dvChart; // the data

  void Add();
  void Del();

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTradeCall( const ou::tf::Trade& trade );
  void HandleTradePut( const ou::tf::Trade& trade );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
