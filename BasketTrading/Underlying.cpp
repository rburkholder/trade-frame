/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Underlying.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on 2021/06/19 19:41
 */

#include "Underlying.h"

Underlying::Underlying(
  pWatch_t pWatchUnderlying_,
  const std::string& sDailyBarPath
)
: pWatchUnderlying( pWatchUnderlying_ ),
  GexCalc( pWatchUnderlying_ ),
  m_sDailyBarPath( sDailyBarPath )
{
  assert( pWatchUnderlying_ );
  m_pChartDataView = std::make_shared<ou::ChartDataView>();
  //BollingerTransitions::ReadDailyBars( m_sDailyBarPath, m_cePivots ); // TODO: enable this for visual clues
}

// TODO: don't really need this anymore, placeholder for real code
void Underlying::BuildUnderlyingChains( gex_t& gex ) {
  //gex.LoadChains( m_fOptionNamesByUnderlying );
}

