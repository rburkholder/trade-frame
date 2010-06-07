/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#pragma once

// pg 36 May 2005 Technical Analysis of Stocks and Commodities - Darvas Style Trading - Daryl Guppy
// pg 16 June 2005 Technical Analysis of Stocks and Commodities - Something Darvas, Something New - Daryl Guppy

#include <algorithm>

#include <LibTimeSeries/DatedDatum.h>

class CDarvasResults {
public:
  CDarvasResults( bool bTrigger, double dblStopLevel ): m_bTrigger( bTrigger ), m_dblStopLevel( dblStopLevel ) {};
  ~CDarvasResults( void ) {};
  bool GetTrigger( void ) { return m_bTrigger; };
  double GetStopLevel( void ) { return m_dblStopLevel; };
protected:
  bool m_bTrigger;
  double m_dblStopLevel;
private:
};

// is a functor
//class CDarvas: public std::unary_function<CBar &, void> {
template <typename T>
class CDarvas {
public:
  CDarvas<T>(void);
  ~CDarvas<T>(void);
  void operator()( const CBar& bar ) { Calc( bar ); };
  void Calc( const CBar& bar );
  void Clear( void ) { 
    ResetState(); 
    m_dblTop = m_dblBottom = m_dblStop = m_dblGhostHeight = m_cntBreakOuts = 0;
  };
protected:
  void ConservativeTrigger( void ) {};
  void AggressiveTrigger( void ) {};
  void SetStop( double stop ) {};
  void StopTrigger( void ) {};
  void BreakOutAlert( size_t ) {};
private:

  double m_dblTop;
	double m_dblBottom;
	double m_dblStop;
  double m_dblGhostHeight;

  size_t m_cntBreakOuts;

  enum enumStatesForFindingTop {
    ELookingForHigh, ELookingForLowerHigh1, ELookingForLowerHigh2, ELookingForLowerHigh3, ETopFound, EStopLooking
  } m_stateTop;
  enum enumStatesForFindingBottom {
     ELookingForBottom, ELookingForHigherLow1, ELookingForHigherLow2, ELookingForHigherLow3, EBottomFound
  } m_stateBottom;

  void ResetState( void );
    
};

template <typename T>
CDarvas<T>::CDarvas(void)
: m_stateTop( ELookingForHigh ), m_stateBottom( ELookingForBottom ),
  m_dblTop( 0 ), m_dblBottom( 0 ), m_dblStop( 0 ), m_dblGhostHeight( 0 ), m_cntBreakOuts( 0 )
{
}

template <typename T>
CDarvas<T>::~CDarvas(void) {
}

template <typename T>
void CDarvas<T>::ResetState( void ) {
  m_stateTop = ELookingForLowerHigh1;
  m_stateBottom = ELookingForBottom;
  if ( 0 != m_dblGhostHeight ) {
    m_dblStop = m_dblTop - m_dblGhostHeight;
  }
}

template <typename T>
void CDarvas<T>::Calc(const CBar& bar) {
  switch ( m_stateTop ) {
    case ELookingForHigh: 
      ResetState();
      m_dblTop = bar.High();
      break;
    case ELookingForLowerHigh1:
      if ( m_dblTop < bar.High() ) {
        ResetState();
        m_dblTop = bar.High();
      }
      else {
        m_stateTop = ELookingForLowerHigh2;
      }
      break;
    case ELookingForLowerHigh2:
      if ( m_dblTop < bar.High() ) {
        ResetState();
        m_dblTop = bar.High();
      }
      else {
        m_stateTop = ELookingForLowerHigh3;
      }
      break;
    case ELookingForLowerHigh3:
      if ( m_dblTop < bar.High() ) {
        ResetState();
        m_dblTop = bar.High();
      }
      else {
        m_stateTop = ETopFound;
      }
      break;
    case ETopFound:
      if ( m_dblTop < bar.Close() ) {
        if ( EBottomFound == m_stateBottom ) {
          // issue conservative trigger
          static_cast<T*>( this )->ConservativeTrigger();
          // issue breakout alerts
          ++m_cntBreakOuts;
          static_cast<T*>( this )->BreakOutAlert( m_cntBreakOuts );
        }
        ResetState();
        m_dblTop = bar.High();
      }
      break;
    case EStopLooking:
      break;
  };

  switch ( m_stateBottom ) {
    case ELookingForBottom:
      m_dblBottom = bar.Low();
      m_stateBottom = ELookingForHigherLow1;
      break;
    case ELookingForHigherLow1:
      if ( m_dblBottom <= bar.Low() ) {
        m_stateBottom = ELookingForHigherLow2;
      }
      else {
        m_dblBottom = bar.Low();
        m_stateBottom = ELookingForHigherLow1;  // yes stay in the same state
      }
      break;
    case ELookingForHigherLow2:
      if ( m_dblBottom <= bar.Low() ) {
        m_stateBottom = ELookingForHigherLow3;
      }
      else {
        m_dblBottom = bar.Low();
        m_stateBottom = ELookingForHigherLow1; 
      }
      break;
    case ELookingForHigherLow3:
      if ( m_dblBottom <= bar.Low() ) {
        m_stateBottom = EBottomFound;
        m_dblGhostHeight = m_dblTop - m_dblBottom;
//        if ( m_dblBottom > m_dblStop ) {  // considers if stop has been moved by GhostHeight
          m_dblStop = m_dblBottom;
//        }
        // issue set stop trigger
        static_cast<T*>( this )->SetStop( m_dblStop );
      }
      else {
        m_dblBottom = bar.Low();
        m_stateBottom = ELookingForHigherLow1; 
      }
      break;
    case EBottomFound:
      break;
  };

  if ( m_dblStop > bar.Close() ) {
    ResetState();
    m_stateTop = EStopLooking;
    // issue stop trigger
    static_cast<T*>( this )->StopTrigger();
  }

  // while ETopFound and EBottomFound, issue aggressive trigger
  if ( ( ETopFound == m_stateTop ) && ( EBottomFound == m_stateBottom ) ) {
    static_cast<T*>( this )->AggressiveTrigger();
  }

}



// conservative trigger:  on a breakout after establishing the box
// aggressive trigger:  once the box has been established, trade any time

// calculate number of rising darvas boxes, on second box, can be used as downtrend break out.