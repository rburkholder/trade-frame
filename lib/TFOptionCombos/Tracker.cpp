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

// 2023/02/22 TestShort has todo for rolling for premium

#include <boost/log/trivial.hpp>

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
: m_transition( ETransition::Initial )
, m_bLock( false )
, m_compare( nullptr )
, m_luItmStrike( nullptr )
, m_luItmName( nullptr )
, m_pChain( nullptr )
, m_dblUnderlyingSlope {}, m_dblUnderlyingPrice {}
, m_fOptionRoll_Construct( nullptr )
, m_fOptionRoll_Open( nullptr )
{}

Tracker::Tracker( Tracker&& rhs )
: m_transition( rhs.m_transition )
, m_bLock( false )
, m_compare( std::move( rhs.m_compare ) )
, m_luItmStrike( std::move( rhs.m_luItmStrike ) )
, m_luItmName( std::move( rhs.m_luItmName ) )
, m_pChain( std::move( rhs.m_pChain ) )
, m_dblStrikePosition( rhs.m_dblStrikePosition )
, m_sidePosition( rhs.m_sidePosition )
, m_dblUnderlyingPrice( rhs.m_dblUnderlyingPrice )
, m_dblUnderlyingSlope( rhs.m_dblUnderlyingSlope )
, m_pPosition( std::move( rhs.m_pPosition ) )
, m_pOptionCandidate( std::move( rhs.m_pOptionCandidate ) )
, m_fConstructOption( std::move( rhs.m_fConstructOption ) )
, m_fOpenLeg( std::move( rhs.m_fOpenLeg ) )
, m_fCloseLeg( std::move( rhs.m_fCloseLeg ) )
, m_fOptionRoll_Construct( std::move( rhs.m_fOptionRoll_Construct ) )
, m_fOptionRoll_Open( std::move( rhs.m_fOptionRoll_Open ) )
{
  assert( !m_pOptionCandidate );  // can't be watching
}

Tracker::~Tracker() {
  Quiesce();
  m_transition = ETransition::Done;
  m_compare = nullptr;
  m_luItmStrike = nullptr;
  m_luItmName = nullptr;
  m_pPosition.reset();
  m_fConstructOption = nullptr;
  m_fOpenLeg = nullptr;
  m_fCloseLeg = nullptr;
  m_fOptionRoll_Construct = nullptr;
  m_fOptionRoll_Open = nullptr;
}

void Tracker::Initialize(
  pPosition_t pPosition,
  const chain_t* pChain,
  fConstructOption_t&& fConstructOption,
  fCloseLeg_t&& fCloseLeg,
  fOpenLeg_t&& fOpenLeg
) {

  //BOOST_LOG_TRIVIAL(info) << "Tracker::Initialize,external";

  assert( pPosition );
  assert( fConstructOption );
  assert( fCloseLeg );
  assert( fOpenLeg );
  assert( ETransition::Initial == m_transition );

  m_pChain = pChain;

  m_fConstructOption = std::move( fConstructOption );
  m_fCloseLeg = std::move( fCloseLeg );
  m_fOpenLeg = std::move( fOpenLeg );

  Initialize( pPosition );

}

void Tracker::Initialize( pPosition_t pPosition ) {

  //BOOST_LOG_TRIVIAL(info) << "Tracker::Initialize,internal";

  assert( ETransition::Initial == m_transition );  // needs to be set prior to entry

  m_pPosition = std::move( pPosition );

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  pInstrument_t pInstrument = m_pPosition->GetInstrument();
  assert( pInstrument->IsOption() || pInstrument->IsFuturesOption() );

  m_dblStrikePosition = pInstrument->GetStrike();
  m_sidePosition = pInstrument->GetOptionSide();

  switch ( m_sidePosition ) {
    case ou::tf::OptionSide::Call:
      m_compare = &gt;
      m_luItmStrike = [pChain=m_pChain](double dblUnderlying){ return pChain->Call_Itm( dblUnderlying ); };
      m_luItmName =   [pChain=m_pChain](double strike) { return pChain->GetIQFeedNameCall( strike ); };
      break;
    case ou::tf::OptionSide::Put:
      m_compare = &lt;
      m_luItmStrike = [pChain=m_pChain](double dblUnderlying){ return pChain->Put_Itm( dblUnderlying ); };
      m_luItmName =   [pChain=m_pChain](double strike) { return pChain->GetIQFeedNamePut( strike ); };
      break;
    default:
      assert( false );
  }
  assert( m_compare );

  m_transition = ETransition::Track;
}

void Tracker::TestLong( boost::posix_time::ptime dt, double dblUnderlyingSlope, double dblUnderlyingPrice ) {

  switch ( m_transition ) {
    case ETransition::Track:
      {
        m_dblUnderlyingPrice = dblUnderlyingPrice;
        m_dblUnderlyingSlope = dblUnderlyingSlope;

        double strikeItm = m_luItmStrike( dblUnderlyingPrice );

        if ( m_compare( strikeItm, m_dblStrikePosition ) ) { // is new strike further itm?
          // TODO: refactor to remove the if/else and put Vacant/Construct together?
          if ( m_pOptionCandidate ) { // if already tracking the option
            if ( m_compare( strikeItm, m_pOptionCandidate->GetStrike() ) ) { // move further itm?
              m_transition = ETransition::Vacant;
              OptionCandidate_StopWatch();
              m_pOptionCandidate.reset();
              OptionCandidate_Construct( dt, strikeItm );
            }
            else {
              // TODO: if retreating, stay pat, retreat, or try the roll?
              // nothing to do, track in existing option as quotes are updated
            }
          }
          else {
            // need to obtain option, but track via state machine to request only once
            m_transition = ETransition::Vacant;
            OptionCandidate_Construct( dt, strikeItm );
          }
        }
        else {
          // nothing to do, hasn't moved enough itm
        }
      }
      break;
    case ETransition::Roll_start:
      StartOptionRoll();
      break;
    default:
      break;
  }

}

void Tracker::TestShort( boost::posix_time::ptime dt, double dblUnderlyingSlope, double dblUnderlyingPrice ) {

  switch ( m_transition ) {
    case ETransition::Track:
      {
        m_dblUnderlyingPrice = dblUnderlyingPrice;
        m_dblUnderlyingSlope = dblUnderlyingSlope;

        double diff = m_pPosition->GetUnRealizedPL();

        // close out when value close to zero - or auto calendar roll?
        const ou::tf::Quote& quote( m_pPosition->GetWatch()->LastQuote() );
        if ( quote.IsNonZero() && ( quote.Ask() > quote.Bid() ) ) {
          if ( 0.401 >= quote.Ask() && ( 0.0 < quote.Bid() ) ) {
            // TODO: perform a calendar roll to regain premium? (but not on expiry date)
            m_transition = ETransition::Fill;
            m_fCloseLeg( m_pPosition );
            m_pPosition.reset();
            m_transition = ETransition::Initial;
          }
        }
      }
      break;
    case ETransition::Roll_start:
      StartOptionRoll();
    default:
      break;
  }
}

void Tracker::OptionCandidate_Construct( boost::posix_time::ptime dt, double strike ) {

  // TODO: need to enable greeks to track IV, Delta

  m_transition = ETransition::Acquire;

  std::string sName;
  assert( m_luItmName );
  sName = m_luItmName( strike );

  BOOST_LOG_TRIVIAL(info)
    << dt.time_of_day() << " "
    << "Tracker::Construct: "
    << sName
    << " at " << strike
    ;

  m_fConstructOption(
    sName,
    [this]( pOption_t pOption ){
      assert( !m_pOptionCandidate );
      m_pOptionCandidate = std::move( pOption );
      OptionCandidate_StartWatch();
      m_transition = ETransition::Track;
    } );
}

// called when m_pOptionCandidate exists (via OnQuote)
void Tracker::OptionCandidate_HandleQuote( const ou::tf::Quote& quote ) {

  switch ( m_transition ) {
    case ETransition::Track:
      {
        if ( m_compare( m_dblUnderlyingSlope, 0.0 ) ) {
          // nothing if the slope is going in the right direction
          // positive for long call, negative for long put
        }
        else {
          // test if roll will be profitable for long option
          //double diff = m_pPosition->GetUnRealizedPL() - quote.Ask();  // buy new at the ask
          double diff = m_pPosition->GetUnRealizedPL() /  // calc per share
            ( m_pPosition->GetActiveSize() * m_pPosition->GetInstrument()->GetMultiplier() );
          diff -= ( 2.0 * quote.Spread() );  // unrealized p/l incorporates entry spread, this calculates exit spread
          diff -= 0.10;  // subtract estimated commissions plus some spare change
          if ( 1.10 < diff ) { // minimum profit on the roll - but look at overall profit from all legs
            if ( ( 0 == quote.BidSize() ) || ( 0.0 == quote.Bid() ) || ( 0.0 == quote.Ask() )
            ) {
              // is un-buyable, or is grotesquely bad
            }
            else {
              if ( !m_bLock ) {
                assert( nullptr == m_fOptionRoll_Construct );
                assert( nullptr == m_fOptionRoll_Open );
                auto pOldWatch = m_pPosition->GetWatch();
                BOOST_LOG_TRIVIAL(info)
                  << quote.DateTime().time_of_day()
                  << ",roll"
                  << ",old=" << pOldWatch->GetInstrumentName()
                  << ",b=" << pOldWatch->LastQuote().Bid()
                  << ",a=" << pOldWatch->LastQuote().Ask()
                  << ",new=" << m_pOptionCandidate->GetInstrument()->GetInstrumentName()
                  << ",b=" << m_pOptionCandidate->LastQuote().Bid()
                  << ",a=" << m_pOptionCandidate->LastQuote().Ask()
                  << ",roll-per-share-diff=" << diff
                  << ",underlying=" << m_dblUnderlyingPrice
                  << ",slope=" << m_dblUnderlyingSlope
                  ;
                m_transition = ETransition::Roll_profit;
                OptionCandidate_StopWatch();
                m_compare = nullptr;
                m_luItmStrike = nullptr;
                m_luItmName = nullptr;
                pOption_t pOption( std::move( m_pOptionCandidate ) );
                std::string sNotes( m_pPosition->Notes() ); // notes are needed for new position creation
                m_fCloseLeg( m_pPosition );  // TODO: closer needs to use EnableStatsAdd
                m_pPosition.reset();
                // TODO: on opening a position, will need to extend states to handle order with errors
                m_transition = ETransition::Initial; // NEEDS to be here, prior to m_fOpenLeg, which calls back in
                // NOTE: Initialize may not be required as m_fOpenLeg calls back into Tracker
                //    as such, there may be a double init going on
                //Initialize( m_fOpenLeg( std::move( pOption ), sNotes ) ); // with new position, NOTE: opener needs to use EnableStatsAdd
                m_fOpenLeg( std::move( pOption ), sNotes );
              }
            }
          }
        }
      }
      break;
    case ETransition::Roll_warmup:
      {
        assert( m_pOptionCandidate );
        ou::tf::Watch::tupleSpreadStats_t stats = m_pOptionCandidate->SpreadStats();

        if ( std::get<bool>( stats ) ) {
          assert( m_fOptionRoll_Open );
          m_fOptionRoll_Open();
          m_fOptionRoll_Open = nullptr;
        }
      }
      break;
    default:
      break;
  }
}

void Tracker::StartOptionRoll() {
  if ( m_fOptionRoll_Construct ) {

    if ( m_pOptionCandidate ) {
      OptionCandidate_StopWatch();
      m_pOptionCandidate.reset();
    }

    m_fOptionRoll_Construct();  // call the function
    m_fOptionRoll_Construct = nullptr;
  }
}

void Tracker::GenericRoll( double strike ) {
  if ( m_pPosition ) {
    if ( m_pPosition->IsActive() ) {

      m_transition = ETransition::Roll_init;

      assert( nullptr == m_fOptionRoll_Construct );
      m_fOptionRoll_Construct =  // for background loop
        [this,strike](){

          std::string sName_New;
          assert( m_luItmName );
          sName_New = m_luItmName( strike );

          ou::tf::OptionSide::EOptionSide sidePosition( m_sidePosition );

          std::string sNotes( m_pPosition->Notes() ); // notes are needed for new position creation
          std::string sName_Old( m_pPosition->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );

          m_compare = nullptr;
          m_luItmStrike = nullptr;
          m_luItmName = nullptr;

          m_fCloseLeg( m_pPosition );
          m_pPosition.reset();

          BOOST_LOG_TRIVIAL(info) << "Tracker::GenericRoll: " << sName_Old << " to " << sName_New;

          m_fConstructOption(
            sName_New,
            [this,sNotes_=std::move(sNotes)]( pOption_t pOption ){
              BOOST_LOG_TRIVIAL(info) << "Tracker::GenericRoll::m_fConstructOption" << " " << sNotes_;
              assert( nullptr == m_fOptionRoll_Open );
              m_fOptionRoll_Open =
                [this,sNotes__=std::move(sNotes_)](){
                  m_transition = ETransition::Initial; // NEEDS to be here, prior to m_fOpenLeg, which calls back in
                  // TODO: as above, may only need the m_fOpenLeg
                  //Initialize( m_fOpenLeg( std::move( pOption ), sNotes_ ) ); // with new position
                  OptionCandidate_StopWatch();
                  m_fOpenLeg( std::move( m_pOptionCandidate ), sNotes__ );
                };
              m_transition = ETransition::Roll_warmup;
              assert( !m_pOptionCandidate );
              m_pOptionCandidate = std::move( pOption );
              OptionCandidate_StartWatch();
            } );
        };

      m_transition = ETransition::Roll_start;

    }
  }
}

void Tracker::CalendarRoll() {
  assert( 0.0 < m_dblStrikePosition );
  GenericRoll( m_dblStrikePosition );
}

void Tracker::DiagonalRoll() {
  assert( m_luItmStrike );
  assert( 0.0 < m_dblUnderlyingPrice );
  double strike = m_luItmStrike( m_dblUnderlyingPrice );
  GenericRoll( strike );
}

void Tracker::Lock( bool bLock ) {
  BOOST_LOG_TRIVIAL(info) << "Tracker::Lock() not implemented";
  m_bLock = bLock;
}

void Tracker::Close() {
  BOOST_LOG_TRIVIAL(info) << "Tracker::Close() not implemented";
}

void Tracker::TestItmRoll( boost::gregorian::date date, boost::posix_time::time_duration time ) {
  switch ( m_transition ) {
    case ETransition::Quiesce:
      {
        if ( m_pPosition ) {
          if ( m_pPosition->IsActive() ) {
            if ( m_pPosition->GetInstrument()->GetExpiry() == date ) {

              CalendarRoll();

            }
          }
        }
      }
      break;
    default:
      std::cout
        << "Tracker::TestItmRoll: bad state: " << int( m_transition )
        << std::endl;
      break;
  }
}

void Tracker::OptionCandidate_StartWatch() {
  assert( m_pOptionCandidate );
  m_pOptionCandidate->EnableStatsAdd();
  m_pOptionCandidate->OnQuote.Add( MakeDelegate( this, &Tracker::OptionCandidate_HandleQuote ) );
  m_pOptionCandidate->StartWatch();
}

void Tracker::OptionCandidate_StopWatch() {
  assert( m_pOptionCandidate );
  m_pOptionCandidate->StopWatch();
  m_pOptionCandidate->OnQuote.Remove( MakeDelegate( this, &Tracker::OptionCandidate_HandleQuote ) );
  m_pOptionCandidate->EnableStatsRemove();
}

void Tracker::Quiesce() { // called from destructor, Collar
  m_transition = ETransition::Quiesce;
  if ( m_pOptionCandidate ) {
    OptionCandidate_StopWatch();
    m_pOptionCandidate.reset();
  }
}

} // namespace option
} // namespace tf
} // namespace ou
