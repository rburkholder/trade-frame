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
     pInstrument_t pTrin, pInstrument_t pTick, pInstrument_t pIndex,
     pProvider_t pDataProvider
   ) 
   : m_bStarted( false ), m_pTrin( pTrin ), m_pTick( pTick ), m_pIndex( pIndex ), m_pProvider( pDataProvider ),
     m_zzTrin( 0.15), m_zzIndex( 5.0 ),
     m_bFirstIndexFound( false ), m_dblFirstIndex( 0.0 ),
     m_nPixelsX( 500 ), m_nPixelsY( 200 )

{
  m_bfIndex.SetOnBarUpdated( MakeDelegate( this, &IndicatorPackage::HandleOnBFIndexUpdated ) );
  m_bfIndex.SetOnBarComplete( MakeDelegate( this, &IndicatorPackage::HandleOnBFIndexComplete ) );
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

void IndicatorPackage::HandleOnTrin( const ou::tf::Trade& trade ) {
  //m_tradesTrin.Append( trade );
  m_dblTrin = trade.Price();
  m_zzTrin.Check( trade.DateTime(), m_dblTrin );
}

void IndicatorPackage::HandleOnTick( const ou::tf::Trade& trade ) {
  //m_tradesTick.Append( trade );
}

void IndicatorPackage::HandleOnIndex( const ou::tf::Trade& trade ) {
  //m_tradesIndex.Append( trade );
  if ( !m_bFirstIndexFound ) {
    m_bFirstIndexFound = true;
    m_dblFirstIndex = trade.Price();
  }
  m_dblOfsIdx = trade.Price() - m_dblFirstIndex;
  m_zzIndex.Check( trade.DateTime(), m_dblOfsIdx );
  m_bfIndex.Add( trade );
}

void IndicatorPackage::HandleOnBFIndexUpdated( const ou::tf::Bar& ) {
}

void IndicatorPackage::HandleOnBFIndexComplete( const ou::tf::Bar& ) {
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
}

void IndicatorPackage::DrawChartIndex( void ) {
}

void IndicatorPackage::DrawChartArms( void ) {

  if ( m_bTrinOfZZPairReady && m_bIndexOfZZPairReady ) {
    m_vTrin.push_back( m_dblZZTrin );
    m_vOfsIdx.push_back( m_dblZZIndex );
    m_bTrinOfZZPairReady = m_bIndexOfZZPairReady = false;
  }

  m_vTrin.push_back( m_dblTrin );
  m_vOfsIdx.push_back( m_dblOfsIdx );

  XYChart chart( m_widthXChartArms, m_widthYChartArms );

  // do an overlay for the leading line, so can get a different color on it
  chart.addTitle( "Trin vs Delta Indu" );
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