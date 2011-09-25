/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include <iostream>

#include <TFTimeSeries/TimeSeries.h>
#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include "RunningStats.h"
#include "Pivots.h"

#include "CalcAboveBelow.h"

class CalcRangeStats {
public:
  CalcRangeStats( void ) : 
    m_dblTotalOfOpen( 0 ), m_dblTotalOfClose( 0 ),
    m_nCount( 0 ),
    m_nCountAbove( 0 ), m_nCountBelow( 0 ),
    m_dblAbove( 0 ), m_dblBelow( 0 ),
    m_nCountUp( 0 ), m_nCountDown( 0 )//, m_dblRange( 0 )
  {
    std::cout << "=================" << std::endl;
  };
  ~CalcRangeStats( void ) {
    std::cout << "*****************" << std::endl;
  };
  void operator() ( const CBar &bar ) {
    std::cout.setf( std::ios::fixed, std::ios::floatfield );
    std::cout 
      << bar.m_dt 
      << std::setprecision( 2 )
      << std::setw( 6 ) << bar.m_dblOpen
      << std::setw( 6 ) << bar.m_dblHigh
      << std::setw( 6 ) << bar.m_dblLow
      << std::setw( 6 ) << bar.m_dblClose 
      << " "            << bar.m_nVolume
      << std::endl;
    ++m_nCount;
    m_dblTotalOfOpen += bar.m_dblOpen;
    m_dblTotalOfClose += bar.m_dblClose;
    m_dblAbove += bar.m_dblHigh - bar.m_dblOpen;
    m_dblBelow += bar.m_dblOpen - bar.m_dblLow;
    //m_dblRange += bar.m_dblHigh - bar.m_dblLow;
    m_statsRange.Add( m_nCount, bar.m_dblHigh - bar.m_dblLow );
    if ( bar.m_dblOpen > bar.m_dblClose ) ++m_nCountDown;
    if ( bar.m_dblOpen < bar.m_dblClose ) ++m_nCountUp;
  };
  operator double() { 
    //m_statsAbove.CalcStats();
    //m_statsBelow.CalcStats();
    m_statsRange.CalcStats();
    std::cout << "Stats:  "
      << std::setprecision( 5 )
      << std::setw( 6 )
      <<  " ave above=" << m_dblAbove / m_nCount
      << ", ave below=" << m_dblBelow / m_nCount
      << ", ave range=" << m_statsRange.meanY 
      << std::setprecision( 2 )
      << ":" << m_statsRange.RR
      << std::setprecision( 5 )
      << ", count=" << m_nCount
      << ", up=" << m_nCountUp
      << ", down=" << m_nCountDown
      << ", ave open=" << m_dblTotalOfOpen / m_nCount
      << ", ave close=" << m_dblTotalOfClose / m_nCount
      << ", sd=" << m_statsRange.SD
      << std::endl;
    return m_statsRange.SD; 
  };
protected:
private:
  double m_dblTotalOfOpen;
  double m_dblTotalOfClose;
  double m_dblAbove;
  double m_dblBelow;
  //double m_dblRange;
  unsigned long m_nCount;
  unsigned long m_nCountAbove;
  unsigned long m_nCountBelow;
  unsigned long m_nCountUp;
  unsigned long m_nCountDown;
  //CRunningStats m_statsAbove;
  //CRunningStats m_statsBelow;
  CRunningStats m_statsRange;
};

//
// CCalcAboveBelow
//

CCalcAboveBelow::CCalcAboveBelow( CInstrument::pInstrument_t pInstrument, CProviderInterface* pDataProvider, CProviderInterface* pExecutionProvider ) 
: m_pInstrument( pInstrument ), m_pDataProvider( pDataProvider), m_pExecutionProvider( pExecutionProvider ),
  m_dblLast( 0 )
{
  m_pDataProvider->AddTradeHandler( m_pInstrument->GetSymbolName(), MakeDelegate( this, &CCalcAboveBelow::HandleTrade ) );
  m_pExecutionProvider->OnUpdatePortfolioRecord.Add( MakeDelegate( this, &CCalcAboveBelow::HandleUpdatePortfolioRecord ) );
}

CCalcAboveBelow::~CCalcAboveBelow(void) {
  m_pExecutionProvider->OnUpdatePortfolioRecord.Remove( MakeDelegate( this, &CCalcAboveBelow::HandleUpdatePortfolioRecord ) );
  m_pDataProvider->RemoveTradeHandler( m_pInstrument->GetSymbolName(), MakeDelegate( this, &CCalcAboveBelow::HandleTrade ) );
}

void CCalcAboveBelow::Start( void ) {
  string sPath;
  CHDF5DataManager::DailyBarPath( m_pInstrument->GetSymbolName(), sPath );
  std::cout << "Processing " << m_pInstrument->GetSymbolName() << " in " << sPath << std::endl;
  //string sPath( "/bar/86400/I/C/" + m_sSymbol );
  CHDF5TimeSeriesContainer<CBar> barRepository( sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  end = barRepository.end();
  if ( 20 > ( end - barRepository.begin() ) ) {
    std::cout << m_pInstrument->GetSymbolName() << " does not have 20 or more daily bars" << std::endl;
  }
  else {
    CTimeSeries<CBar> m_bars;
    begin = end - 20;
    m_bars.Resize( 20 );
    barRepository.Read( begin, end, &m_bars );
    double sd = std::for_each( m_bars.begin(), m_bars.end(), CalcRangeStats() );

    CBar bar( *m_bars.Last() );
    std::cout << "Trade points:  "
      << std::setprecision( 5 )
      << std::setw( 6 )
      <<         bar.m_dblClose - 3 * sd
      << ", " << bar.m_dblClose - 2 * sd
      << ", " << bar.m_dblClose - 1 * sd
      << ", " << bar.m_dblClose 
      << ", " << bar.m_dblClose + 1 * sd
      << ", " << bar.m_dblClose + 2 * sd
      << ", " << bar.m_dblClose + 3 * sd
      << std::endl;
    CPivotSet pivot( m_pInstrument->GetSymbolName(), bar.m_dblHigh, bar.m_dblLow, bar.m_dblClose );
    std::cout << "Pivots: "
      << "S2=" << pivot.GetPivotValue( CPivotSet::S2 )
      << " S1=" << pivot.GetPivotValue( CPivotSet::S1 )
      << " PV=" << pivot.GetPivotValue( CPivotSet::PV )
      << " R1=" << pivot.GetPivotValue( CPivotSet::R1 )
      << " R2=" << pivot.GetPivotValue( CPivotSet::R2 )
      << std::endl;
  }
}

void CCalcAboveBelow::Stop( void ) {
}

void CCalcAboveBelow::HandleUpdatePortfolioRecord(CPortfolio::UpdatePortfolioRecord_t rec) {
  std::cout << "cab:  " 
    << rec.pInstrument->GetSymbolName() 
    << " " << rec.nPosition 
    << " " << rec.dblPrice
    << " " << rec.dblAverageCost
    << std::endl;
}

void CCalcAboveBelow::HandleTrade( const CTrade &trade ) {
  m_dblLast = trade.m_dblTrade;
}
