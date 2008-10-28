#include "StdAfx.h"
#include "SimulationSymbol.h"

#include "HDF5TimeSeriesContainer.h"
#include "HDF5IterateGroups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// sDirectory needs to be available on instantiation to enable signal availability
CSimulationSymbol::CSimulationSymbol( const std::string &sSymbol, const std::string &sDirectory) 
: CSymbol(sSymbol), m_sDirectory( sDirectory )
{
  // this is dealt with in the SimulationProvider, but we don't have a .Remove
  //m_OnTrade.Add( MakeDelegate( &m_simExec, &CSimulateOrderExecution::NewTrade ) );
}

CSimulationSymbol::~CSimulationSymbol(void) {
  // we don't yet have a .Remove for this in SimulationProvider yet.
  //m_OnTrade.Remove( MakeDelegate( &m_simExec, &CSimulateOrderExecution::NewTrade ) );

}

void CSimulationSymbol::StartTradeWatch( void ) {
  std::string sPath( m_sDirectory + "/trades/" + m_sSymbolName );
  CHDF5TimeSeriesContainer<CTrade> tradeRepository( sPath );
  CHDF5TimeSeriesContainer<CTrade>::iterator begin, end;
  begin = tradeRepository.begin();
  end = tradeRepository.end();
  m_trades.Resize( end - begin );
  tradeRepository.Read( begin, end, &m_trades );
}

void CSimulationSymbol::StopTradeWatch( void ) {
}

void CSimulationSymbol::StartQuoteWatch( void ) {
  std::string sPath( m_sDirectory + "/quotes/" + m_sSymbolName );
  CHDF5TimeSeriesContainer<CQuote> quoteRepository( sPath );
  CHDF5TimeSeriesContainer<CQuote>::iterator begin, end;
  begin = quoteRepository.begin();
  end = quoteRepository.end();
  m_quotes.Resize( end - begin );
  quoteRepository.Read( begin, end, &m_quotes );
}

void CSimulationSymbol::StopQuoteWatch( void ) {
}

void CSimulationSymbol::StartDepthWatch( void ) {
}

void CSimulationSymbol::StopDepthWatch( void ) {
}

//bool CSimluationSymbol::AddTradeHandler( CSymbol::tradehandler_t handler ) {
//  return CSymbol::AddTradeHandler( handler );
//}

//bool CSimulationSymbol::RemoveTradeHandler( CSymbol::tradehandler_t handler ) {
//  return CSymbol::RemoveTradeHandler( handler );
//}

void CSimulationSymbol::HandleQuoteEvent( const CDatedDatum &datum ) {
  m_OnQuote( dynamic_cast<const CQuote &>( datum ) ); 
}

void CSimulationSymbol::HandleTradeEvent( const CDatedDatum &datum ) {
  m_OnTrade( dynamic_cast<const CTrade &>( datum ) );  
}

