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

// non-re-usable, construct a new tracker with each new leg
// otherwise need to work on state initialization

#include <memory>

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
, m_track_type( ETransition::Unknown )
, m_bLock( false )
, m_compare( nullptr )
, m_luStrike( nullptr )
, m_luNameAtStrike( nullptr )
, m_pChain( nullptr )
, m_dblStrikePosition {}
, m_sidePosition( ou::tf::OptionSide::Unknown )
, m_dblUnderlyingSlope {}, m_dblUnderlyingPrice {}
, m_fConstructOption( nullptr )
, m_fLegRoll( nullptr )
, m_fLegClose( nullptr )
{}

Tracker::Tracker( Tracker&& rhs )
: m_transition( rhs.m_transition )
, m_track_type( rhs.m_track_type )
, m_bLock( false )
, m_compare( std::move( rhs.m_compare ) )
, m_luStrike( std::move( rhs.m_luStrike ) )
, m_luNameAtStrike( std::move( rhs.m_luNameAtStrike ) )
, m_pChain( std::move( rhs.m_pChain ) )
, m_dblStrikePosition( rhs.m_dblStrikePosition )
, m_sidePosition( rhs.m_sidePosition )
, m_dblUnderlyingPrice( rhs.m_dblUnderlyingPrice )
, m_dblUnderlyingSlope( rhs.m_dblUnderlyingSlope )
, m_pPosition( std::move( rhs.m_pPosition ) )
, m_pOptionCandidate( std::move( rhs.m_pOptionCandidate ) )
, m_fConstructOption( std::move( rhs.m_fConstructOption ) )
, m_fLegRoll( std::move( rhs.m_fLegRoll ) )
, m_fLegClose( std::move( rhs.m_fLegClose ) )
{
  assert( !m_pOptionCandidate );  // can't be watching
}

Tracker::~Tracker() {
  if ( m_pPosition ) {
    m_pPosition->GetWatch()->EnableStatsRemove();
  }
  Quiesce();
  m_transition = ETransition::Done;
  m_compare = nullptr;
  m_luStrike = nullptr;
  m_luNameAtStrike = nullptr;
  m_fConstructOption = nullptr;
  m_fLegRoll = nullptr;
  m_fLegClose = nullptr;
  m_pPosition.reset();
}

void Tracker::Initialize(
  pPosition_t pPosition
, const chain_t* pChain
, fConstructOption_t&& fConstructOption
, fLegRoll_t&& fLegRoll
, fLegClose_t&& fLegClose
) {

  //BOOST_LOG_TRIVIAL(info) << "Tracker::Initialize,external";

  assert( pPosition );
  assert( fConstructOption );
  assert( fLegRoll );
  assert( fLegClose );

  assert( ETransition::Initial == m_transition );

  m_pChain = pChain;

  m_fConstructOption = std::move( fConstructOption );
  m_fLegRoll = std::move( fLegRoll );
  m_fLegClose = std::move( fLegClose );

  Initialize( pPosition );

}

void Tracker::Initialize( pPosition_t pPosition ) {

  //BOOST_LOG_TRIVIAL(info) << "Tracker::Initialize,internal";

  assert( ETransition::Initial == m_transition );  // needs to be set prior to entry
  assert( !m_pPosition );

  m_pPosition = std::move( pPosition );

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  pInstrument_t pInstrument = m_pPosition->GetInstrument();
  assert( pInstrument->IsOption() || pInstrument->IsFuturesOption() );

  m_dblStrikePosition = pInstrument->GetStrike();
  m_sidePosition = pInstrument->GetOptionSide();

  switch ( m_sidePosition ) {
    case ou::tf::OptionSide::Call:
      m_compare = &gt;
      m_luNameAtStrike = [pChain=m_pChain](double strike) { return pChain->GetIQFeedNameCall( strike ); };
      break;
    case ou::tf::OptionSide::Put:
      m_compare = &lt;
      m_luNameAtStrike = [pChain=m_pChain](double strike) { return pChain->GetIQFeedNamePut( strike ); };
      break;
    default:
      assert( false );
  }

  m_pPosition->GetWatch()->EnableStatsAdd();

  m_transition = ETransition::Vacant_Init;
}

bool Tracker::TestLong( boost::posix_time::ptime dt, double dblUnderlyingSlope, double dblUnderlyingPrice ) {

  bool bRemove( false );

  m_dblUnderlyingPrice = dblUnderlyingPrice;
  m_dblUnderlyingSlope = dblUnderlyingSlope;

  switch ( m_transition ) {

    case ETransition::Vacant_Init:

      assert( nullptr == m_luStrike );
      m_track_type = ETransition::Track_Long;

      switch ( m_sidePosition ) {
        case ou::tf::OptionSide::Call:
          m_luStrike = [pChain=m_pChain](double dblUnderlying){ return pChain->Call_Itm( dblUnderlying ); };
          break;
        case ou::tf::OptionSide::Put:
          m_luStrike = [pChain=m_pChain](double dblUnderlying){ return pChain->Put_Itm( dblUnderlying ); };
          break;
        default:
          assert( false );
      }

      m_transition = ETransition::Vacant;
      // fall through
    case ETransition::Vacant:
      {
        double strikeItm = m_luStrike( m_dblUnderlyingPrice );

        if ( m_compare( strikeItm, m_dblStrikePosition ) ) { // is new strike further itm?
          OptionCandidate_Construct( dt, strikeItm );
        }
      }
      break;
    case ETransition::Track_Long:

      assert( m_luStrike );
      assert( m_pOptionCandidate );

      {
        double strikeItm = m_luStrike( m_dblUnderlyingPrice );

        if ( m_compare( strikeItm, m_dblStrikePosition ) ) { // is new strike further itm?
        // TODO: refactor to remove the if/else and put Vacant/Construct together?
          if ( m_compare( strikeItm, m_pOptionCandidate->GetStrike() ) ) {
            m_transition = ETransition::Vacant;
            OptionCandidate_StopWatch();
            m_pOptionCandidate.reset();
            //m_luStrike = nullptr;
            //OptionCandidate_Construct( dt, strikeItm );
          }
          else {
            // TODO: if retreating, stay pat, retreat, or try the roll?
          }
        }
      }

      break;
    case ETransition::Roll_start:
      LegRoll(); // delayed a tick to call in Tick thread
      bRemove = true;
      break;
    case ETransition::Track_Short: // mutually exclusive
      assert( false );
    default:
      break;
  }
  return bRemove;
}

bool Tracker::TestShort( boost::posix_time::ptime dt, double dblUnderlyingSlope, double dblUnderlyingPrice ) {

  bool bRemove( false );

  m_dblUnderlyingPrice = dblUnderlyingPrice;
  m_dblUnderlyingSlope = dblUnderlyingSlope;

  switch ( m_transition ) {
    case ETransition::Vacant_Init:

      assert( nullptr == m_luStrike );
      m_track_type = ETransition::Track_Short;

      switch ( m_sidePosition ) {
        case ou::tf::OptionSide::Call:
          m_luStrike = [this,pChain=m_pChain](double strike ){ return pChain->Call_ItmAtm( strike ); };
          break;
        case ou::tf::OptionSide::Put:
          m_luStrike = [this,pChain=m_pChain](double strike ){ return pChain->Put_ItmAtm( strike ); };
          break;
        default:
          assert( false );
      }
      m_transition = ETransition::Vacant;
      // fall through
    case ETransition::Vacant:
      {
        double strike = m_luStrike( m_dblStrikePosition );
        OptionCandidate_Construct( dt, strike );
      }
      break;
    case ETransition::Track_Short:
      {
        assert( m_pOptionCandidate );

        using pWatch_t = ou::tf::Watch::pWatch_t;
        pWatch_t pWatchCurrent = m_pPosition->GetWatch();

        const ou::tf::Quote& quote( pWatchCurrent->LastQuote() );
        if ( quote.IsNonZero() && ( quote.Ask() > quote.Bid() ) ) {

          bool bSpreadOk_current, bSpreadOk_candidate;
          size_t nCount_current, nCount_candidate;
          double dblSpread_current, dblSpread_candiate;

          std::tie( bSpreadOk_current, nCount_current, dblSpread_current ) = pWatchCurrent->SpreadStats();
          std::tie( bSpreadOk_candidate, nCount_candidate, dblSpread_candiate ) = m_pOptionCandidate->SpreadStats();

          if ( bSpreadOk_current && bSpreadOk_candidate ) {

            if ( 0.401 > quote.Bid() ) { // decision time

              const std::string& sCurrent( pWatchCurrent->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );
              const std::string& sCandidate( m_pOptionCandidate->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );

              if ( sCurrent == sCandidate ) {
                // perform exit, no roll available
                LegClose();
                bRemove = true;
              }
              else {
                // exit or calendar roll, depending upon room and gained premium
                pOption_t pOptionCurrent = std::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
                ou::tf::option::Option::premium_t premiumCurrent( pOptionCurrent->Premium( m_dblUnderlyingPrice ) );
                ou::tf::option::Option::premium_t premiumCandidate( m_pOptionCandidate->Premium( m_dblUnderlyingPrice ) );

                if ( 0.20 < ( premiumCandidate.extrinsic < premiumCurrent.extrinsic ) ) {
                  LegRoll(); // TODO: need to verify operation
                  bRemove = true;
                }
                else {
                  // exit, roll not economical
                  LegClose();
                  bRemove = true;
                }
              }
            }
          }
        }
      }
      break;
    case ETransition::Roll_start: // used for Roll or Close
      LegRoll(); // delayed a tick to call in Tick thread
      bRemove = true;
    case ETransition::Track_Long: // mutually exclusive
      assert( false );
    default:
      break;
  }
  return bRemove;
}

void Tracker::OptionCandidate_Construct( boost::posix_time::ptime dt, double strike ) {

  // TODO: need to enable greeks to track IV, Delta for Cobmo level aggregation

  m_transition = ETransition::Acquire;

  assert( m_luNameAtStrike );

  const std::string& sNameCandidate( m_luNameAtStrike( strike ) );

  using pWatch_t = ou::tf::Watch::pWatch_t;
  pWatch_t pWatchCurrent = m_pPosition->GetWatch();

  const std::string& sNameCurrent( pWatchCurrent->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );

  if ( sNameCurrent != sNameCandidate ) {
    BOOST_LOG_TRIVIAL(info)
      << dt.time_of_day() << " "
      << "Tracker::Construct candidate "
      << sNameCandidate
      << " at " << strike
      ;

    m_fConstructOption(
      sNameCandidate,
      [this]( pOption_t pOption ){
        assert( !m_pOptionCandidate );
        m_pOptionCandidate = std::move( pOption );
        OptionCandidate_StartWatch();
        m_transition = ETransition::Spread;
      } );
  }

}

void Tracker::OptionCandidate_HandleQuote( const ou::tf::Quote& quote ) {
  // different thread than TestLong/TestShort

  switch ( m_transition ) {
    case ETransition::Spread:
      {
        bool bSpreadOk;
        size_t nCount;
        double dblSpread;

        std::tie( bSpreadOk, nCount, dblSpread ) = m_pOptionCandidate->SpreadStats();

        if ( bSpreadOk ) {
          m_transition = m_track_type;
        }
      }
      break;
    case ETransition::Track_Long:
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
                auto pOldWatch = m_pPosition->GetWatch();
                BOOST_LOG_TRIVIAL(info)
                  << pOldWatch->LastQuote().DateTime().time_of_day()
                  << ",roll-per-share-diff=" << diff
                  ;
                m_transition = ETransition::Roll_start;
                //LegRoll();
              }
            }
          }
        }
      }
      break;
    case ETransition::Track_Short:
      // anything to do here?
      break;
    default:
      break;
  }
}

void Tracker::LegRoll() {

  auto pOldWatch = m_pPosition->GetWatch();
  BOOST_LOG_TRIVIAL(info)
    << pOldWatch->LastQuote().DateTime().time_of_day()
    << ",roll"
    << ",old=" << pOldWatch->GetInstrumentName()
    << ",b=" << pOldWatch->LastQuote().Bid()
    << ",a=" << pOldWatch->LastQuote().Ask()
    << ",new=" << m_pOptionCandidate->GetInstrument()->GetInstrumentName()
    << ",b=" << m_pOptionCandidate->LastQuote().Bid()
    << ",a=" << m_pOptionCandidate->LastQuote().Ask()
    << ",underlying=" << m_dblUnderlyingPrice
    << ",slope=" << m_dblUnderlyingSlope
    ;

  assert( m_pPosition );
  m_transition = ETransition::Done;

  OptionCandidate_StopWatch();
  pOption_t pOption = std::move( m_pOptionCandidate );
  m_pOptionCandidate.reset();

  pPosition_t pPosition = std::move( m_pPosition );
  m_pPosition.reset(); // might be redundant

  m_fLegRoll( pPosition, pOption );
  m_fLegRoll = nullptr;
}

void Tracker::LegClose() {

  auto pOldWatch = m_pPosition->GetWatch();
  BOOST_LOG_TRIVIAL(info)
    << pOldWatch->LastQuote().DateTime().time_of_day()
    << ",close"
    << ",old=" << pOldWatch->GetInstrumentName()
    << ",b=" << pOldWatch->LastQuote().Bid()
    << ",a=" << pOldWatch->LastQuote().Ask()
    ;

  m_transition = ETransition::Done;

  OptionCandidate_StopWatch();
  m_pOptionCandidate.reset();

  pPosition_t pPosition = std::move( m_pPosition );
  m_pPosition.reset(); // might be redundant

  m_fLegClose( pPosition );
  m_fLegClose = nullptr;
}

void Tracker::Lock( bool bLock ) {
  //BOOST_LOG_TRIVIAL(info) << "Tracker::Lock()";
  m_bLock = bLock;
}

void Tracker::TestItmRoll( boost::gregorian::date date, boost::posix_time::time_duration time ) {
  switch ( m_transition ) {
    case ETransition::Quiesce:
      {
        if ( m_pPosition ) {
          if ( m_pPosition->IsActive() ) {
            if ( m_pPosition->GetInstrument()->GetExpiry() == date ) {
              //LegRoll();
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
