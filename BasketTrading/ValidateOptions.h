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
#include <TFOptions/Leg.h>
#include <TFOptions/Exceptions.h>

#include "LegSelected.h"

class ValidateOptions {
public:

  using vLegSelected_t = LegSelected::vLegSelected_t;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;

  using pOption_t = ou::tf::option::Option::pOption_t;
  using vOption_t = std::vector<pOption_t>;

  using Chain = ou::tf::option::Chain;
  using mapChains_t = std::map<boost::gregorian::date, Chain>;

  using fChooseStrikes_t
    = std::function<void(vLegSelected_t&, const mapChains_t&, boost::gregorian::date, double)>; // double is current price

  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t = std::function<void(const std::string&, const pInstrument_t, fConstructedOption_t)>;  // source from IQFeed Symbol Name

  using fValidatedOption_t = std::function<void(pOption_t)>;

  ValidateOptions(
    pWatch_t,  // underlying
    const mapChains_t&,
    fConstructOption_t&
    );
  ValidateOptions( const ValidateOptions& );
  ValidateOptions( const ValidateOptions&& );
  ~ValidateOptions( );

  bool ValidateSpread( boost::gregorian::date, double price, size_t nDuration, fChooseStrikes_t&& );
  void ValidatedOptions( fValidatedOption_t&& );
  void ClearValidation();

private:

  pWatch_t m_pWatchUnderlying;

  const mapChains_t& m_mapChains;

  vLegSelected_t m_vLegSelected;

  fConstructOption_t& m_fConstructOption;

  ou::tf::SpreadValidation m_SpreadValidation;

};

#endif /* VALIDATEOPTIONS_H */

