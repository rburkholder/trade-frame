#include "StdAfx.h"
#include "ChartRealTimeModel.h"

//
// CChartRealTimeModel
//

//   OnDataChanged( this );  might be called way too often

CChartRealTimeModel::CChartRealTimeModel(void) {
  m_ceAsks.Reserve( 50000 );
  m_ceBids.Reserve( 50000 );
  m_ceTrades.Reserve( 50000 );
  m_barFactory.SetBarWidth( 6 );
  m_barFactory.SetOnNewBarStarted( MakeDelegate( this, &CChartRealTimeModel::HandleNewBarStarted ) );
  m_barFactory.SetOnBarUpdated( MakeDelegate( this, &CChartRealTimeModel::HandleBarUpdated ) );
  m_barFactory.SetOnBarComplete( MakeDelegate( this, &CChartRealTimeModel::HandleBarCompleted ) );
}

CChartRealTimeModel::~CChartRealTimeModel(void) {
}

void CChartRealTimeModel::AddQuote(const CQuote &quote) {
  m_vQuotes.push_back( quote );
  m_ceAsks.Add( quote.m_dt, quote.m_dblAsk );
  m_ceBids.Add( quote.m_dt, quote.m_dblBid );
  m_ceSpreadMidPoint.Add( quote.m_dt, ( quote.m_dblAsk + quote.m_dblBid ) / 2 );
  OnQuote( this );
}

void CChartRealTimeModel::AddTrade(const CTrade &trade) {
  m_ceTrades.Add( trade.m_dt, trade.m_dblTrade );
  m_vTrades.push_back( trade );
  m_barFactory.Add( trade );
  OnTrade( this );
}

void CChartRealTimeModel::HandleNewBarStarted(const CBar &bar) {
  m_barUpdating = bar;
  OnBarStarted( this );
}

void CChartRealTimeModel::HandleBarUpdated(const CBar &bar) {
  m_barUpdating = bar;
  OnBarUpdated( this );
}

void CChartRealTimeModel::HandleBarCompleted(const CBar &bar) {
  m_vBars.push_back( bar );
  m_ceBars.AddBar( bar );
  OnBarComplete( this );
}