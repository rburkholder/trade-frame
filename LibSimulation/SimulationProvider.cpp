#include "StdAfx.h"
#include "SimulationProvider.h"

#include <stdexcept>

#include "HDF5DataManager.h"

#include "MergeDatedDatums.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSimulationProvider::CSimulationProvider(void)
: CProviderInterface(), m_bMergeHasBeenRun( false )
{
  m_sName = "Simulator";
  m_nID = EProviderSimulator;
}

CSimulationProvider::~CSimulationProvider(void) {
}

void CSimulationProvider::SetGroupDirectory( const std::string sGroupDirectory ) {
  CHDF5DataManager dm;
  std::string s;
  if( !dm.GroupExists( sGroupDirectory ) ) throw std::invalid_argument( "Could not find: " + sGroupDirectory );
  s = sGroupDirectory + "/trades";
  if( !dm.GroupExists( s ) ) throw std::invalid_argument( "Could not find: " + s );
  s = sGroupDirectory + "/quotes";
  if( !dm.GroupExists( s ) ) throw std::invalid_argument( "Could not find: " + s );
  m_sGroupDirectory = sGroupDirectory;
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
  if ( 0 == m_sGroupDirectory.size() ) throw std::invalid_argument( "Group Directory is empty" );
  if ( 0 == m_mapSymbols.size() ) throw std::invalid_argument( "No Symbols to simulate" );
  CMergeDatedDatums merge;
  for ( m_mapSymbols_t::iterator iter = m_mapSymbols.begin();
    iter != m_mapSymbols.end(); ++iter ) {
      CSimulationSymbol *sym = dynamic_cast<CSimulationSymbol*>(iter->second);
      CQuotes *quotes = &sym->m_quotes;
      CDatedDatums *qdatums = dynamic_cast<CDatedDatums *>( quotes );
      merge.Add( 
        qdatums, 
        MakeDelegate( dynamic_cast<CSimulationSymbol*>( iter->second ), &CSimulationSymbol::HandleQuoteEvent ) );
      CTrades *trades = &sym->m_trades;
      CDatedDatums *tdatums = dynamic_cast<CDatedDatums *>( trades );
      merge.Add( 
        tdatums, 
        MakeDelegate( dynamic_cast<CSimulationSymbol*>( iter->second ), &CSimulationSymbol::HandleTradeEvent ) );
  }
  merge.Run();
}
