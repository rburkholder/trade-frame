#include "StdAfx.h"
#include "ChartRealTimeModel.h"


//
// CChartRealTimeModel
//

CChartRealTimeModel::CChartRealTimeModel(void) {
  m_barFactory.SetBarWidth( 60 );
  m_barFactory.SetOnNewBarStarted( MakeDelegate( this, &CChartRealTimeModel::HandleNewBarStarted ) );
  m_barFactory.SetOnBarUpdated( MakeDelegate( this, &CChartRealTimeModel::HandleBarUpdated ) );
  m_barFactory.SetOnBarComplete( MakeDelegate( this, &CChartRealTimeModel::HandleBarCompleted ) );
}

CChartRealTimeModel::~CChartRealTimeModel(void) {
}

void CChartRealTimeModel::AddQuote(const CQuote &quote) {
  m_vQuotes.push_back( quote );
}

void CChartRealTimeModel::AddTrade(const CTrade &trade) {
  m_vTrades.push_back( trade );
  m_barFactory.Add( trade );
}

void CChartRealTimeModel::HandleNewBarStarted(const CBar &bar) {
  m_barUpdating = bar;
}

void CChartRealTimeModel::HandleBarUpdated(const CBar &bar) {
  m_barUpdating = bar;
}

void CChartRealTimeModel::HandleBarCompleted(const CBar &bar) {
  m_vBars.push_back( bar );
}