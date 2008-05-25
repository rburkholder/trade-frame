#include "StdAfx.h"
#include "IQFeedProvider.h"
#include "IQFeedSymbol.h"

CIQFeedProvider::CIQFeedProvider(void)
: CProviderInterface(), CIQFeed()
{
  m_sName = "IQFeed";
}

CIQFeedProvider::~CIQFeedProvider(void) {
}

void CIQFeedProvider::Connect() {
  if ( !m_bConnected ) {
    CIQFeed::Connect();
    m_bConnected = true;
    OnConnected( 0 );
  }
}

void CIQFeedProvider::Disconnect() {
  if ( m_bConnected ) {
    CIQFeed::Disconnect();
    m_bConnected = false;
    OnDisconnected( 0 );
  }
}

CSymbol *CIQFeedProvider::NewCSymbol(const std::string &sSymbolName) {
  return new CIQFSymbol( sSymbolName );
}

void CIQFeedProvider::StartQuoteWatch(CSymbol *pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<CIQFSymbol *>( pSymbol ) );
}

void CIQFeedProvider::StopQuoteWatch(CSymbol *pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<CIQFSymbol *>( pSymbol ) );
}

void CIQFeedProvider::StartTradeWatch(CSymbol *pSymbol) {
  StartQuoteTradeWatch( dynamic_cast<CIQFSymbol *>( pSymbol ) );
}

void CIQFeedProvider::StopTradeWatch(CSymbol *pSymbol) {
  StopQuoteTradeWatch( dynamic_cast<CIQFSymbol *>( pSymbol ) );
}

