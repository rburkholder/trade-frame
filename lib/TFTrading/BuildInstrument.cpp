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
 * File:    BuildInstrument.cpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created: Sept 20, 2021, 21:52
 */

#include <boost/log/trivial.hpp>

#include <TFIQFeed/BuildInstrument.h>

#include <TFTrading/Watch.h>
#include <TFTrading/InstrumentManager.h>

#include "BuildInstrument.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

using pWatch_t = Watch::pWatch_t;

// basic instrument
BuildInstrument::BuildInstrument( pProviderIQFeed_t pIQFeed )
: m_pIQ( std::move( pIQFeed ) )
, m_bDeleteIterator( false )
{
  assert( m_pIQ );
}

// instrument with contract
BuildInstrument::BuildInstrument( pProviderIQFeed_t pIQFeed, pProviderIBTWS_t pIB )
: m_pIQ( std::move( pIQFeed ) )
, m_pIB( std::move( pIB ) )
, m_bDeleteIterator( false )
{
  assert( m_pIQ );
  assert( m_pIB );
}

void BuildInstrument::Queue( const std::string& sIQFeedSymbol, fInstrument_t&& fInstrument ) {

  pInstrument_t pInstrument;

  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );

  pInstrument = im.LoadInstrument( ou::tf::keytypes::EProviderIQF, sIQFeedSymbol );
  if ( pInstrument ) { // skip the build
    //BOOST_LOG_TRIVIAL(debug) << "BuildInstrument::Build existing: " << pInstrument->GetInstrumentName();
    if ( m_pIB ) {
      m_pIB->Sync( pInstrument );
    }
    fInstrument( pInstrument, false );
  }
  else { // build a new instrument

    //BOOST_LOG_TRIVIAL(debug) << "BuildInstrument::Build new: " << sIQFeedSymbol;

    {
      std::lock_guard<std::mutex> lock( m_mutexMap );
      m_mapSymbol.emplace( std::make_pair( sIQFeedSymbol, std::move( fInstrument ) ) );
    }

    Update();
  }

}

 // queue this to be deleted on next run through
 // prevents a deadlock on delete of watch as the deletion of AcquireFundamentals
 //   happens within the HandleFundamentals delegate
void BuildInstrument::Update( mapInProgress_t::iterator iter ) {
  Update();
  std::lock_guard<std::mutex> lock( m_mutexMap );
  m_bDeleteIterator = true;
  m_iterToDelete = iter;
}

void BuildInstrument::Update() {

  bool bDoBuild( false );
  mapInProgress_t::iterator iterInProgress;

  {
    std::lock_guard<std::mutex> lock( m_mutexMap );

    if ( m_bDeleteIterator ) {
      m_bDeleteIterator = false;
      m_mapInProgress.erase( m_iterToDelete );
    }

    // 5 may not be important any more as IB manages queue
    // but why is queue not being replenished?
    if ( 5 > m_mapInProgress.size() ) { // 5 is set in m_pIB
      if ( 0 != m_mapSymbol.size() ) {

        mapSymbol_t::iterator iterSymbol = m_mapSymbol.begin();

        {
          auto& [ sIQFeedSymbol_, fInstrument_ ] = *iterSymbol;
          const std::string sIQFeedSymbol( sIQFeedSymbol_ );

          auto result = m_mapInProgress.emplace( std::make_pair( sIQFeedSymbol, InProgress( std::move( fInstrument_ ) ) ) );
          assert( result.second );
          iterInProgress = result.first;
          bDoBuild = true;
        }

        //BOOST_LOG_TRIVIAL(debug) << "BuildInstrument::Update erase " << iterSymbol->first;
        m_mapSymbol.erase( iterSymbol );
      }
    }
  }

  if ( bDoBuild ) Build( iterInProgress );

}

void BuildInstrument::Build( mapInProgress_t::iterator iterInProgress ) {

  auto& [ sIQFeedSymbol, ip ] = *iterInProgress;

  pInstrument_t pInstrument;

  // temporary instrument solely for obtaining fundamental data with which to build real instrument
  //pInstrument = ou::tf::iqfeed::BuildInstrument( "Acquire-" + sIQFeedSymbol, trd );
  pInstrument = std::make_shared<ou::tf::Instrument>( "Acquire-" + sIQFeedSymbol ); // just enough to obtain fundamentals
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sIQFeedSymbol );
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQ );

  //BOOST_LOG_TRIVIAL(debug)
  //  << "BuildInstrument: "
  //  << sIQFeedSymbol
  //  ;

  AcquireFundamentals::pAcquireFundamentals_t pAcquireFundamentals
    = std::make_shared<AcquireFundamentals>(
        std::move( pWatch ),
        [this,iterInProgress]( pWatch_t pWatchOld ) { // async call once fundamentals arrive

          //BOOST_LOG_TRIVIAL(debug) << "AcquireFundamentals_done enter: " << iterInProgress->first;

          const ou::tf::Watch::Fundamentals& fundamentals( pWatchOld->GetFundamentals() );
          pInstrument_t pInstrument
            = ou::tf::iqfeed::BuildInstrument( fundamentals );

          std::string sWaiting;
          if ( 0 < m_mapInProgress.size() ) {
            for ( mapInProgress_t::value_type& vt: m_mapInProgress ) {
              sWaiting += "," + vt.first;
            }
          }

          BOOST_LOG_TRIVIAL(info)
            << "BuildInstrument start: "
            << pInstrument->GetInstrumentName() << ','
            << fundamentals.sOptionRoots << ','
            << fundamentals.sExchangeRoot << ','
            << m_mapInProgress.size() << ','
            << m_mapSymbol.size()
            //<< "," << iterInProgress->first
            //<< "," << sWaiting
            ;

          if ( m_pIB ) {

            std::string sName;

            switch ( pInstrument->GetInstrumentType() ) {
              case InstrumentType::Option:
                sName = ou::tf::iqfeed::MarketSymbol::OptionBaseName( fundamentals );
                break;
              default:
                sName = ( 0 == fundamentals.sExchangeRoot.size() ) ? fundamentals.sSymbolName : fundamentals.sExchangeRoot;
                break;
            };

            m_pIB->RequestContractDetails(
              sName,  // needs to be the IB base name
              pInstrument,  // this is a filled-in, prepared instrument
              [this,iterInProgress]( const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument ){
                //BOOST_LOG_TRIVIAL(debug) << "BuildInstrument::Build contract: " << pInstrument->GetInstrumentName();
                assert( 0 != pInstrument->GetContract() );
                m_pIB->Sync( pInstrument );
                //ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
                //im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
                iterInProgress->second.fInstrument( pInstrument, true );
              },
              [this,iterInProgress]( bool bStatus ) {
                if ( bStatus ) {
                  //BOOST_LOG_TRIVIAL(debug)
                  //  << "BuildInstrument::Build done: "
                  //  << m_mapSymbol.size() << ","
                  //  << m_mapInProgress.size() << ","
                  //  << iterInProgress->first
                  //;
                }
                else {
                  BOOST_LOG_TRIVIAL(warning)
                    << "BuildInstrument fail:  "
                    << iterInProgress->first
                    << "," << m_mapSymbol.size()
                    << "," << m_mapInProgress.size()
                    ;
                  iterInProgress->second.fInstrument( nullptr, false );
                }
                {
                  std::lock_guard<std::mutex> lock( m_mutexMap );
                  m_mapInProgress.erase( iterInProgress );
                }
                Update(); // this should replenish queue
              }
              );
          }
          else {
            //ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
            //im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
            iterInProgress->second.fInstrument( pInstrument, true );
            pInstrument.reset();
            Update( iterInProgress );
          }
          //BOOST_LOG_TRIVIAL(debug) << "AcquireFundamentals_done exit: " << iterInProgress->first;
        }

      );

  iterInProgress->second.pAcquireFundamentals = pAcquireFundamentals;
  pAcquireFundamentals->Start();
}

void BuildInstrument::Clear() {
  std::lock_guard<std::mutex> lock( m_mutexMap );
  assert( 0 == m_mapInProgress.size() );
  m_mapSymbol.clear();
}

bool BuildInstrument::Active() {
  return !( m_mapSymbol.empty() && m_mapInProgress.empty() );
}

} // namespace tf
} // namespace ou
