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
 * File:    Collect.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 18, 2025 22:04/38
 */

#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include "Collect.hpp"

namespace collect {

void Base::SetAttributes( ou::tf::HDF5Attributes& attr, pWatch_t pWatch ) {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t pInstrument( pWatch->GetInstrument() );

  const ou::tf::InstrumentType::EInstrumentType type( pInstrument->GetInstrumentType() );
  attr.SetInstrumentType( type );

  attr.SetProviderType( pWatch->GetProvider()->ID() );

  attr.SetMultiplier( pInstrument->GetMultiplier() );
  attr.SetSignificantDigits( pInstrument->GetSignificantDigits() );

  switch ( type ) {
    case ou::tf::InstrumentType::Future: {
        const ou::tf::HDF5Attributes::structFuture attributes(
          pInstrument->GetExpiryYear(),
          pInstrument->GetExpiryMonth(),
          pInstrument->GetExpiryDay()
        );
        attr.SetFutureAttributes( attributes );
      }
      break;
    case ou::tf::InstrumentType::Option: {
        const ou::tf::HDF5Attributes::structOption attributes(
          pInstrument->GetStrike(),
          pInstrument->GetExpiryYear(),
          pInstrument->GetExpiryMonth(),
          pInstrument->GetExpiryDay(),
          pInstrument->GetOptionSide()
        );
        attr.SetOptionAttributes( attributes );
      }
      break;
    default:
      assert( true ); // todo: validate
      break;
  }

}

} // namespace collect
