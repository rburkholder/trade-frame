/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "ZigZag.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ZigZag::ZigZag( void ) : 
  m_dblFilterWidth( 1.0 ), 
  m_PatternState( EDirection::Init ),
  m_dblPatternPt0( 0 ), m_dblPatternPt1( 0 ),
  m_cntNewUp( 0 ), m_cntNewDown( 0 ), m_cntTurns( 0 )
  {
}

ZigZag::ZigZag( double FilterWidth ) : 
  m_dblFilterWidth( FilterWidth ), 
  m_PatternState( EDirection::Init ),
  m_dblPatternPt0( 0 ), m_dblPatternPt1( 0 ),
  m_cntNewUp( 0 ), m_cntNewDown( 0 ), m_cntTurns( 0 )
  {
}

ZigZag::~ZigZag(void) {
}

void ZigZag::Check(boost::posix_time::ptime dt, double val) {
  double dif;

  switch ( m_PatternState ) {
    case EDirection::Up:
      m_dblPatternPt0 = val;
      if ( val > m_dblPatternPt1 ) {
        m_dblPatternPt1 = val;
        m_dtPatternPt1 = dt;
        ++m_cntNewUp;
        if ( NULL != UpDecisionPointFound ) UpDecisionPointFound( this );
      }
      dif = m_dblPatternPt1 - m_dblPatternPt0;
      if ( dif >= m_dblFilterWidth ) {
        ++m_cntTurns;
        if ( NULL != OnPeakFound ) OnPeakFound( *this, m_dtPatternPt1, m_dblPatternPt1, m_PatternState );
        if ( m_dblPatternPt1 > m_dblPatternPt0 ) m_PatternState = EDirection::Down;
      }
      break;
    case EDirection::Down:
      m_dblPatternPt0 = val;
      if ( val < m_dblPatternPt1 ) {
        m_dblPatternPt1 = val;
        m_dtPatternPt1 = dt;
        ++m_cntNewDown;
        if ( NULL != DnDecisionPointFound ) DnDecisionPointFound( this );
      }
      dif = m_dblPatternPt0 - m_dblPatternPt1;
      if ( dif >= m_dblFilterWidth ) {
        ++m_cntTurns;
        if ( NULL != OnPeakFound ) OnPeakFound( *this, m_dtPatternPt1, m_dblPatternPt1, m_PatternState );
        if ( m_dblPatternPt1 < m_dblPatternPt0 ) m_PatternState = EDirection::Up;
      }
      break;
    case EDirection::Init:
      m_dblPatternPt0 = val;
      m_dblPatternPt1 = val;
      m_dtPatternPt1 = dt;
      m_PatternState = EDirection::Start;
      if ( NULL != OnPeakFound ) OnPeakFound( *this, m_dtPatternPt1, m_dblPatternPt1, m_PatternState );
      break;
    case EDirection::Start:
      if ( abs( val - m_dblPatternPt1 ) >= m_dblFilterWidth ) {
        m_dtPatternPt1 = dt;
        m_dblPatternPt0 = val;
        if ( val > m_dblPatternPt1 ) {
          m_PatternState = EDirection::Up;
        }
        else {
          m_PatternState = EDirection::Down;
        }
        m_dblPatternPt1 = val;
      }
      break;
  }
}

//=================

ZigZagTotalMovement::ZigZagTotalMovement( CQuotes& quotes, double width )
  : ZigZag( width ), m_quotes( quotes ), m_sum( 0.0 ), m_last( 0.0 )
{
  ZigZag::SetOnPeakFound( MakeDelegate( this, &ZigZagTotalMovement::HandlePeakFound ) );
  m_quotes.OnAppend.Add( MakeDelegate( this, &ZigZagTotalMovement::HandleQuote ) );
}

ZigZagTotalMovement::~ZigZagTotalMovement( void ) {
  m_quotes.OnAppend.Remove( MakeDelegate( this, &ZigZagTotalMovement::HandleQuote ) );
  ZigZag::SetOnPeakFound( 0 );
}

void ZigZagTotalMovement::HandleQuote( const CQuote& quote ) {
  ZigZag::Check( quote.DateTime(), quote.Midpoint() );
}

void ZigZagTotalMovement::HandlePeakFound( ZigZag& zigzag, ptime dt, double val, ZigZag::EDirection direction ) {
  switch ( direction ) {
  case EDirection::Start:
    break;
  case EDirection::Up:
    m_sum += m_last - val;
    break;
  case EDirection::Down:
    m_sum += val - m_last;
    break;
  }
  m_last = val;
}

} // namespace tf
} // namespace ou
