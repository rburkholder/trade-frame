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

AcquireFundamentals::AcquireFundamentals( pWatch_t&& pWatch, fDone_t&& fDone )
: m_pWatch( std::move( pWatch ) ), m_fDone( std::move( fDone ) ) {
  assert( ou::tf::keytypes::EProviderIQF == m_pWatch->GetProvider()->ID() );
  //std::cout << "AcquireFundamentals::AcquireFundamentals(): " << m_pWatch->GetInstrumentName() << std::endl;
}

AcquireFundamentals::~AcquireFundamentals() {
  //std::cout << "AcquireFundamentals::~AcquireFundamentals(): " << m_pWatch->GetInstrumentName() << std::endl;
}

void AcquireFundamentals::Start() {
  //std::cout << "AcquireFundamentals::Start(): " << m_pWatch->GetInstrumentName() << std::endl;
  m_pWatch->OnFundamentals.Add( MakeDelegate( this, &AcquireFundamentals::HandleFundamentals) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &AcquireFundamentals::HandleTrade ) );
  m_pWatch->OnSummary.Add( MakeDelegate( this, &AcquireFundamentals::HandleSummary ) );
  m_pWatch->StartWatch();
}

void AcquireFundamentals::HandleFundamentals( const ou::tf::Watch::Fundamentals& fundamentals ) {
  // the watch will retain variables from the fundamentals message
  //std::cout << "AcquireFundamentals::HandleFundamentals() enter: " << m_pWatch->GetInstrumentName()  << std::endl;
  //m_pWatch->StopWatch();
  //m_pWatch->OnSummary.Remove( MakeDelegate( this, &AcquireFundamentals::HandleSummary ) );
  //m_pWatch->OnTrade.Remove( MakeDelegate(this, &AcquireFundamentals::HandleTrade ) );
  //m_pWatch->OnFundamentals.Remove( MakeDelegate( this, &AcquireFundamentals::HandleFundamentals) );
  //m_fDone( m_pWatch );  // fundamentals reside in watch
  //std::cout << "AcquireFundamentals::HandleFundamentals() exit: " << m_pWatch->GetInstrumentName() << std::endl;
}

void AcquireFundamentals::HandleSummary( const ou::tf::Watch::Summary& summary ) {
  // summary comes after fundamentals, so this should ensure both are obtained
  //std::cout << "AcquireFundamentals::HandleSummary() enter: " << m_pWatch->GetInstrumentName()  << std::endl;
  m_pWatch->StopWatch();
  m_pWatch->OnSummary.Remove( MakeDelegate( this, &AcquireFundamentals::HandleSummary ) );
  m_pWatch->OnTrade.Remove( MakeDelegate(this, &AcquireFundamentals::HandleTrade ) );
  m_pWatch->OnFundamentals.Remove( MakeDelegate( this, &AcquireFundamentals::HandleFundamentals) );
  m_fDone( m_pWatch );  // fundamentals reside in watch
  //std::cout << "AcquireFundamentals::HandleSummary() exit: " << m_pWatch->GetInstrumentName() << std::endl;
}

void AcquireFundamentals::HandleTrade( const ou::tf::Trade& trade ) {
  // a watch is required in order to obtain the fundamental
  // no action required, just a placeholder
}

} // namespace tf
} // namespace ou
