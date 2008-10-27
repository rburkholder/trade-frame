#include "StdAfx.h"
#include "BasketTradeSymbolBase.h"

#include "InstrumentFile.h"
#include "HDF5TimeSeriesContainer.h"
#include "HDF5WriteTimeSeries.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBasketTradeSymbolBase::CBasketTradeSymbolBase(
  const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy
  ) 
: m_status( sSymbolName ), m_sPath( sPath ),  m_sStrategy( sStrategy ), 
  m_pInstrument( NULL ), m_pdvChart( NULL ), m_pModelParameters( NULL )
{
  Initialize();
}

CBasketTradeSymbolBase::CBasketTradeSymbolBase( std::stringstream *pStream ) 
: m_pInstrument( NULL ), m_pdvChart( NULL ), m_pModelParameters( NULL )
{
  *pStream >> m_status.sSymbolName >> m_sPath >> m_sStrategy;
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

void CBasketTradeSymbolBase::CalculateTrade( structCommonModelInformation *pParameters ) {

  m_pModelParameters = pParameters;
  m_dblProposedEntryCost = 0;
  CHDF5TimeSeriesContainer<CBar> barRepository( m_sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dt3MonthsAgo = pParameters->dtTradeDate - date_duration( 365 / 4 );
  ptime dt6MonthsAgo = pParameters->dtTradeDate - date_duration( 365 / 2 );
  begin = lower_bound( barRepository.begin(), barRepository.end(), dt6MonthsAgo );
  end = lower_bound( begin, barRepository.end(), pParameters->dtTradeDate );
  hsize_t cnt = end - begin;
  CBars bars;
  bars.Resize( cnt );
  barRepository.Read( begin, end, &bars );

  std::cout << "Entry for " << m_status.sSymbolName;
  if ( 20 < cnt ) { // needs to be 21 or more for ATR

    double dblClose = bars.Last()->m_dblClose;  
    m_nQuantityForEntry = ( ( (int) ( m_pModelParameters->dblFunds / dblClose ) ) / 100 ) * 100;
    m_dblProposedEntryCost = m_nQuantityForEntry * bars.Last()->m_dblClose;
    std::cout  
      << ": " << m_nQuantityForEntry << "@" << m_dblProposedEntryCost 
      << ", " << dblClose
      << std::endl;

    if ( structCommonModelInformation::Final == m_pModelParameters->nCalcStep ) {
      ModelReady( &bars );
      // calc Average Daily Range, calc Averge True Range at some point in time
      double range = 0;
      CBar *pBar;
      for ( int ix = cnt - 20; ix < cnt; ++ix ) {
        pBar = bars[ ix ];
        range += pBar->m_dblHigh - pBar->m_dblLow;
      }
      m_status.dblAvgDailyRange = range / 20.0;
    }
  }
  else {
    m_nQuantityForEntry = 0;
    m_dblProposedEntryCost = 0;
    std::cout << m_status.sSymbolName << " didn't have enough bars" << std::endl;
  }
}

void CBasketTradeSymbolBase::AddTradeHandler(CSymbol::tradehandler_t handler) {
  m_pModelParameters->pDataProvider->AddTradeHandler( m_status.sSymbolName, handler );
}

void CBasketTradeSymbolBase::AddQuoteHandler(CSymbol::quotehandler_t handler) {
  m_pModelParameters->pDataProvider->AddQuoteHandler( m_status.sSymbolName, handler );
}

void CBasketTradeSymbolBase::AddOpenHandler(CSymbol::tradehandler_t handler) {
  m_pModelParameters->pDataProvider->AddOnOpenHandler( m_status.sSymbolName, handler );
}

void CBasketTradeSymbolBase::RemoveTradeHandler(CSymbol::tradehandler_t handler) {
  m_pModelParameters->pDataProvider->RemoveTradeHandler( m_status.sSymbolName, handler );
}

void CBasketTradeSymbolBase::RemoveQuoteHandler(CSymbol::quotehandler_t handler) {
  m_pModelParameters->pDataProvider->RemoveQuoteHandler( m_status.sSymbolName, handler );
}

void CBasketTradeSymbolBase::RemoveOpenHandler(CSymbol::tradehandler_t handler) {
  m_pModelParameters->pDataProvider->RemoveOnOpenHandler( m_status.sSymbolName, handler );
}

void CBasketTradeSymbolBase::PlaceOrder( COrder *pOrder ) {
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolBase::HandleOrderFilled ) );
  m_OrderManager.PlaceOrder( m_pModelParameters->pExecutionProvider, pOrder );
}

void CBasketTradeSymbolBase::HandleOrderFilled(COrder *pOrder) {
  // make the assumption that the order arriving is the order we are expecting, ie no multiple or cancelled orders
  // at some point, possibly in different basket algorithm, will need to handle cancelled orders
  pOrder->OnOrderFilled.Remove( MakeDelegate( this, &CBasketTradeSymbolBase::HandleOrderFilled ) );
  m_status.dblFilledPrice = pOrder->GetAverageFillPrice();
  double dblPreviousAverageCost;
  bool bClosing;

  switch ( pOrder->GetOrderSide() ) {
    case OrderSide::Buy:
      bClosing = m_status.nPositionSize < 0;  // we are short, with a buy, closing all or part of position, => realized PL
      dblPreviousAverageCost = m_status.dblAverageCost;
      m_status.nPositionSize += pOrder->GetQuanFilled();
      //m_status.dblPositionSize += pOrder->GetQuanFilled() * pOrder->GetAverageFillPrice();
      m_status.dblPositionSize = m_status.nPositionSize * pOrder->GetAverageFillPrice();
      m_status.dblAverageCost = ( 0 == m_status.nPositionSize ) ? 0 : m_status.dblPositionSize / m_status.nPositionSize;
      if ( bClosing ) {
        m_status.dblRealizedPL += ( dblPreviousAverageCost - pOrder->GetAverageFillPrice() ) * pOrder->GetQuanFilled();
      }
      break;
    case OrderSide::Sell:
      bClosing = m_status.nPositionSize > 0;  // we are long, with a sell, closing all or part of postion, => realized PL
      dblPreviousAverageCost = m_status.dblAverageCost;
      m_status.nPositionSize -= pOrder->GetQuanFilled();
      //m_status.dblPositionSize -= pOrder->GetQuanFilled() * pOrder->GetAverageFillPrice();
      m_status.dblPositionSize = m_status.nPositionSize * pOrder->GetAverageFillPrice();
      m_status.dblAverageCost = ( 0 == m_status.nPositionSize ) ? 0 : m_status.dblPositionSize / m_status.nPositionSize;
      if ( bClosing ) {
        m_status.dblRealizedPL += ( pOrder->GetAverageFillPrice() - dblPreviousAverageCost ) * pOrder->GetQuanFilled();
      }
      break;
  }
}

void CBasketTradeSymbolBase::WriteTradesAndQuotes(const std::string &sPathPrefix) {
  if ( 0 != m_trades.Count() ) {
    CHDF5WriteTimeSeries<CTrades, CTrade> wts;
    wts.Write( sPathPrefix + "/trades/" + m_status.sSymbolName, &m_trades );
  }
  if ( 0 != m_quotes.Count() ) {
    CHDF5WriteTimeSeries<CQuotes, CQuote> wts;
    wts.Write( sPathPrefix + "/quotes/" + m_status.sSymbolName, &m_quotes );
  }
}