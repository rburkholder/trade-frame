/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "ProviderInterface.h"

CProviderInterface::CProviderInterface(void) 
: m_bConnected( false ), m_nID( 0 )
{
}

CProviderInterface::~CProviderInterface(void) {
  // need to step through and unwatch anything still watching
  m_mapSymbols_t::iterator iter = m_mapSymbols.begin();
  while ( m_mapSymbols.end() != iter ) {
//    PreSymbolDestroy( iter->second );
    delete iter->second;
    ++iter;
  }
  m_mapSymbols.clear();
}

void CProviderInterface::Connect() {
}

void CProviderInterface::Disconnect() {
}

CSymbol* CProviderInterface::AddCSymbol( CSymbol* pSymbol) {
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pSymbol->Name() );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( pSymbol->Name(), pSymbol ) );
    iter = m_mapSymbols.find( pSymbol->Name() );
    assert( m_mapSymbols.end() != iter );
  }
  return iter->second;
}

void CProviderInterface::AddQuoteHandler(const std::string &sSymbol, CSymbol::quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddQuoteHandler( handler ) ) {
    StartQuoteWatch( iter->second );
  }
}

void CProviderInterface::RemoveQuoteHandler(const std::string &sSymbol, CSymbol::quotehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
  //  m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, new CSymbol( sSymbol ) ) );
  }
  else {
    if ( iter->second->RemoveQuoteHandler( handler ) ) {
      StopQuoteWatch( iter->second );
    }
  }
}

void CProviderInterface::AddTradeHandler(const std::string &sSymbol, CSymbol::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddTradeHandler( handler ) ) {
    StartTradeWatch( iter->second );
  }
}

void CProviderInterface::RemoveTradeHandler(const std::string &sSymbol, CSymbol::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
  //  m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, new CSymbol( sSymbol ) ) );
    // should probably raise exception here as trying to remove handler from non-existtant symbol
  }
  else {
    if ( iter->second->RemoveTradeHandler( handler ) ) {
      StopTradeWatch( iter->second );
    }
  }
}

void CProviderInterface::AddOnOpenHandler(const std::string &sSymbol, CSymbol::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  iter->second->AddOnOpenHandler( handler );
}

void CProviderInterface::RemoveOnOpenHandler(const std::string &sSymbol, CSymbol::tradehandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
  //  m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, new CSymbol( sSymbol ) ) );
  }
  else {
    iter->second->RemoveOnOpenHandler( handler );
  }
}

void CProviderInterface::AddDepthHandler(const std::string &sSymbol, CSymbol::depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddDepthHandler( handler ) ) {
    StartDepthWatch( iter->second );
  }
}

void CProviderInterface::RemoveDepthHandler(const std::string &sSymbol, CSymbol::depthhandler_t handler) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
  //  m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, new CSymbol( sSymbol ) ) );
  }
  else {
    if ( iter->second->RemoveDepthHandler( handler ) ) {
      StopDepthWatch( iter->second );
    }
  }
}

CSymbol *CProviderInterface::GetSymbol( const string &sSymbol ) {
  m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( pair_mapSymbols_t( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  return iter->second;
}

//void CProviderInterface::PreSymbolDestroy( CSymbol *pSymbol ) {
//}

void CProviderInterface::PlaceOrder( COrder *pOrder ) {
  //throw std::runtime_error( "CProviderInterface::PlaceOrder not implemented." );
  pOrder->SetProviderName( m_sName );
}

void CProviderInterface::CancelOrder( COrder *pOrder ) {
  //throw std::runtime_error( "CProviderInterface::CancelOrder not implemented." );
}

void CProviderInterface::SetAlternateInstrumentName(const std::string &OriginalInstrumentName, const std::string &AlternateIntrumentName) {
  m_lutAlternateInstrumentNames.Save( m_sName, OriginalInstrumentName, AlternateIntrumentName );
  std::map<std::string, std::string>::iterator iter 
    = m_mapAlternateNames.find( OriginalInstrumentName );
  if ( m_mapAlternateNames.end() == iter ) {
    m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, AlternateIntrumentName ) );
  }
  else m_mapAlternateNames[ OriginalInstrumentName ] = AlternateIntrumentName;
}

void CProviderInterface::GetAlternateInstrumentName(const std::string &OriginalInstrumentName, std::string *pAlternateInstrumentName) {
  std::map<std::string, std::string>::iterator iter 
    = m_mapAlternateNames.find( OriginalInstrumentName );
  if ( m_mapAlternateNames.end() != iter ) {
    pAlternateInstrumentName->assign( iter->second );
  }
  else {
    try {
      m_lutAlternateInstrumentNames.Get( m_sName, OriginalInstrumentName, pAlternateInstrumentName );
      m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, *pAlternateInstrumentName ) );
    }
    catch ( std::out_of_range e ) {
      m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, OriginalInstrumentName ) );
      pAlternateInstrumentName->assign( OriginalInstrumentName );
    }
    catch ( std::exception e ) {
      std::cout << "CProviderInterface::GetAlternateInstrumentName has error: " << e.what() << std::endl;
    }
  }
}