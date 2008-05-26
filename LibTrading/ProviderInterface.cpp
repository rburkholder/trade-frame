#include "StdAfx.h"
#include "ProviderInterface.h"
//#include "boost\lambda.hpp"

//#include <algorithms>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProviderInterface::CProviderInterface(void) 
: m_bConnected( false )
{
}

CProviderInterface::~CProviderInterface(void) {
  // need to step through and unwatch anything still watching
  std::map<string, CSymbol*>::iterator iter = m_mapSymbols.begin();
  while ( m_mapSymbols.end() != iter ) {
    delete iter->second;
    ++iter;
  }
  m_mapSymbols.clear();
}

void CProviderInterface::Connect() {
}

void CProviderInterface::Disconnect() {
}

void CProviderInterface::AddQuoteHandler(const std::string &sSymbol, CSymbol::quotehandler_t handler) {
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddQuoteHandler( handler ) ) {
    StartQuoteWatch( iter->second );
  }
}

void CProviderInterface::RemoveQuoteHandler(const std::string &sSymbol, CSymbol::quotehandler_t handler) {
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
  //  m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, new CSymbol( sSymbol ) ) );
  }
  else {
    if ( iter->second->RemoveQuoteHandler( handler ) ) {
      StopQuoteWatch( iter->second );
    }
  }
}

void CProviderInterface::AddTradeHandler(const std::string &sSymbol, CSymbol::tradehandler_t handler) {
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddTradeHandler( handler ) ) {
    StartTradeWatch( iter->second );
  }
}

void CProviderInterface::RemoveTradeHandler(const std::string &sSymbol, CSymbol::tradehandler_t handler) {
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
  //  m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, new CSymbol( sSymbol ) ) );
  }
  else {
    if ( iter->second->RemoveTradeHandler( handler ) ) {
      StopTradeWatch( iter->second );
    }
  }
}

void CProviderInterface::AddDepthHandler(const std::string &sSymbol, CSymbol::depthhandler_t handler) {
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  if ( iter->second->AddDepthHandler( handler ) ) {
    StartDepthWatch( iter->second );
  }
}

void CProviderInterface::RemoveDepthHandler(const std::string &sSymbol, CSymbol::depthhandler_t handler) {
  std::map<string, CSymbol*>::iterator iter;
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
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    m_mapSymbols.insert( std::pair<string, CSymbol*>( sSymbol, NewCSymbol( sSymbol ) ) );
    iter = m_mapSymbols.find( sSymbol );
  }
  return iter->second;
}