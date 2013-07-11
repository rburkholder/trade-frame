/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// Started 2012/07/10

#pragma once

#include <map>

#include <boost/smart_ptr.hpp>

#include <TFTrading/Watch.h>
#include "Strike.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class Bundle {
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  Bundle(void);
  ~Bundle(void);

  void SetUnderlying( pInstrument_t pInstrument, pProvider_t pProvider );
  void SetCall( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );
  void SetPut( pInstrument_t pInstrument, pProvider_t pDataProvider, pProvider_t pGreekProvider );

  void SetWatchOn( void );
  void SetWatchOff( void );

  void SaveSeries( const std::string& sPrefix );

protected:
private:

  typedef std::map<double,Strike> mapStrikes_t;
  typedef std::pair<double,Strike> mapStrikes_pair_t;

  bool m_bWatching;  // single threadable only

  boost::shared_ptr<ou::tf::Watch> m_pwatchUnderlying;
  mapStrikes_t m_mapStrikes;

  mapStrikes_t::iterator FindStrike( double strike );

};

} // namespace option
} // namespace tf
} // namespace ou

