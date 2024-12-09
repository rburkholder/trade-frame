#include "StdAfx.h"

#include "MarketProfile.h"

#include <math.h>
#include <iostream>

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

CMarketProfile::CMarketProfile(void) 
: m_nMin( 0 ), m_nMax( 0 ), m_nBucketSize( 5 )
{
  m_pBars = new Bars( 20 );
  m_bf.SetBarWidth( 30 * 60 );  // 30 minutes
  m_bf.SetOnBarComplete( MakeDelegate( this, &CMarketProfile::OnNewBar ) );
  m_bf.SetOnBarUpdated( MakeDelegate( this, &CMarketProfile::OnBarUpdate ) );
}

CMarketProfile::~CMarketProfile(void) {
  delete m_pBars;
  for each ( pair<int, string *> p in m_mapProfile ) {
    delete p.second;
  }
}

void CMarketProfile::Add(const Trade &trade) {
  m_bf.Add( trade.m_dt, trade.m_dblTrade, trade.m_nTradeSize );
}

void CMarketProfile::OnNewBar( const Bar &bar ) {
}

void CMarketProfile::OnBarUpdate( const Bar &bar ) {
}

void CMarketProfile::Draw() {
  for each ( std::pair<int, std::string *> p in m_mapProfile ) {
    delete p.second;
  }
  m_mapProfile.clear();
  std::map<int, string *>::iterator iter;
  char ch = 'A';
  Bar* pBar = m_pBars->First();
  while ( NULL != pBar ) {
    int Hi = (int) (pBar->m_dblHigh * 100.0 + 0.5); // scale and round to the nearest penny
    int Lo = (int) (pBar->m_dblLow * 100.0 + 0.5);  // scale and round to the nearest penny
    Hi -= div( Hi, m_nBucketSize ).rem;
    Lo -= div( Lo, m_nBucketSize ).rem;
    m_nMin = ( 0 == m_nMin ) ? Lo : min( m_nMin, Lo );
    m_nMax = ( 0 == m_nMax ) ? Hi : max( m_nMax, Hi );

    std::string *pStr;
    for ( int i = Lo; i <= Hi; i += m_nBucketSize ) {
      iter = m_mapProfile.find( i );
      if ( m_mapProfile.end() == iter ) {
        pStr = new std::string;
        m_mapProfile.insert( m_mapProfile_Pair( i, pStr ) );
      }
      else {
        pStr = iter->second;
      }
      pStr->push_back( ch );
    }
    ++ch;
    m_pBars->Next();
  }

  char buf[30];
  
  for each ( std::pair<int, string *> p in m_mapProfile ) {
    sprintf( buf, "%f0.2: %s", ( (double) p.first ) / 100.0, *p.second );
    std::cout << buf << std::endl;
  }
}
