/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

#include <string>

#include <TFIndicators/TSSWStats.h>
#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified

class ChartEntryMark;

namespace tf { // TradeFrame



class BollingerTransitions {
public:

  BollingerTransitions();

  void ReadDailyBars( const std::string& sPath, ou::ChartEntryMark& cePivots );
  void Crossing( double price );

protected:
private:

  enum class EBollXing { None, Lower, Mean, Upper, _Count };
  enum EBollingerState { Unknown, BelowLower, MeanToLower, MeanToUpper, AboveUpper, _Count };
  EBollingerState m_stateBollinger;

  double m_dblBollingerUpper;
  double m_dblBollingerMean;
  double m_dblBollingerLower;

  ou::tf::Prices m_pricesDailyClose;
  ou::tf::TSSWStatsPrice m_pricesDailyCloseBollinger20;

};


} // namespace tf
} // namespace ou
