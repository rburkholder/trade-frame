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
 * File:    Stochastic.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFBitsNPieces
 * Created: 2022/11/29 12:40:23
 */

#include "Stochastic.hpp"

Stochastic::Stochastic( const std::string sIx, ou::tf::Quotes& quotes, int nPeriods, time_duration td, ou::Colour::EColour colour ) {

  m_ceStochastic.SetColour( colour );
  m_ceStochasticMax.SetColour( colour );
  m_ceStochasticMin.SetColour( colour );

  m_ceStochastic.SetName( "k" + sIx );
  m_ceStochasticMax.SetName( "k" + sIx + "u" );
  m_ceStochasticMin.SetName( "k" + sIx + "l" );

  m_pIndicatorStochastic = std::make_unique<ou::tf::TSSWStochastic>(
    quotes, nPeriods, td,
    [this,sIx]( ptime dt, double k, double min, double max ){
      //std::cout << sIx << " is " << k << "," << max << "," << min << std::endl;
      m_ceStochasticMax.Append( dt, max );
      m_ceStochasticMin.Append( dt, min );
      m_ceStochastic.Append( dt, k ); // resides on top of min/max
    }
  );
}

void Stochastic::AddToView( ou::ChartDataView& cdv, size_t price, size_t stoch ) {
  cdv.Add( price, &m_ceStochasticMax );
  cdv.Add( price, &m_ceStochasticMin );
  cdv.Add( stoch, &m_ceStochastic );
}

Stochastic::~Stochastic() {
  m_pIndicatorStochastic.reset();
  m_ceStochastic.Clear();
  m_ceStochasticMax.Clear();
  m_ceStochasticMin.Clear();
}

