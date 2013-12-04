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

#include "StdAfx.h"

#include "IndicatorPackage.h"

IndicatorPackage::IndicatorPackage( 
     pProvider_t pDataProvider,
     pInstrument_t pInstIndex, pInstrument_t pInstTick, pInstrument_t pInstTrin
   ) 
   : m_bStarted( false ), m_pTrin( pInstTrin ), m_pTick( pInstTick ), m_pIndex( pInstIndex ), m_pProvider( pDataProvider ),
     m_zzTrin( 0.15), m_zzIndex( 5.0 ),
     m_bFirstIndexFound( false ), m_dblFirstIndex( 0.0 ),
     m_nPixelsX( 600 ), m_nPixelsY( 200 )

{
  m_bfIndex.SetOnBarUpdated( MakeDelegate( &m_bdIndex, &BarDoubles::WorkingBar ) );
  m_bfIndex.SetOnBarComplete( MakeDelegate( &m_bdIndex, &BarDoubles::PushBack ) );
  m_bfTick.SetOnBarUpdated( MakeDelegate( &m_bdTicks, &BarDoubles::WorkingBar ) );
  m_bfTick.SetOnBarComplete( MakeDelegate( &m_bdTicks, &BarDoubles::PushBack ) );
  m_zzTrin.SetOnPeakFound( MakeDelegate( this, &IndicatorPackage::HandleOnZZTrinPeakFound ) );
  m_zzIndex.SetOnPeakFound( MakeDelegate( this, &IndicatorPackage::HandleOnZZIndexPeakFound ) );
  m_pProvider->AddTradeHandler( m_pTrin, MakeDelegate( this, &IndicatorPackage::HandleOnTrin ) );
  m_pProvider->AddTradeHandler( m_pTick, MakeDelegate( this, &IndicatorPackage::HandleOnTick ) );
  m_pProvider->AddTradeHandler( m_pIndex, MakeDelegate( this, &IndicatorPackage::HandleOnIndex ) );
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

void IndicatorPackage::HandleOnIndex( const ou::tf::Trade& trade ) {
  if ( !m_bFirstIndexFound ) {
    m_bFirstIndexFound = true;
    m_dblFirstIndex = trade.Price();
  }
  m_dblOfsIdx = trade.Price() - m_dblFirstIndex;
  m_bfIndex.Add( trade );
  m_zzIndex.Check( trade.DateTime(), m_dblOfsIdx );
}

void IndicatorPackage::HandleOnTrin( const ou::tf::Trade& trade ) {
  m_dblTrin = trade.Price();
  m_zzTrin.Check( trade.DateTime(), m_dblTrin );
}

void IndicatorPackage::HandleOnTick( const ou::tf::Trade& trade ) {
  m_bfTick.Add( trade );
}

void IndicatorPackage::HandleOnZZTrinPeakFound( const ou::tf::ZigZag&, ptime, double pt, ou::tf::ZigZag::EDirection ) {
  m_dblZZTrin = pt;
  m_bTrinOfZZPairReady = true;
}

void IndicatorPackage::HandleOnZZIndexPeakFound( const ou::tf::ZigZag&, ptime, double pt, ou::tf::ZigZag::EDirection ) {
  m_dblZZIndex = pt;
  m_bIndexOfZZPairReady = true;
}

void IndicatorPackage::SetChartDimensions( unsigned int x, unsigned int y ) {
  m_nPixelsX = x;
  m_nPixelsY = y;
}

void IndicatorPackage::DrawCharts( void ) {
  DrawChartIndex();
  DrawChartArms();
  DrawChartTick();
}

void IndicatorPackage::DrawChart( BarDoubles& bd, const std::string& sName ) {
  if ( 0 < bd.m_vBarHigh.size() ) {
    bd.PushWorkingBar();
    XYChart chart( m_nPixelsX, m_nPixelsY );
    chart.addTitle( sName.c_str() );
    chart.setPlotArea( 30, 10, 550, 130, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1 );
    CandleStickLayer *candle = chart.addCandleStickLayer( 
      bd.High(), bd.Low(), bd.Open(), bd.Close(), 0x0000ff00, 0x00ff0000, 0xFFFF0001 );  // 0xff000000
    candle->setXData( bd.Time() );
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

  if ( m_bTrinOfZZPairReady && m_bIndexOfZZPairReady ) {
    m_vTrin.push_back( m_dblZZTrin );
    m_vOfsIdx.push_back( m_dblZZIndex );
    m_bTrinOfZZPairReady = m_bIndexOfZZPairReady = false;
  }

  m_vTrin.push_back( m_dblTrin );
  m_vOfsIdx.push_back( m_dblOfsIdx );

  XYChart chart( m_nPixelsX, m_nPixelsY );

  // do an overlay for the leading line, so can get a different color on it
  chart.addTitle( "Trin vs Delta Index" );
  chart.setPlotArea( 30, 10, 550, 130, 0xffffff, -1, 0xc0c0c0, 0xc0c0c0, -1 );
  chart.yAxis()->setLogScale( true );
  chart.addText(  30,  15, "NormDecl" );
  chart.addText(  30,  60, "WeakDecl" );
  chart.addText(  30, 105, "UpSoon" );
  chart.addText( 250, 105, "UpSn!!" );
  chart.addText( 460, 105, "NormAdv" );
  chart.addText( 460,  60, "WeakAdv" );
  chart.addText( 470,  15, "DnSoon" );
  chart.addText( 250,  15, "DnSn!!" );

  DoubleArray daTrin( &m_vTrin.front(), m_vTrin.size() );
  DoubleArray daIndu( &m_vOfsIdx.front(), m_vOfsIdx.size() );

  LineLayer* pLLIndu = chart.addLineLayer( daTrin, 0x33ff33, "Indu" );
  pLLIndu->setXData( daIndu );
  pLLIndu->setLineWidth( 3 );

  MemBlock m = chart.makeChart( BMP );
  if ( 0 != m_OnDrawChartArms ) m_OnDrawChartArms( m );

  m_vTrin.pop_back();
  m_vOfsIdx.pop_back();
}