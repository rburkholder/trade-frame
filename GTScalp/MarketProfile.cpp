#include "StdAfx.h"
//#include "GTScalp.h"
#include "MarketProfile.h"
#include <math.h>
#include <iostream>
using namespace std;

CMarketProfile::CMarketProfile(void) {
  m_nMin = m_nMax = 0;
  m_pBars = new CBars( 20 );
  m_bf.SetBarWidth( 30 * 60 );  // 30 minutes
  m_nBucketSize = 5;
  m_bf.SetOnNewBar( MakeDelegate( this, &CMarketProfile::OnNewBar ) );
  m_bf.SetOnBarUpdated( MakeDelegate( this, &CMarketProfile::OnBarUpdate ) );
}

CMarketProfile::~CMarketProfile(void) {
  delete m_pBars;
  for each ( pair<int, string *> p in m_mapProfile ) {
    delete p.second;
  }
}

void CMarketProfile::Add(const CTrade &trade) {
  m_bf.Add( trade.m_dt, trade.m_dblTrade, trade.m_nTradeSize );
}

void CMarketProfile::OnNewBar( const CBar &bar ) {
}

void CMarketProfile::OnBarUpdate( const CBar &bar ) {
}

void CMarketProfile::Draw() {
  for each ( pair<int, string *> p in m_mapProfile ) {
    delete p.second;
  }
  m_mapProfile.clear();
  map<int, string *>::iterator iter;
  char ch = 'A';
  CBar *pBar = m_pBars->First();
  while ( NULL != pBar ) {
    int Hi = (int) (pBar->m_dblHigh * 100.0 + 0.5); // scale and round to the nearest penny
    int Lo = (int) (pBar->m_dblLow * 100.0 + 0.5);  // scale and round to the nearest penny
    Hi -= div( Hi, m_nBucketSize ).rem;
    Lo -= div( Lo, m_nBucketSize ).rem;
    m_nMin = ( 0 == m_nMin ) ? Lo : min( m_nMin, Lo );
    m_nMax = ( 0 == m_nMax ) ? Hi : max( m_nMax, Hi );

    string *pStr;
    for ( int i = Lo; i <= Hi; i += m_nBucketSize ) {
      iter = m_mapProfile.find( i );
      if ( m_mapProfile.end() == iter ) {
        pStr = new string;
        m_mapProfile.insert( m_mapProfile_Pair( i, pStr ) );
      }
      else {
        pStr = iter->second;
      }
      //pStr->append( ch );
      pStr->push_back( ch );
    }
    ++ch;
    m_pBars->Next();
  }

  CString s;
  
  for each ( pair<int, string *> p in m_mapProfile ) {
    s.Format( "%f0.2: %s", ( (double) p.first ) / 100.0, *p.second );
    //theApp.pConsoleMessages->WriteLine( s );
    cout << s << endl;
    //theApp.pConsoleMessages->Write( itoa( p.first ) );
    //theApp.pConsoleMessages->Write( ":  " );
    //theApp.pConsoleMessages->WriteLine( *p.second );
  }
}
