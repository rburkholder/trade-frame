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
 * File:    Flags.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: 2025 August 25 10:15:40
 */

#pragma once

struct Flags {
  bool bEnableBidAskPrice;
  bool bEnableBidAskVolume;
  bool bEnableImbalance;
  bool bEnablePrediction;
  bool bEnableAdvDec;

  Flags()
  : bEnableBidAskPrice( false )
  , bEnableBidAskVolume( false )
  , bEnableImbalance( false )
  , bEnablePrediction( false )
  , bEnableAdvDec( false )
  {}
};