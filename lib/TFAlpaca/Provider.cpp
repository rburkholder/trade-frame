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
 * File:    Provider.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFAlpaca
 * Created: June 5, 2022 16:04
 */

#include "Provider.hpp"

namespace ou {
namespace tf {
namespace alpaca {

Provider::Provider()
: ProviderInterface<Provider,Asset>()
{
  m_sName = "Alpaca";
  m_nID = keytypes::EProviderAlpaca;
  m_pProvidesBrokerInterface = true;
}

Provider::~Provider() {
}

void Provider::Connect() {
}

Provider::pSymbol_t Provider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new Asset( pInstrument->GetInstrumentName( ID() ), pInstrument ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}


} // namespace alpaca
} // namespace tf
} // namespace ou
