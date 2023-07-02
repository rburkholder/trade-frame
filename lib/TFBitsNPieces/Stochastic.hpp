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
 * File:    Stochastic.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFBitsNPieces
 * Created: 2022/11/29 12:40:23
 */

#pragma once

#include <string>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFIndicators/TSSWStochastic.h>

class Stochastic {
public:

  Stochastic( const std::string sIx, ou::tf::Quotes& quotes, int nPeriods, time_duration td, ou::Colour::EColour colour );
  Stochastic( const Stochastic& ) = delete;
  Stochastic( Stochastic&& rhs ) = delete;

  ~Stochastic();

  void AddToView( ou::ChartDataView& cdv, size_t price, size_t stoch );

  double MaxLatest() const { return m_pIndicatorStochastic->Max(); }
  const std::string& MaxName() const { return m_ceStochasticMax.GetName(); }

  double Latest() const { return m_pIndicatorStochastic->K(); }

  double MinLatest() const { return m_pIndicatorStochastic->Min(); }
  const std::string& MinName() const { return m_ceStochasticMin.GetName(); }

  void RemoveFrmView( ou::ChartDataView& cdv, size_t price, size_t stoch );

protected:
private:

  using pTSSWStochastic_t = std::unique_ptr<ou::tf::TSSWStochastic>;

  pTSSWStochastic_t m_pIndicatorStochastic;
  ou::ChartEntryIndicator m_ceStochastic;
  ou::ChartEntryIndicator m_ceStochasticMax;
  ou::ChartEntryIndicator m_ceStochasticMin;

};

