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

#include "StdAfx.h"

#include "Pivots.h"

const std::string CPivotSet::m_sPivotNames[ CPivotSet::PivotCount ] 
  = { "R3", "R23", "R2", "R12", "R1", "PVR1", "PV", "PVS1", "S1", "S12", "S2", "S23" "S3" };

const ou::Colour::enumColour CPivotSet::m_rPivotColours[ CPivotSet::PivotCount ] 
  = { ou::Colour::Tomato, ou::Colour::OrangeRed, ou::Colour::Orange, ou::Colour::RosyBrown, ou::Colour::Red, ou::Colour::Pink, 
      ou::Colour::DarkRed, 
      ou::Colour::BlueViolet, ou::Colour::Blue, ou::Colour::RoyalBlue, ou::Colour::Purple, ou::Colour::SkyBlue, ou::Colour::Violet };

CPivotSet::CPivotSet(void) 
 {
   for ( unsigned short ix = 0; ix < PivotCount; ++ix ) {
     m_rPivots[ ix ] = 0;
   }
}

CPivotSet::CPivotSet( const std::string &sName, double _S3, double _S2, double _S1, double _PV, double _R1, double _R2, double _R3 ) :
  m_sName( sName )
{
  m_rPivots[ R3 ] = _R3;
  m_rPivots[ R2 ] = _R2;
  m_rPivots[ R1 ] = _R1;
  m_rPivots[ PV ] = _PV;
  m_rPivots[ S1 ] = _S1;
  m_rPivots[ S2 ] = _S2;
  m_rPivots[ S3 ] = _S3;
  CalcHalfPivots();
}

CPivotSet::CPivotSet( const std::string &sName, CBars *bars ) {
  double hi = 0;
  double lo = 0;
  double cl = 0;
  size_t cnt = bars->Size();
  CBar *bar;
  if ( cnt > 0 ) {
    bar = bars->At( 0 );
    hi = bar->High();
    lo = bar->Low();
    cl = bar->Close();
    for ( unsigned int i = 1; i < cnt; i++ ) {
      bar = bars->At( i );
      hi = max( hi, bar->High() );
      lo = min( lo, bar->Low() );
      cl = bar->Close();
    }
  }
  CalcPivots( sName, hi, lo, cl );
}

CPivotSet::CPivotSet( const std::string &sName, double Hi, double Lo, double Close ) {
  CalcPivots( sName, Hi, Lo, Close );
}

CPivotSet::~CPivotSet() {
}

void CPivotSet::CalcPivots( const std::string &sName, double Hi, double Lo, double Close ) {
  m_sName = sName;
  double dif = Hi - Lo;
  m_rPivots[ PV ] = ( Hi + Lo + Close ) / 3;
  m_rPivots[ R1 ] = 2 * m_rPivots[ PV ] - Lo;
  m_rPivots[ R2 ] = m_rPivots[ PV ] + dif;
  m_rPivots[ R3 ] = m_rPivots[ R1 ] + dif;
  m_rPivots[ S1 ] = 2 * m_rPivots[ PV ] - Hi;
  m_rPivots[ S2 ] = m_rPivots[ PV ] - dif;
  m_rPivots[ S3 ] = m_rPivots[ S1 ] - dif;
  CalcHalfPivots();
}

void CPivotSet::CalcHalfPivots() {
  m_rPivots[ R23  ] = ( m_rPivots[ R3 ] + m_rPivots[ R2 ] ) / 2;
  m_rPivots[ R12  ] = ( m_rPivots[ R1 ] + m_rPivots[ R2 ] ) / 2;
  m_rPivots[ PVR1 ] = ( m_rPivots[ PV ] + m_rPivots[ R1 ] ) / 2;
  m_rPivots[ PVS1 ] = ( m_rPivots[ PV ] + m_rPivots[ S1 ] ) / 2;
  m_rPivots[ S12  ] = ( m_rPivots[ S1 ] + m_rPivots[ S2 ] ) / 2;
  m_rPivots[ S23  ] = ( m_rPivots[ S2 ] + m_rPivots[ S3 ] ) / 2;
}

/*
http://www.earnforex.com/pivot_points_calculator.php
The floor pivot points, presented in the first column of the calculation results table, 
are the most basic and popular type of pivots used in Forex trading technical analysis. 
The pivot point is interpreted as the primary support/resistance level - the point at 
which the main trend will be born. First-third level resistance and support points 
serve as additional indicators of possible trend reversal or continuation. The rules 
to calculate floor pivot points are quite simple: 

Pivot (P) = (H + L + C) / 3 

Resistance (R1) = (2 X P) - L 

R2 = P + H - L 

R3 = H + 2 X (P - L) 

Support (S1) = (2 X P) - H 

S2 = P - H + L 

S3 = L - 2 X (H - P) 

Other popular method of calculating a simple TA indicator which helps trader to forecast 
future trend is Tom DeMark's pivot points. Which are not pivot points exactly, but predicted 
low and high of the period. To calculate DeMark's pivot points follow these rules: 

If Close < Opencurrent Then X = H + 2 X L + C; 

If Close > Opencurrent Then X = 2 X H + L + C; 

If Close = Opencurrent Then X = H + L + 2 X C; 

New High = X / 2 - L; New Low = X / 2 - H 

Woodie's pivot points are similar to floor pivot points, but are calculated in a 
somewhat different way, giving more weight to the Close price of the 
previous period. Use the following rules to calculate Woodie's pivot points: 

Pivot (P) = (H + L + 2 X C) / 4 

Resistance (R1) = (2 X P) - L 

R2 = P + H - L 

Support (S1) = (2 X P) - H 

S2 = P - H + L 

Camarilla pivot points is a set of eight very probable levels 
which resemble support and resistance values for a current trend. 
The origin and the precise way to calculate these pivot points are unclear. 
The most important is that these pivot points work for all traders and help in 
setting the right stop-loss and take-profit orders. We use the following 
rules to calculate Camarilla pivot points: 

R4 = (H - L) X 1.1 / 2 + C 

R3 = (H - L) X 1.1 / 4 + C 

R2 = (H - L) X 1.1 / 6 + C 

R1 = (H - L) X 1.1 / 12 + C 

S1 = C - (H - L) X 1.1 / 12 

S2 = C - (H - L) X 1.1 / 6 

S3 = C - (H - L) X 1.1 / 4 

S4 = C - (H - L) X 1.1 / 2 
*/