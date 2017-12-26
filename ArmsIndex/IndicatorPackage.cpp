/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "stdafx.h"

#include <OUCommon/TimeSource.h>

#include "IndicatorPackage.h"

IndicatorPackage::IndicatorPackage( 
     pProvider_t pDataProvider,
     pInstrument_t pInstIndex, pInstrument_t pInstTick, pInstrument_t pInstTrin
   ) 
   : m_bStarted( false ), m_pTrin( pInstTrin ), m_pTick( pInstTick ), m_pIndex( pInstIndex ), m_pProvider( pDataProvider ),
     m_zzTrin( 0.15), m_zzIndex( 5.0 ),
     m_bFirstIndexFound( false ), m_dblFirstIndex( 0.0 ),
     m_nPixelsX( 600 ), m_nPixelsY( 200 ), 
     m_dblTrin( 0 ), m_dblOfsIdx( 0 ), 
     m_bIndexOfZZPairReady( false ), m_bTrinOfZZPairReady( false ), 
     m_dblZZTrin( 0.0 ), m_dblZZIndex( 0.0 ), m_bDayView( true )

{

  // http://krazydad.com/tutorials/makecolors.php
  double freq = 0.1;
  for ( int i = 0.0; i < 32.0; ++i ) {
    int red = std::sin( i * freq + 0.0 ) * 127 + 128;
    int grn = std::sin( i * freq + 2.0 ) * 127 + 128;
    int blu = std::sin( i * freq + 4.0 ) * 127 + 128;
    m_vColours.push_back( ( ( ( red << 8 ) + grn ) << 8 ) + blu );
  }

  m_bfIndex.SetOnBarUpdated( MakeDelegate( &m_bdIndex, &BarDoubles::WorkingBar ) );
  m_bfIndex.SetOnBarComplete( MakeDelegate( &m_bdIndex, &BarDoubles::PushBack ) );
  m_bfTick.SetOnBarUpdated( MakeDelegate( &m_bdTicks, &BarDoubles::WorkingBar ) );
  m_bfTick.SetOnBarComplete( MakeDelegate( &m_bdTicks, &BarDoubles::PushBack ) );
  m_zzTrin.SetOnPeakFound( MakeDelegate( this, &IndicatorPackage::HandleOnZZTrinPeakFound ) );
  m_zzIndex.SetOnPeakFound( MakeDelegate( this, &IndicatorPackage::HandleOnZZIndexPeakFound ) );
  m_pProvider->AddTradeHandler( m_pTrin, MakeDelegate( this, &IndicatorPackage::HandleOnTrin ) );
  m_pProvider->AddTradeHandler( m_pTick, MakeDelegate( this, &IndicatorPackage::HandleOnTick ) );
  m_pProvider->AddTradeHandler( m_pIndex, MakeDelegate( this, &IndicatorPackage::HandleOnIndex ) );

  ptime now = ou::TimeSource::Instance().External();
  ptime dtBegin( 
        ou::TimeSource::Instance().ConvertRegionalToUtc( now.date(), time_duration( 9, 30 , 0 ), "America/New_York", true ) );
  ptime dtEnd( 
        ou::TimeSource::Instance().ConvertRegionalToUtc( now.date(), time_duration( 16, 0 , 0 ), "America/New_York", true ) );

  m_ctViewBegin = m_ctDayBegin = Chart::chartTime( dtBegin.date().year(), dtBegin.date().month(), dtBegin.date().day(),
                                     dtBegin.time_of_day().hours(), dtBegin.time_of_day().minutes(), dtBegin.time_of_day().seconds() );

  m_ctViewEnd   = m_ctDayEnd   = Chart::chartTime( dtEnd.date().year(), dtEnd.date().month(), dtEnd.date().day(),
                                     dtEnd.time_of_day().hours(), dtEnd.time_of_day().minutes(), dtEnd.time_of_day().seconds() );
  
//  std::cout << pInstIndex->GetInstrumentName() 
//            << ": " << dtBegin << "," << dtEnd 
//            << "; " << m_ctViewBegin << "," << m_ctViewEnd
//            << std::endl;

}

IndicatorPackage::~IndicatorPackage(void) {
  m_bfIndex.SetOnBarUpdated( 0 );
  m_bfIndex.SetOnBarComplete( 0 );
  m_zzTrin.SetOnPeakFound( 0 );
  m_zzIndex.SetOnPeakFound( 0 );
  m_pProvider->RemoveTradeHandler( m_pTrin, MakeDelegate( this, &IndicatorPackage::HandleOnTrin ) );
  m_pProvider->RemoveTradeHandler( m_pTick, MakeDelegate( this, &IndicatorPackage::HandleOnTick ) );
  m_pProvider->RemoveTradeHandler( m_pIndex, MakeDelegate( this, &IndicatorPackage::HandleOnIndex ) );
}

//void IndicatorPackage::Start( void ) {
//}

void IndicatorPackage::ToggleView( void ) {
  
  if ( m_bDayView ) {
    if ( 1 <= m_bdIndex.m_vTime.size() ) {
      m_bDayView = false;  // switch to short view
    }
    else {
      m_bDayView = true; // can't switch yet
      std::cout << "Not Enough Data Yet" << std::endl;
    }
  }
  else {
    m_bDayView = true;  // switch to day view
    m_ctViewBegin = m_ctDayBegin;
    m_ctViewEnd = m_ctDayEnd;
  }

}

void IndicatorPackage::HandleOnIndex( const ou::tf::Trade& trade ) {

  // update for arms chart
  if ( !m_bFirstIndexFound ) {
    m_bFirstIndexFound = true;
    m_dblFirstIndex = trade.Price();
  }
  m_dblOfsIdx = trade.Price() - m_dblFirstIndex;
  m_zzIndex.Check( trade.DateTime(), m_dblOfsIdx );

  // keep for bar chart
  if ( m_lfIndex.push( trade ) ) {
  }
  else {
    std::cout << "HandleOnIndex push problems" << std::endl;
  }
}

void IndicatorPackage::HandleOnTick( const ou::tf::Trade& trade ) {
  if ( m_lfTick.push( trade ) ) {
  }
  else {
    std::cout << "HandleOnTick push problems" << std::endl;
  }
}

void IndicatorPackage::HandleOnTrin( const ou::tf::Trade& trade ) {
  m_dblTrin = trade.Price();
  m_zzTrin.Check( trade.DateTime(), m_dblTrin );
}

void IndicatorPackage::HandleOnZZTrinPeakFound( const ou::tf::ZigZag&, ptime, double pt, ou::tf::ZigZag::EDirection ) {
  m_dblZZTrin = pt;
  m_bTrinOfZZPairReady = true;
  PushZZPair();
}

void IndicatorPackage::HandleOnZZIndexPeakFound( const ou::tf::ZigZag&, ptime, double pt, ou::tf::ZigZag::EDirection ) {
  m_dblZZIndex = pt;
  m_bIndexOfZZPairReady = true;
  PushZZPair();
}

void IndicatorPackage::PushZZPair( void ) {
  if ( m_bTrinOfZZPairReady && m_bIndexOfZZPairReady ) {
    ZZPair zz( m_dblZZIndex, m_dblZZTrin );
    m_lfIndexTrinPair.push( zz );
    m_bTrinOfZZPairReady = m_bIndexOfZZPairReady = false;
  }
}

void IndicatorPackage::SetChartDimensions( unsigned int x, unsigned int y ) {
  m_nPixelsX = x;
  m_nPixelsY = y;
}

void IndicatorPackage::PopData( void ) {

  ou::tf::Trade trade;

  while ( m_lfIndex.pop( trade ) ) {
    m_bfIndex.Add( trade );
  }

  while ( m_lfTick.pop( trade ) ) {
    m_bfTick.Add( trade );
  }

  ZZPair pair;

  while ( m_lfIndexTrinPair.pop( pair ) ) {
    m_vOfsIdx.push_back( pair.dblOfsIndx );
    m_vTrin.push_back( pair.dblTrin );
  }

}

void IndicatorPackage::DrawCharts( void ) {

  if ( !m_bDayView ) {
    ptime t( m_bdIndex.m_barWorking.DateTime() );
    m_ctViewEnd   = Chart::chartTime( t.date().year(), t.date().month(), t.date().day(),
                                      t.time_of_day().hours(), t.time_of_day().minutes() + 1, t.time_of_day().seconds() );

    m_ctViewBegin = Chart::chartTime( t.date().year(), t.date().month(), t.date().day(),
                                      t.time_of_day().hours(), t.time_of_day().minutes() - 60, t.time_of_day().seconds() );
  }

  DrawChartIndex();
  DrawChartArms();
  DrawChartTick();
}

void IndicatorPackage::DrawChart( BarDoubles& bd, const std::string& sName ) {
  if ( 0 < bd.m_vBarHigh.size() ) {
    bd.PushWorkingBar();
    XYChart chart( m_nPixelsX, m_nPixelsY );
    chart.addTitle( sName.c_str() );
    chart.setPlotArea( 40, 10, m_nPixelsX - 50, m_nPixelsY - 43, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1 );
    CandleStickLayer *candle = chart.addCandleStickLayer( 
      bd.High(), bd.Low(), bd.Open(), bd.Close(), 0x0000ff00, 0x00ff0000, 0xFFFF0001 );  // 0xff000000
    candle->setXData( bd.Time() );
    chart.xAxis()->setDateScale( m_ctViewBegin, m_ctViewEnd, 0, 0 );
    MemBlock m = chart.makeChart( BMP );
    if ( 0 != bd.m_cb ) bd.m_cb( m );
    bd.PopBack();
  }
}

void IndicatorPackage::DrawChartIndex( void ) {
  DrawChart( m_bdIndex, "Index" );
}

void IndicatorPackage::DrawChartTick( void ) {
  DrawChart( m_bdTicks, "Ticks" );
}

void IndicatorPackage::DrawChartArms( void ) {

  m_vTrin.push_back( m_dblTrin );
  m_vOfsIdx.push_back( m_dblOfsIdx );

  XYChart chart( m_nPixelsX, m_nPixelsY );

  // do an overlay for the leading line, so can get a different color on it
  chart.addTitle( "Trin vs Delta Index" );
  chart.setPlotArea( 40, 10, m_nPixelsX - 50, m_nPixelsY - 35, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1 );
  chart.yAxis()->setLogScale( true );
  chart.addText(  45,  20, "NormDecl" );
  chart.addText(  45, m_nPixelsY / 2 - 15, "WeakDecl" );
  chart.addText(  45, m_nPixelsY - 45, "UpSoon" );
  chart.addText( m_nPixelsX / 2, m_nPixelsY - 45, "UpSn!!" );
  chart.addText( m_nPixelsX - 70, m_nPixelsY - 45, "NormAdv" );
  chart.addText( m_nPixelsX - 70,  m_nPixelsY / 2 - 15, "WeakAdv" );
  chart.addText( m_nPixelsX - 70,  20, "DnSoon" );
  chart.addText( m_nPixelsX / 2,  20, "DnSn!!" );

  vColours_t::const_iterator iterc( m_vColours.begin() );

/*  DoubleArray daTrin( &m_vTrin.front(), m_vTrin.size() );
  DoubleArray daIndu( &m_vOfsIdx.front(), m_vOfsIdx.size() );

  LineLayer* pLLIndu = chart.addLineLayer( daTrin, 0x33ff33, "Indu" );
  pLLIndu->setXData( daIndu );
  pLLIndu->setLineWidth( 3 );
  */

  typedef vDouble_t::const_reverse_iterator iterDoubles;
  iterDoubles iterx1( m_vOfsIdx.rbegin() );
  iterDoubles iterx2( iterx1 ); ++iterx2;
  iterDoubles itery1( m_vTrin.rbegin() );
  iterDoubles itery2( itery1 ); ++itery2;
  while ( m_vOfsIdx.rend() != iterx2 ) {
    DoubleArray dax( &(*iterx2), 2 );
    DoubleArray day( &(*itery2), 2 );
    LineLayer* pLLIndu = chart.addLineLayer( day, *iterc, "Trin" );
    pLLIndu->setXData( dax );
    pLLIndu->setLineWidth( 2 );
    pLLIndu->setBorderColor( 0xff000000 );
    //pLLIndu->moveFront();
    ++iterx1; ++iterx2; ++itery1; ++itery2;
    ++iterc;  
    if ( m_vColours.end() == iterc ) --iterc;
  }

  MemBlock m = chart.makeChart( BMP );
  if ( 0 != m_OnDrawChartArms ) m_OnDrawChartArms( m );

  m_vTrin.pop_back();
  m_vOfsIdx.pop_back();
}