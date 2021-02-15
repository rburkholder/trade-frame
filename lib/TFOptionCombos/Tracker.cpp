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
 * Created: Novemeber 8, 2020, 11:41 AM
 */

 #include "Tracker.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace {
  bool lt( double a, double b ) { return a < b; }
  bool gt( double a, double b ) { return a > b; }
  bool eq( double a, double b ) { return a == b; }
  bool ge( double a, double b ) { return a >= b; }
  bool le( double a, double b ) { return a <= b; }
}

Tracker::Tracker()
: m_transition( ETransition::Initial ),
  m_pChain( nullptr ),
  m_compare( nullptr ),
  m_luStrike( nullptr ),
  m_dblUnderlyingSlope {}, m_dblUnderlyingPrice {}
{}

Tracker::Tracker( Tracker&& rhs )
: m_compare( std::move( rhs.m_compare ) ),
  m_luStrike( std::move( rhs.m_luStrike ) ),
  m_dblStrikeWatch( rhs.m_dblStrikeWatch ),
  m_sideWatch( rhs.m_sideWatch ),
  m_dblUnderlyingPrice( rhs.m_dblUnderlyingPrice ),
  m_dblUnderlyingSlope( rhs.m_dblUnderlyingSlope ),
  m_transition( rhs.m_transition ),
  m_pChain( std::move( rhs.m_pChain ) ),
  m_pPosition( std::move( rhs.m_pPosition ) ),
  m_pOption( std::move( rhs.m_pOption ) ),
  m_fConstructOption( std::move( rhs.m_fConstructOption ) ),
  m_fRoll( std::move( rhs.m_fRoll ) )
{}

Tracker::~Tracker() {
  if ( m_pOption ) {
    m_pOption->StopWatch();
    m_pOption->OnQuote.Remove( MakeDelegate( this, &Tracker::HandleOptionQuote ) );
    m_pOption.reset();
  }
  m_pPosition.reset();
}

void Tracker::Initialize(
  pPosition_t pPosition,
  const ou::tf::option::Chain* pChain,
  fConstructOption_t&& fConstructOption,
  fRoll_t&& fRoll
) {

  assert( ETransition::Initial == m_transition );

  m_pChain = pChain;

  m_fConstructOption = std::move( fConstructOption );
  m_fRoll = std::move( fRoll );

  Initialize( pPosition );

  m_transition = ETransition::Track;
}

void Tracker::Initialize( pPosition_t pPosition ) {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  m_pPosition = pPosition;

  pInstrument_t pInstrument = m_pPosition->GetWatch()->GetInstrument();
  assert( pInstrument->IsOption() );

  m_dblStrikeWatch = pInstrument->GetStrike();
  m_sideWatch = pInstrument->GetOptionSide();

  switch ( pInstrument->GetOptionSide() ) {
    case ou::tf::OptionSide::Call:
      m_compare = &gt;
      m_luStrike = [pChain=m_pChain](double dblUnderlying){ return pChain->Call_Itm( dblUnderlying ); };
      break;
    case ou::tf::OptionSide::Put:
      m_compare = &lt;
      m_luStrike = [pChain=m_pChain](double dblUnderlying){ return pChain->Put_Itm( dblUnderlying ); };
      break;
  }
  assert( m_compare );

}

void Tracker::TestLong( double dblUnderlyingSlope, double dblUnderlyingPrice ) {

  switch ( m_transition ) {
    case ETransition::Track:
      {
        m_dblUnderlyingPrice = dblUnderlyingPrice;
        m_dblUnderlyingSlope = dblUnderlyingSlope;

        double strikeItm = m_luStrike( dblUnderlyingPrice );

        if ( m_compare( strikeItm, m_dblStrikeWatch ) ) { // is new strike further itm?
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
  switch ( m_sideWatch ) {
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
          // positive for long call, negative for long put
        }
        else {
          // test if roll will be profitable for long option
          //double diff = m_pPosition->GetUnRealizedPL() - quote.Ask();  // buy new at the ask
          double diff = m_pPosition->GetUnRealizedPL();
          diff -= quote.Spread();  // subtract exit spread for extra margin?
          diff -= 0.10;  // subtract commissions and such plus some spare change
          if ( 0.10 < diff ) { // desire at least 10 cents on the roll
            std::cout
              << quote.DateTime().time_of_day() << " "
              << m_pOption->GetInstrument()->GetInstrumentName()
              << " roll on diff=" << diff
              << ",underlying=" << m_dblUnderlyingPrice
              << ",slope=" << m_dblUnderlyingSlope
              << std::endl;
            m_transition = ETransition::Roll;
            m_pOption->StopWatch();
            m_pOption->OnQuote.Remove( MakeDelegate( this, &Tracker::HandleOptionQuote ) );
            m_compare = nullptr;
            m_luStrike = nullptr;
            pOption_t pOption( std::move( m_pOption ) );
            Initialize( m_fRoll( m_pPosition, std::move( pOption ) ) );
            m_transition = ETransition::Track;  // start all over again
          }
        }
      }
      break;
  }
}

} // namespace option
} // namespace tf
} // namespace ou
