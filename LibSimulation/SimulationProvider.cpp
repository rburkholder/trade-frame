#include "StdAfx.h"
#include "SimulationProvider.h"

#include "MergeDatedDatums.h"

CSimulationProvider::CSimulationProvider(void)
: CProviderInterface(), m_bMergeHasBeenRun( false )
{
  m_sName = "Simulator";
  m_nID = EProviderSimulator;
}

CSimulationProvider::~CSimulationProvider(void) {
}

void CSimulationProvider::Connect() {
  if ( !m_bConnected ) {
    m_bConnected = true;
    CProviderInterface::Connect();
    OnConnected( 0 );
  }
}

void CSimulationProvider::Disconnect() {
  if ( m_bConnected ) {
    m_bConnected = false;
    CProviderInterface::Disconnect();
    OnDisconnected( 0 );
  }
}

CSymbol *CSimulationProvider::NewCSymbol( const std::string &sSymbolName ) {
  return dynamic_cast<CSymbol *>( new CSimulationSymbol(sSymbolName, m_sGroupDirectory) );
}

// these need to open the data file, load the data, and prepare to simulate
void CSimulationProvider::StartQuoteWatch( CSymbol *pSymbol ) {
  dynamic_cast<CSimulationSymbol*>( pSymbol )->StartQuoteWatch();
}

void CSimulationProvider::StopQuoteWatch( CSymbol *pSymbol ) {
  dynamic_cast<CSimulationSymbol*>( pSymbol )->StopQuoteWatch();
}

void CSimulationProvider::StartTradeWatch( CSymbol *pSymbol ) {
  dynamic_cast<CSimulationSymbol*>( pSymbol )->StartTradeWatch();
}

void CSimulationProvider::StopTradeWatch( CSymbol *pSymbol ) {
  dynamic_cast<CSimulationSymbol*>( pSymbol )->StopTradeWatch();
}

void CSimulationProvider::StartDepthWatch( CSymbol *pSymbol ) {
  dynamic_cast<CSimulationSymbol*>( pSymbol )->StartDepthWatch();
}

void CSimulationProvider::StopDepthWatch( CSymbol *pSymbol ) {
  dynamic_cast<CSimulationSymbol*>( pSymbol )->StopDepthWatch();
}

void CSimulationProvider::Run() {
  CMergeDatedDatums merge;
  for ( m_mapSymbols_t::iterator iter = m_mapSymbols.begin();
    iter != m_mapSymbols.end(); ++iter ) {
      merge.Add( 
        dynamic_cast<CTimeSeries<CDatedDatum>*>( &(dynamic_cast<CSimulationSymbol*>(iter->second)->m_quotes) ), 
        MakeDelegate( dynamic_cast<CSimulationSymbol*>( iter->second ), &CSimulationSymbol::HandleQuoteEvent ) );
      merge.Add( 
        dynamic_cast<CTimeSeries<CDatedDatum>*>( &(dynamic_cast<CSimulationSymbol*>(iter->second)->m_trades) ), 
        MakeDelegate( dynamic_cast<CSimulationSymbol*>( iter->second ), &CSimulationSymbol::HandleTradeEvent ) );
  }
  merge.Run();
}
