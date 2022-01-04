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
 * File:    AcquireFundamentals.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on September 18, 2021, 14:21
 */

 #include "AcquireFundamentals.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

AcquireFundamentals::AcquireFundamentals( pWatch_t&& pWatch_, fDone_t&& fDone_ )
: pWatch( std::move( pWatch_ ) ), fDone( std::move( fDone_ ) ) {
  assert( ou::tf::keytypes::EProviderIQF == pWatch->GetProvider()->ID() );
  //std::cout << "AcquireFundamentals::AcquireFundamentals(): " << pWatch->GetInstrumentName() << std::endl;
}

AcquireFundamentals::~AcquireFundamentals() {
    //std::cout << "AcquireFundamentals::~AcquireFundamentals(): " << pWatch->GetInstrumentName() << std::endl;
  }

void AcquireFundamentals::Start() {
  //std::cout << "AcquireFundamentals::Start(): " << pWatch->GetInstrumentName() << std::endl;
  pWatch->OnFundamentals.Add( MakeDelegate( this, &AcquireFundamentals::HandleFundamentals) );
  pWatch->OnTrade.Add( MakeDelegate( this, &AcquireFundamentals::HandleTrade ) );
  pWatch->StartWatch();
}

void AcquireFundamentals::HandleFundamentals( const ou::tf::Watch::Fundamentals& fundamentals ) {
  // the watch will retain variables from the fundamentals message
  //std::cout << "AcquireFundamentals::HandleFundamentals(): " << pWatch->GetInstrumentName() << std::endl;
  pWatch->StopWatch();
  pWatch->OnFundamentals.Remove( MakeDelegate( this, &AcquireFundamentals::HandleFundamentals) );
  pWatch->OnTrade.Remove( MakeDelegate(this, &AcquireFundamentals::HandleTrade ) );
  fDone( pWatch );  // fundamentals reside in watch
}

void AcquireFundamentals::HandleTrade( const ou::tf::Trade& trade ) {
  // a watch is required in order to obtain the fundamental
  // no action required, just a placeholder
}

} // namespace tf
} // namespace ou
