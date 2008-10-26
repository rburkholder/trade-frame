#include "StdAfx.h"
#include "BasketTradeSymbolV2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PivotGroup.h"
#include "Log.h"

#include "HDF5TimeSeriesContainer.h"
#include "HDF5WriteTimeSeries.h"

CBasketTradeSymbolV2::CBasketTradeSymbolV2( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy ) 
: CBasketTradeSymbolBase( sSymbolName, sPath, sStrategy )
{
}

CBasketTradeSymbolV2::~CBasketTradeSymbolV2(void)  {
}

void CBasketTradeSymbolV2::Initialize() {
  m_ceBarVolume.SetColour( Colour::Black );
  m_ceQuoteAsks.SetColour( Colour::Red );
  m_ceQuoteBids.SetColour( Colour::Blue );
  m_ceTrades.SetColour( Colour::Green );
  m_ceOrdersBuy.SetColour( Colour::Blue ); m_ceOrdersBuy.SetShape( CChartEntryShape::EBuy );
  m_ceOrdersSell.SetColour( Colour::Red ); m_ceOrdersSell.SetShape( CChartEntryShape::ESell );

  m_pdvChart->Add( 0, &m_ceBars );
  m_pdvChart->Add( 1, &m_ceBarVolume );
  m_pdvChart->Add( 0, &m_ceQuoteAsks );
  m_pdvChart->Add( 0, &m_ceQuoteBids );
  m_pdvChart->Add( 0, &m_ceTrades );
  //m_pdvChart->Add( 1, &m_ceTradeVolume );
  m_pdvChart->Add( 0, &m_ceLevels );
  m_pdvChart->Add( 0, &m_ceOrdersBuy );
  m_pdvChart->Add( 0, &m_ceOrdersSell );
}

void CBasketTradeSymbolV2::ModelReady( CBars *pBars ) {
  CPivotGroup pivots( pBars );
  for ( CPivotGroup::const_iterator iter = pivots.begin(); iter != pivots.end(); ++iter ) {
    m_ceLevels.AddMark( iter->first, iter->second.colour, iter->second.sName.c_str() );
  }
}

void CBasketTradeSymbolV2::StartTrading() {
  if ( NULL != m_pModelParameters ) {
    m_pModelParameters->pTreeView->Add( "BasketV2", m_status.sSymbolName, m_pdvChart );

    m_1MinBarFactory.SetBarWidth( m_nBarWidth );  
    m_1MinBarFactory.SetOnBarComplete( MakeDelegate( this, &CBasketTradeSymbolV2::HandleBarFactoryBar ) );

    AddTradeHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleTrade ) );
    AddQuoteHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleQuote ) );
    AddOpenHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleOpen ) );
  }
}

void CBasketTradeSymbolV2::StopTrading() {
  if ( NULL != m_pModelParameters ) {
    RemoveTradeHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleTrade ) );
    RemoveQuoteHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleQuote ) );
    RemoveOpenHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleOpen ) );

    m_pModelParameters->pTreeView->Remove( "BasketV2", m_status.sSymbolName );
  }
}

void CBasketTradeSymbolV2::HandleOpen( const CTrade &trade ) {
  m_status.dblOpen = trade.m_dblTrade; // official open
  m_ceLevels.AddMark( trade.m_dblTrade, Colour::Plum, "Open" );
  m_bFoundOpeningTrade = true;
}

void CBasketTradeSymbolV2::HandleQuote( const CQuote &quote ) {
  m_quotes.AppendDatum( quote );
  if ( ( quote.m_dt >= m_pModelParameters->dtRTHBgn ) && ( quote.m_dt < m_pModelParameters->dtRTHEnd ) ) {
    assert( 0 < quote.m_dblAsk );
    assert( 0 < quote.m_dblBid );
    m_ceQuoteAsks.Add( quote.m_dt, quote.m_dblAsk );
    m_ceQuoteBids.Add( quote.m_dt, quote.m_dblBid );
    m_pdvChart->SetChanged();
  }
}

void CBasketTradeSymbolV2::HandleTrade(const CTrade &trade) {
  assert( 0 < trade.m_dblTrade );
  assert( 0 < trade.m_nTradeSize );
  m_1MinBarFactory.Add( trade );




  OnBasketTradeSymbolChanged( this );
}

void CBasketTradeSymbolV2::HandleBarFactoryBar(const CBar &bar) {
  m_bars.AppendDatum( bar );
  m_ceBars.AddBar( bar );
  m_ceBarVolume.Add( bar.m_dt, bar.m_nVolume );
}

void CBasketTradeSymbolV2::HandleOrderFilled(COrder *pOrder) {

  CBasketTradeSymbolBase::HandleOrderFilled( pOrder );
}

