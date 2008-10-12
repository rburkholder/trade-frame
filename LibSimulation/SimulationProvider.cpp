#include "StdAfx.h"
#include "SimulationProvider.h"

#include <stdexcept>

#include "HDF5DataManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSimulationProvider::CSimulationProvider(void)
: CProviderInterface(), 
  m_pMerge( NULL ), m_pMergeThread( NULL )
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
  CSimulationSymbol *pSymbol = new CSimulationSymbol(sSymbolName, m_sGroupDirectory);
  pSymbol->m_simExec.SetOnOrderFill( MakeDelegate( this, &CSimulationProvider::HandleExecution ) );
  //CProviderInterface::AddTradeHandler( sSymbolName, MakeDelegate( &pSymbol->m_simExec, &CSimulateOrderExecution::NewTrade ) );
  return dynamic_cast<CSymbol *>( pSymbol );
}

//void CSimulationProvider::PreSymbolDestroy( CSymbol *pSymbol ) {
  //CSimulationSymbol *pSymSymbol = dynamic_cast<CSimulationSymbol *>( pSymbol );
  //CProviderInterface::RemoveTradeHandler( pSymbol->Name(), MakeDelegate( &pSymSymbol->m_simExec, &CSimulateOrderExecution::NewTrade ) );
  //CProviderInterface::PreSymbolDestroy( pSymbol );
//}

void CSimulationProvider::AddTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler ) {
  CProviderInterface::AddTradeHandler( sSymbol, handler );
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  assert( m_mapSymbols.end() != iter );
  CSimulationSymbol *pSymSymbol = dynamic_cast<CSimulationSymbol *>( iter->second );
  if ( 1 == iter->second->GetTradeHandlerCount() ) {
    CProviderInterface::AddTradeHandler( sSymbol, MakeDelegate( &pSymSymbol->m_simExec, &CSimulateOrderExecution::NewTrade ) );
  }
}

void CSimulationProvider::RemoveTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler ) {
  CProviderInterface::RemoveTradeHandler( sSymbol, handler );
  std::map<string, CSymbol*>::iterator iter;
  iter = m_mapSymbols.find( sSymbol );
  if ( m_mapSymbols.end() == iter ) {
    assert( false );  // this shouldn't occur
  }
  else {
    if ( 1 == iter->second->GetTradeHandlerCount() ) {
      CSimulationSymbol *pSymSymbol = dynamic_cast<CSimulationSymbol *>( iter->second );
      CProviderInterface::RemoveTradeHandler( sSymbol, MakeDelegate( &pSymSymbol->m_simExec, &CSimulateOrderExecution::NewTrade ) );
    }
  }
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

UINT __cdecl CSimulationProvider::Merge( LPVOID lpParam ) {
  CSimulationProvider *pProvider = reinterpret_cast<CSimulationProvider *>( lpParam );

  pProvider -> m_pMerge = new CMergeDatedDatums();

  for ( m_mapSymbols_t::iterator iter = pProvider->m_mapSymbols.begin();
    iter != pProvider->m_mapSymbols.end(); ++iter ) {
      CSimulationSymbol *sym = dynamic_cast<CSimulationSymbol*>(iter->second);
      CQuotes *quotes = &sym->m_quotes;
      pProvider -> m_pMerge -> Add( 
        quotes, 
        MakeDelegate( dynamic_cast<CSimulationSymbol*>( iter->second ), &CSimulationSymbol::HandleQuoteEvent ) );
      CTrades *trades = &sym->m_trades;
      pProvider -> m_pMerge -> Add( 
        trades, 
        MakeDelegate( dynamic_cast<CSimulationSymbol*>( iter->second ), &CSimulationSymbol::HandleTradeEvent ) );
  }

  //CTimeSource ts;
  bool bOldMode = CTimeSource::GetSimulationMode();
  pProvider -> m_dtSimStart = CTimeSource::External();
  CTimeSource::SetSimulationMode();
  pProvider -> m_pMerge -> Run();
  CTimeSource::SetSimulationMode( bOldMode );
  pProvider -> m_dtSimStop = CTimeSource::External();

  delete pProvider -> m_pMerge;
  pProvider -> m_pMerge = NULL;
  pProvider -> m_pMergeThread = NULL;
  return 1;
}

void CSimulationProvider::Run() {
  if ( 0 == m_sGroupDirectory.size() ) throw std::invalid_argument( "Group Directory is empty" );
  if ( 0 == m_mapSymbols.size() ) throw std::invalid_argument( "No Symbols to simulate" );
  // how to detect end of thread, and reset m_hMergeThread?
  if ( NULL != m_pMerge ) {
    std::cout << "Simulation already in progress" << std::endl;
  }
  else {
    m_pMergeThread = AfxBeginThread( &CSimulationProvider::Merge, reinterpret_cast<LPVOID>( this ), THREAD_PRIORITY_NORMAL );
    assert( NULL != m_pMergeThread );
    //m_hMergeThread = CreateThread( NULL, 0, Merge, this, 0, &m_idMergeThread );
    //assert( NULL != m_hMergeThread );
  }
}

// at some point:  run, stop, pause, resume, reset
void CSimulationProvider::Stop() {
  if ( NULL == m_pMerge ) {
    std::cout << "no simulation to stop" << std::endl;
  }
  else {
    m_pMerge->Stop();
    std::cout << "stopping simulation" << std::endl;
  }
}

void CSimulationProvider::PlaceOrder( COrder *pOrder ) {
  CProviderInterface::PlaceOrder( pOrder ); // any underlying initialization
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pOrder->GetInstrument()->GetSymbolName() );
  if ( m_mapSymbols.end() == iter ) {
    std::cout << "Can't place order, can't find symbol: " << pOrder->GetInstrument()->GetSymbolName() << std::endl;
  }
  else {
    dynamic_cast<CSimulationSymbol *>( iter->second )->m_simExec.SubmitOrder( pOrder );
  }
}

void CSimulationProvider::CancelOrder( COrder *pOrder ) {
  CProviderInterface::CancelOrder( pOrder );
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pOrder->GetInstrument()->GetSymbolName() );
  if ( m_mapSymbols.end() == iter ) {
    std::cout << "Can't cancel order, can't find symbol: " << pOrder->GetInstrument()->GetSymbolName() << std::endl;
  }
  else {
    dynamic_cast<CSimulationSymbol *>( iter->second )->m_simExec.CancelOrder( pOrder->GetOrderId() );
  }
}

void CSimulationProvider::HandleExecution( const CExecution &exec ) {
  m_OrderManager.ReportExecution( exec );
}