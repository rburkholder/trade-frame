#include "StdAfx.h"

#include "ZigZag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CZigZag::CZigZag( double FilterWidth ) : 
  m_dblFilterWidth( FilterWidth ), 
  m_PatternState( EDirection::Init ),
  m_dblPatternPt0( 0 ), m_dblPatternPt1( 0 ),
  cntNewUp( 0 ), cntNewDown( 0 ), cntTurns( 0 )
  {
}

CZigZag::~CZigZag(void) {
}

void CZigZag::Check(boost::posix_time::ptime dt, double val) {
  double dif;

  switch ( m_PatternState ) {
    case EDirection::Init:
      m_dblPatternPt0 = val;
      m_dblPatternPt1 = val;
      m_dtPatternPt1 = dt;
      m_PatternState = EDirection::Start;
      if ( NULL != OnPeakFound ) OnPeakFound( this, m_dtPatternPt1, m_dblPatternPt1, m_PatternState );
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
    case EDirection::Up:
      m_dblPatternPt0 = val;
      if ( val > m_dblPatternPt1 ) {
        m_dblPatternPt1 = val;
        m_dtPatternPt1 = dt;
        ++cntNewUp;
        if ( NULL != UpDecisionPointFound ) UpDecisionPointFound( this );
      }
      dif = m_dblPatternPt1 - m_dblPatternPt0;
      if ( dif >= m_dblFilterWidth ) {
        ++cntTurns;
        if ( NULL != OnPeakFound ) OnPeakFound( this, m_dtPatternPt1, m_dblPatternPt1, m_PatternState );
        if ( m_dblPatternPt1 > m_dblPatternPt0 ) m_PatternState = EDirection::Down;
      }
      break;
    case EDirection::Down:
      m_dblPatternPt0 = val;
      if ( val < m_dblPatternPt1 ) {
        m_dblPatternPt1 = val;
        m_dtPatternPt1 = dt;
        ++cntNewDown;
        if ( NULL != DnDecisionPointFound ) DnDecisionPointFound( this );
      }
      dif = m_dblPatternPt0 - m_dblPatternPt1;
      if ( dif >= m_dblFilterWidth ) {
        ++cntTurns;
        if ( NULL != OnPeakFound ) OnPeakFound( this, m_dtPatternPt1, m_dblPatternPt1, m_PatternState );
        if ( m_dblPatternPt1 < m_dblPatternPt0 ) m_PatternState = EDirection::Up;
      }
      break;
  }
}
