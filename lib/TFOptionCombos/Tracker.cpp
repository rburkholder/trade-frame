/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * File:    Tracker.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on Novemeber 8, 2020, 11:41 AM
 */

 #include "Tracker.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace {

  bool lt( double a, double b ) { return a < b; }
  bool gt( double a, double b ) { return a > b; }
  bool eq( double a, double b ) { return a == b; }
}

Tracker::Tracker()
: m_transition( ETransition::Initial ),
  m_pChain( nullptr ),
  m_compare( nullptr ),
  m_luStrike( nullptr ),
  m_dblUnderlyingSlope {}, m_dblUnderlying {}
{}

Tracker::~Tracker() {
  if ( m_pOption ) {
    m_pOption->StopWatch();
    m_pOption->OnQuote.Remove( MakeDelegate( this, &Tracker::HandleOptionQuote ) );
    m_pOption.reset();
    m_pPosition.reset();
    m_pWatch.reset();
  }
}

void Tracker::Initialize(
  pPosition_t pPosition,
  const ou::tf::option::Chain* pChain,
  fConstructOption_t&& fConstructOption,
  fRoll_t&& fRoll
) {

  assert( ETransition::Initial == m_transition );

  m_pPosition = pPosition;
  m_pChain = pChain;

  m_fConstructOption = std::move( fConstructOption );
  m_fRoll = std::move( fRoll );

  m_pWatch = m_pPosition->GetWatch();
  pInstrument_t pInstrument = m_pWatch->GetInstrument();
  assert( pInstrument->IsOption() );

  m_dblStrikePosition = pInstrument->GetStrike();
  m_sidePosition = pInstrument->GetOptionSide();

  switch ( pInstrument->GetOptionSide() ) {
    case ou::tf::OptionSide::Call:
      m_compare = &gt;
      m_luStrike = [this](double dblUnderlying){ return m_pChain->Call_Itm( dblUnderlying ); };
      break;
    case ou::tf::OptionSide::Put:
      m_compare = &lt;
      m_luStrike = [this](double dblUnderlying){ return m_pChain->Put_Itm( dblUnderlying ); };
      break;
  }
  assert( m_compare );

  m_transition = ETransition::Track;
}

void Tracker::TestLong( double dblUnderlyingSlope, double dblUnderlying ) {

  switch ( m_transition ) {
    case ETransition::Track:
      {
        m_dblUnderlying      = dblUnderlying;
        m_dblUnderlyingSlope = dblUnderlyingSlope;

        double strikeItm = m_luStrike( dblUnderlying );

        if ( m_compare( strikeItm, m_dblStrikePosition ) ) { // is new strike further itm?
          if ( m_pOption ) { // if already tracking the option
            if ( m_compare( strikeItm, m_pOption->GetStrike() ) ) { // move further itm?
              m_transition = ETransition::Vacant;
              m_pOption->StopWatch();
              m_pOption->OnQuote.Remove( MakeDelegate( this, &Tracker::HandleOptionQuote ) );
              m_pOption.reset();
              Construct( strikeItm );
            }
            else {
              // TODO: if retreating, stay pat, retreat, or try the roll?
              // nothing to do, track in existing option as quotes are updated
            }
          }
          else {
            // need to obtain option, but track via state machine to request only once
            m_transition = ETransition::Vacant;
            Construct( strikeItm );
          }
        }
        else {
          // nothing to do, hasn't moved enough itm
        }
      }
      break;
  }

}

void Tracker::Construct( double strikeItm ) {
  std::string sName;
  switch ( m_sidePosition ) {
    case ou::tf::OptionSide::Call:
      sName = m_pChain->GetIQFeedNameCall( strikeItm );
      break;
    case ou::tf::OptionSide::Put:
      sName = m_pChain->GetIQFeedNamePut( strikeItm );
      break;
  }
  std::cout << "Tracker::Construct: " << sName << std::endl;
  m_transition = ETransition::Acquire;
  m_fConstructOption(
    sName,
    [this]( pOption_t pOption ){
      m_pOption = pOption;
      m_pOption->OnQuote.Add( MakeDelegate( this, &Tracker::HandleOptionQuote ) );
      m_pOption->StartWatch();
      m_transition = ETransition::Track;
    } );
}

void Tracker::HandleOptionQuote( const ou::tf::Quote& quote ) {
  switch ( m_transition ) {
    case ETransition::Initial:
      break;
    case ETransition::Vacant:
      break;
    case ETransition::Fill:
      break;
    case ETransition::Acquire:
      break;
    case ETransition::Track:
      {
        if ( m_compare( 0.0, m_dblUnderlyingSlope ) ) {
          // nothing if the slope is going in the right direction
        }
        else {
          const ou::tf::Quote& watch( m_pWatch->LastQuote() );
          double diff = watch.Bid() - quote.Ask();  // sell existing at bid, buy new at the ask
          diff -= ( watch.Spread() + quote.Spread() ) / 2.0;  // entry spread
          diff -= 0.10;  // commissions and such
          if ( 0.10 < diff ) { // make at least 10 cents on the roll
            std::cout
              << quote.DateTime().time_of_day() << " "
              << m_pOption->GetInstrument()->GetInstrumentName()
              << " roll on diff=" << diff
              << ",underlying=" << m_dblUnderlying
              << ",slope=" << m_dblUnderlyingSlope
              << std::endl;
            m_transition = ETransition::Roll;
            m_pOption->StopWatch();
            m_pOption->OnQuote.Remove( MakeDelegate( this, &Tracker::HandleOptionQuote ) );
            m_fRoll( m_pOption );
            m_pOption.reset();
            m_pWatch.reset();
            m_pPosition.reset();
            m_transition = ETransition::Initial;  // start all over again
          }
        }
      }
      break;
  }

}


} // namespace option
} // namespace tf
} // namespace ou
