#include "StdAfx.h"
#include "BasketTradeSymbolSkeleton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PivotGroup.h"
#include "Log.h"

#include "HDF5TimeSeriesContainer.h"
#include "HDF5WriteTimeSeries.h"

CBasketTradeSymbolSkeleton::CBasketTradeSymbolSkeleton( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy ) 
: CBasketTradeSymbolBase( sSymbolName, sPath, sStrategy )
{
}

CBasketTradeSymbolSkeleton::~CBasketTradeSymbolSkeleton(void)  {
}

void CBasketTradeSymbolSkeleton::Initialize() {
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

void CBasketTradeSymbolSkeleton::ModelReady( CBars *pBars ) {
  CPivotGroup pivots( pBars );
  for ( CPivotGroup::const_iterator iter = pivots.begin(); iter != pivots.end(); ++iter ) {
    m_ceLevels.AddMark( iter->first, iter->second.colour, iter->second.sName.c_str() );
  }
}

void CBasketTradeSymbolSkeleton::StartTrading() {
  if ( NULL != m_pModelParameters ) {
    m_pModelParameters->pTreeView->Add( "BasketV2", m_status.sSymbolName, m_pdvChart );

    m_1MinBarFactory.SetBarWidth( m_nBarWidth );  
    m_1MinBarFactory.SetOnBarComplete( MakeDelegate( this, &CBasketTradeSymbolSkeleton::HandleBarFactoryBar ) );

    AddTradeHandler( MakeDelegate( this, &CBasketTradeSymbolSkeleton::HandleTrade ) );
    AddQuoteHandler( MakeDelegate( this, &CBasketTradeSymbolSkeleton::HandleQuote ) );
    AddOpenHandler( MakeDelegate( this, &CBasketTradeSymbolSkeleton::HandleOpen ) );
  }
}

void CBasketTradeSymbolSkeleton::StopTrading() {
  if ( NULL != m_pModelParameters ) {
    RemoveTradeHandler( MakeDelegate( this, &CBasketTradeSymbolSkeleton::HandleTrade ) );
    RemoveQuoteHandler( MakeDelegate( this, &CBasketTradeSymbolSkeleton::HandleQuote ) );
    RemoveOpenHandler( MakeDelegate( this, &CBasketTradeSymbolSkeleton::HandleOpen ) );

    m_pModelParameters->pTreeView->Remove( "BasketV2", m_status.sSymbolName );
  }
}

void CBasketTradeSymbolSkeleton::HandleOpen( const CTrade &trade ) {
  m_status.dblOpen = trade.m_dblTrade; // official open
  m_ceLevels.AddMark( trade.m_dblTrade, Colour::Plum, "Open" );
  m_bFoundOpeningTrade = true;
}

void CBasketTradeSymbolSkeleton::HandleQuote( const CQuote &quote ) {
  assert( 0 < quote.m_dblAsk );
  assert( 0 < quote.m_dblBid );
  m_quotes.AppendDatum( quote );
  if ( ( quote.m_dt >= m_pModelParameters->dtRTHBgn ) && ( quote.m_dt < m_pModelParameters->dtRTHEnd ) ) {
    m_ceQuoteAsks.Add( quote.m_dt, quote.m_dblAsk );
    m_ceQuoteBids.Add( quote.m_dt, quote.m_dblBid );
    m_pdvChart->SetChanged();
  }
}

void CBasketTradeSymbolSkeleton::HandleTrade(const CTrade &trade) {
  assert( 0 < trade.m_dblTrade );
  assert( 0 < trade.m_nTradeSize );
  m_trades.AppendDatum( trade );

  if ( ( trade.m_dt >= m_pModelParameters->dtRTHBgn ) && ( trade.m_dt < m_pModelParameters->dtRTHEnd ) ) {
    m_ceTrades.Add( trade.m_dt, trade.m_dblTrade );
    m_1MinBarFactory.Add( trade );
  }

  if ( m_bFoundOpeningTrade ) {
  }

  if ( 0 == m_status.nPositionSize ) {
    m_status.dblUnRealizedPL = 0;
  }
  else {
    if ( 0 < m_status.nPositionSize ) { // long
      m_status.dblUnRealizedPL = ( trade.m_dblTrade - m_status.dblAverageCost ) * m_status.nPositionSize;
    }
    else { // short
      m_status.dblUnRealizedPL = ( m_status.dblAverageCost - trade.m_dblTrade ) * -m_status.nPositionSize;
    }
  }

  OnBasketTradeSymbolChanged( this );
}

void CBasketTradeSymbolSkeleton::HandleBarFactoryBar(const CBar &bar) {
  m_bars.AppendDatum( bar );
  m_ceBars.AddBar( bar );
  m_ceBarVolume.Add( bar.m_dt, bar.m_nVolume );
}

void CBasketTradeSymbolSkeleton::HandleOrderFilled(COrder *pOrder) {

  CBasketTradeSymbolBase::HandleOrderFilled( pOrder );
}

