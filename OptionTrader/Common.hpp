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
 * File:    Common.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 20, 2025 21:41:22
 */

#pragma once

#include <TFOptions/Chain.h>
#include <TFOptions/Option.h>
#include <TFOptions/Engine.h>

enum EChainColums { c_oi, c_iv, c_dlt, c_bid, c_ask, strike, p_bid, p_ask, p_dlt, p_iv, p_oi };

using pInstrument_t = ou::tf::Instrument::pInstrument_t;
using pOption_t = ou::tf::option::Option::pOption_t;

struct Instance: public ou::tf::option::chain::OptionName {
  pInstrument_t pInstrument; // resident in all Options
  pOption_t pOption;  // note, includes Watch, just-in-time Watch/Option construction
};

using chain_t = ou::tf::option::Chain<Instance>;
using mapChains_t = std::map<boost::gregorian::date, chain_t>;

