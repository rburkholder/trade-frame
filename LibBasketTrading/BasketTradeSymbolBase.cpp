#include "StdAfx.h"
#include "BasketTradeSymbolBase.h"

#include "InstrumentFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBasketTradeSymbolBase::CBasketTradeSymbolBase( void ) 
: m_pInstrument( NULL ), m_pdvChart( NULL ), m_pModelParameters( NULL )
{
  Initialize();
}

CBasketTradeSymbolBase::CBasketTradeSymbolBase(
  const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy
  ) 
: m_status( sSymbolName ), m_sPath( sPath ),  m_sStrategy( sStrategy ), 
  m_pInstrument( NULL ), m_pdvChart( NULL ), m_pModelParameters( NULL )
{
  Initialize();
}

CBasketTradeSymbolBase::~CBasketTradeSymbolBase(void) {
  if ( NULL != m_pdvChart ) {
    m_pdvChart->Close();
    delete m_pdvChart;
    m_pdvChart = NULL;
  }
  if ( NULL != m_pInstrument ) {
    delete m_pInstrument;
    m_pInstrument = NULL;
  }
}

void CBasketTradeSymbolBase::StreamSymbolInfo(std::ostream *pStream) {
  *pStream << m_status.sSymbolName << "," << m_sPath << "," << m_sStrategy;
  //int i = pStream->gcount();
  //pStream->rdbuf()->pubsync
}

void CBasketTradeSymbolBase::Initialize( void ) {

  assert( 0 < m_status.sSymbolName.length() );

  CInstrumentFile file;
  file.OpenIQFSymbols();
  try {
    m_pInstrument = file.CreateInstrumentFromIQFeed( m_status.sSymbolName, m_status.sSymbolName );  // todo:  need to verify proper symbol usage
  }
  catch (...) {
    std::cout << "CBasketTradeSymbolInfo::Initialize problems" << std::endl;
  }
  file.CloseIQFSymbols();

  m_pdvChart = new CChartDataView( "Basket", m_status.sSymbolName );

}