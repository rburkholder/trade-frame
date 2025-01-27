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
 * File:    Process.cpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 1, 2022  21:48
 */

#include <TFTrading/AcquireFundamentals.h>

#include "Process.hpp"

Process::Process( const config::Choices& choices, vSymbols_t& vSymbols )
: m_choices( choices ), m_vSymbols( vSymbols ), m_bDone( false )
{

  m_piqfeed = ou::tf::iqfeed::Provider::Factory();

  m_piqfeed->OnConnected.Add( MakeDelegate( this, &Process::HandleConnected ) );
  m_piqfeed->Connect();

}

Process::~Process() {

  assert( m_mapInProgress.empty() );
  m_pAcquireFundamentals_burial.reset();

  for ( mapIgnoreName_t::value_type& vt: m_mapIgnoreName ) {
    if ( !vt.second ) {
      std::cout << "ignore name " << vt.first << " not used" << std::endl;
    }
  }

  m_piqfeed->Disconnect();
  m_piqfeed.reset();

  m_mapIgnoreName.clear();
  m_mapSecurityState.clear();

}

void Process::HandleConnected( int ) {

  using vName_t = config::vName_t;

  using key_t = ou::tf::iqfeed::SymbolLookup::key_t;
  using setNames_t = ou::tf::iqfeed::SymbolLookup::setNames_t;

  setNames_t setListedMarket;

  for ( const vName_t::value_type name: m_choices.m_vListedMarket ) {
    setListedMarket.emplace( name );
  }

  setNames_t setSecurityType;

  for ( const vName_t::value_type type: m_choices.m_vSecurityType ) {
    setSecurityType.emplace( type );
  }

  for ( const vName_t::value_type name: m_choices.m_vIgnoreNames ) {
    mapIgnoreName_t::const_iterator iter = m_mapIgnoreName.find( name );
    if ( m_mapIgnoreName.end() == iter ) {
      m_mapIgnoreName.emplace( std::move( name ), false );
    }
    else {
      std::cout << "ignore name " << name << " exists" << std::endl;
    }
  }

  for ( const vName_t::value_type security_state: m_choices.m_vSecurityState ) {
    std::string::size_type pos = security_state.find( ',' );
    if ( std::string::npos == pos ) {
      std::cout << "malformed line: " << security_state << std::endl;
      assert( false );
    }
    assert( 0 < pos );
    assert( pos < security_state.size() );
    const std::string security( security_state.substr( 0, pos ) );
    const std::string state( security_state.substr( pos + 1 ) );
    //std::cout << "security state " << security << '=' << state << std::endl;
    m_mapSecurityState.emplace( std::move( security ), std::move( state ) );
  }

  std::cout << "symbol retrieval started ..." << std::endl;

  m_piqfeed->SymbolList(
    setListedMarket, setSecurityType,
    [this](const std::string& sSymbol, key_t keyListedMarket){
      mapIgnoreName_t::iterator iter = m_mapIgnoreName.find( sSymbol );
      if ( m_mapIgnoreName.end() != iter ) {
        iter->second = true;
      }
      else {
        m_vSymbols.push_back( dividend_t( sSymbol ) );
      }
    },
    [this](){
      std::cout << "symbol retrieval done" << std::endl;
      m_iterSymbols = m_vSymbols.begin();
      while ( ( m_choices.m_nMaxInTransit > m_mapInProgress.size() ) && ( m_vSymbols.end() != m_iterSymbols ) ) {
        Lookup();
      }
    }
  );

}

void Process::Lookup() {

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using Summary = ou::tf::Watch::Summary;
  using Fundamentals = ou::tf::Watch::Fundamentals;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  const std::string sSymbol( m_iterSymbols->sSymbol );
  //std::cout << "lookup " << sSymbol << std::endl;

  pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sSymbol );
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sSymbol );
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );

  mapInProgress_t::iterator iterInProgress
    = m_mapInProgress.insert(
        m_mapInProgress.begin(),
        mapInProgress_t::value_type( sSymbol, InProgress() ) );

  iterInProgress->second.iterSymbols = m_iterSymbols;

  iterInProgress->second.pAcquireFundamentals
    = ou::tf::AcquireFundamentals::Factory (
      std::move( pWatch ),
      [this,iterInProgress,iterSymbols=m_iterSymbols]( pWatch_t pWatch ){
        dividend_t& dividend( *iterSymbols );
        const Summary& summary( pWatch->GetSummary() );
        dividend.trade = summary.dblTrade;
        const Fundamentals& fundamentals( pWatch->GetFundamentals() );
        dividend.sCompanyName = fundamentals.sCompanyName;
        dividend.sExchange = fundamentals.sExchange;
        dividend.rate = fundamentals.dblDividendRate;
        dividend.yield = fundamentals.dblDividendYield;
        dividend.amount = fundamentals.dblDividendAmount;
        dividend.nAverageVolume = fundamentals.nAverageVolume;
        dividend.datePayed = fundamentals.datePayed;
        dividend.dateExDividend = fundamentals.dateExDividend;
        dividend.nSharesOutstanding = fundamentals.dblCommonSharesOutstanding;
        dividend.sOptionRoots = fundamentals.sOptionRoots;

        mapSecurityState_t::const_iterator iterSecurity = m_mapSecurityState.find( dividend.sSymbol );
        if ( m_mapSecurityState.end() != iterSecurity ) {
          dividend.sState = iterSecurity->second;
        }

        //if ( 10.0 < fundamentals.dblDividendYield ) {
        //  std::cout
        //    << fundamentals.sExchange
        //    << " " << fundamentals.sSymbolName
        //    << " div.yield=" << fundamentals.dblDividendYield
        //    << std::endl;
        //}
        m_pAcquireFundamentals_burial = std::move( iterInProgress->second.pAcquireFundamentals );
        m_mapInProgress.erase( iterInProgress ); // needs to come before the lookup
        if ( m_vSymbols.end() == m_iterSymbols ) {
          if  ( 0 == m_mapInProgress.size() ) {
            m_bDone = true;
            m_cvWait.notify_one();
          }
          else {
            std::cout << "waiting on: ";
            for ( mapInProgress_t::value_type& vt: m_mapInProgress ) {
              std::cout << vt.first << ",";
            }
            std::cout << std::endl;
          }
        }
        else {
          Lookup();
        }
      }
      );
  iterInProgress->second.pAcquireFundamentals->Start();
  m_iterSymbols++;
}

void Process::Wait() {
  std::unique_lock<std::mutex> lock( m_mutexWait );
  m_cvWait.wait( lock, [this]{ return m_bDone; } );
}

