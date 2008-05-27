#include "StdAfx.h"

#include "BasketTradeSymbolInfo.h"

#include "TimeSeries.h"

#include "HDF5TimeSeriesContainer.h"


// 
// CSymbolInfo
//

CBasketTradeSymbolInfo::CBasketTradeSymbolInfo( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy ) 
: m_sSymbolName( sSymbolName ), m_sPath( sSymbolName ),  m_sStrategy( sStrategy )
{
}

CBasketTradeSymbolInfo::~CBasketTradeSymbolInfo( void ) {
}

void CBasketTradeSymbolInfo::CalculateTrade(ptime dtTradeDate, double dblFunds) {
  m_dtTradeDate = dtTradeDate;
  m_dblMaxAllowedFunds = dblFunds;
  CHDF5TimeSeriesContainer<CBar> barRepository( m_sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dt3MonthsAgo = dtTradeDate - date_duration( 365 / 4 );
  ptime dt6MonthsAgo = dtTradeDate - date_duration( 365 / 2 );
  begin = lower_bound( barRepository.begin(), barRepository.end(), dt6MonthsAgo );
  end = lower_bound( begin, barRepository.end(), dtTradeDate );
  hsize_t cnt = end - begin;
  CBars bars;
  bars.Resize( cnt );
  barRepository.Read( begin, end, &bars );
  //CBar bar = *bars.Last();
  double dblClose = bars.Last()->m_dblClose;
  m_nQuantityForEntry = ( ( (int) ( dblFunds / dblClose ) ) / 100 ) * 100;
  m_dblProposedEntryCost = m_nQuantityForEntry * bars.Last()->m_dblClose;
  std::cout << "Entry for " << m_sSymbolName 
    << ": " << m_nQuantityForEntry << "@" << m_dblProposedEntryCost 
    << ", " << dblClose
    << std::endl;}
