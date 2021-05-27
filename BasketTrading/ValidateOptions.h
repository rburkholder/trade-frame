/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    ValidateOptions.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on July 4, 2019, 12:10 PM
 */

#ifndef VALIDATEOPTIONS_H
#define VALIDATEOPTIONS_H

#include <vector>

#include <TFTrading/SpreadValidation.h>

#include <TFOptions/Chain.h>

#include <TFOptionCombos/Leg.h>
#include <TFOptionCombos/Exceptions.h>

#include "LegSelected.h"

class ValidateOptions {
public:

  using vLegSelected_t = std::vector<LegSelected>;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;

  using pOption_t = ou::tf::option::Option::pOption_t;
  using vOption_t = std::vector<pOption_t>;

  using chain_t = ou::tf::option::Chain<ou::tf::option::chain::Option>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;

  using fStrikeSelected_t
    = std::function<void(double,double, boost::gregorian::date, const std::string&)>;
  using fChooseLegs_t
    = std::function<void(const mapChains_t&, boost::gregorian::date, double, fStrikeSelected_t&&)>; // double is current price

  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t = std::function<void(const std::string&, const pInstrument_t, fConstructedOption_t&&)>;  // source from IQFeed Symbol Name

  using fValidatedOption_t = std::function<void(size_t,pOption_t)>; // size_t is ix into combo vLeg

  ValidateOptions(
    pWatch_t,  // underlying
    const mapChains_t&,
    fConstructOption_t&
    );
  ValidateOptions( const ValidateOptions& );
  ValidateOptions( const ValidateOptions&& );
  ~ValidateOptions( );

  void SetSize( vLegSelected_t::size_type );

  bool ValidateBidAsk( boost::gregorian::date, double priceUnderlying, size_t nDuration, fChooseLegs_t&& );
  void ValidatedOptions( fValidatedOption_t&& );
  void ClearValidation();

private:

  enum class EState {
    WaitForSize, FirstTime, FindStrikes,
    BuildOptions, WaitForBuildCompletion,
    Validate
    };
  EState m_state;

  pWatch_t m_pWatchUnderlying;

  const mapChains_t& m_mapChains;

  fConstructOption_t& m_fConstructOption;

  // track leg selection based upon underlying price
  vLegSelected_t m_vLegSelected;
  // when legs change, revisit validation
  ou::tf::SpreadValidation m_SpreadValidation;

};

#endif /* VALIDATEOPTIONS_H */

