/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    StrategyEquityOption.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: March 11, 2034  10:58:03
 */

#pragma once

#include <TFOptions/Chain.h>
#include <TFOptions/Chains.h>
#include <TFOptions/Option.h>
#include <TFOptions/NoRiskInterestRateSeries.h>

#include "StrategyBase.hpp"

namespace ou {
namespace tf {
namespace iqfeed {
  class OptionChainQuery;
}
namespace option {
  class Engine;
}
}
}

namespace Strategy {

class EquityOption: public Base {
public:

  using pPosition_t = ou::tf::Position::pPosition_t;

  using pInstrument_t = ou::tf::Position::pInstrument_t;

  using fConstructedInstrument_t = std::function<void(pInstrument_t)>;
  using fBuildInstrument_t = std::function<void(const std::string&,fConstructedInstrument_t&&)>;

  using pOption_t = ou::tf::option::Option::pOption_t;
  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t = std::function<void(pInstrument_t,fConstructedOption_t&&)>;

  EquityOption(
    const ou::tf::config::symbol_t&
  , ou::tf::TreeItem* pTreeItem
  , fBuildInstrument_t&&
  , fConstructOption_t&&
  );
  virtual ~EquityOption();

  virtual void SetPosition( pPosition_t );
  virtual void SaveWatch( const std::string& sPrefix );

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  std::unique_ptr<ou::tf::iqfeed::OptionChainQuery> m_pOptionChainQuery; // need to disconnect

  fBuildInstrument_t m_fBuildInstrument;
  fConstructOption_t m_fConstructOption;

  struct BuiltOption: public ou::tf::option::chain::OptionName {
    pOption_t pOption;
  };

  using chain_t = ou::tf::option::Chain<BuiltOption>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

  //ou::tf::LiborFromIQFeed m_libor;
  ou::tf::FedRateFromIQFeed m_fedrate;
  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );
};

} // namespace Strategy

