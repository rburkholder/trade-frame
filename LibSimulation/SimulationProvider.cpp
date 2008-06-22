#include "StdAfx.h"
#include "SimulationProvider.h"

CSimulationProvider::CSimulationProvider(void)
: CProviderInterface()
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
  return new CSymbol(sSymbolName) ;
}

// these need to open the data file, load the data, and prepare to simulate
void CSimulationProvider::StartQuoteWatch( CSymbol *pSymbol ) {
}

void CSimulationProvider::StopQuoteWatch( CSymbol *pSymbol ) {
}

void CSimulationProvider::StartTradeWatch( CSymbol *pSymbol ) {
}

void CSimulationProvider::StopTradeWatch( CSymbol *pSymbol ) {
}

void CSimulationProvider::StartDepthWatch( CSymbol *pSymbol ) {
}

void CSimulationProvider::StopDepthWatch( CSymbol *pSymbol ) {
}

