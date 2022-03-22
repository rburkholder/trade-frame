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
 * File:    Indicators.hpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: March 22, 2022 11:01
 */

#pragma once

namespace ou {
  class ChartEntryShape;
}

struct Indicators {

  ou::ChartEntryShape& ceBuySubmit;
  ou::ChartEntryShape& ceBuyFill;
  ou::ChartEntryShape& ceSellSubmit;
  ou::ChartEntryShape& ceSellFill;
  ou::ChartEntryShape& ceCancelled;

  Indicators(
    ou::ChartEntryShape& ceBuySubmit_
  , ou::ChartEntryShape& ceBuyFill_
  , ou::ChartEntryShape& ceSellSubmit_
  , ou::ChartEntryShape& ceSellFill_
  , ou::ChartEntryShape& ceCancelled_
  )
  : ceBuySubmit( ceBuySubmit_ ), ceBuyFill( ceBuyFill_ )
  , ceSellSubmit( ceSellSubmit_ ), ceSellFill( ceSellFill_ )
  , ceCancelled( ceCancelled_ )
  {}
};

