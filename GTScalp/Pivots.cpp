#include "StdAfx.h"
#include "Pivots.h"

CPivotSet::CPivotSet(void) {
  m_R3 = m_R2 = m_R1 = m_PV = m_S1 = m_S2 = m_S3 = 0;
  m_R23 = m_R12 = m_PVR1 = m_PVS1 = m_S12 = m_S23 = 0;
}

CPivotSet::CPivotSet( const string &sName, double S3, double S2, double S1, double PV, double R1, double R2, double R3 ) {
  m_sName = sName;
  m_R3 = R3;
  m_R2 = R2;
  m_R1 = R1;
  m_PV = PV;
  m_S1 = S1;
  m_S2 = S2;
  m_S3 = S3;
  CalcHalfPivots();
}

CPivotSet::CPivotSet( const string &sName, CBars *bars ) {
  double hi = 0;
  double lo = 0;
  double cl = 0;
  size_t cnt = bars->Count();
  CBar *bar;
  if ( cnt > 0 ) {
    bar = bars->At( 0 );
    hi = bar->m_dblHigh;
    lo = bar->m_dblLow;
    cl = bar->m_dblClose;
    for ( unsigned int i = 1; i < cnt; i++ ) {
      bar = bars->At( i );
      hi = max( hi, bar->m_dblHigh );
      lo = min( lo, bar->m_dblLow );
      cl = bar->m_dblClose;
    }
  }
  CalcPivots( sName, hi, lo, cl );
}

CPivotSet::CPivotSet( const string &sName, double Hi, double Lo, double Close ) {
  CalcPivots( sName, Hi, Lo, Close );
}

void CPivotSet::CalcPivots( const string &sName, double Hi, double Lo, double Close ) {
  m_sName = sName;
  double dif = Hi - Lo;
  m_PV = ( Hi + Lo + Close ) / 3;
  m_R1 = 2 * m_PV - Lo;
  m_R2 = m_PV + dif;
  m_R3 = m_R1 + dif;
  m_S1 = 2 * m_PV - Hi;
  m_S2 = m_PV - dif;
  m_S3 = m_S1 - dif;
  CalcHalfPivots();
}

void CPivotSet::CalcHalfPivots() {
  m_R23 = ( m_R3 + m_R2 ) / 2;
  m_R12 = ( m_R1 + m_R2 ) / 2;
  m_PVR1 = ( m_PV + m_R1 ) / 2;
  m_PVS1 = ( m_PV + m_S1 ) / 2;
  m_S12 = ( m_S1 + m_S2 ) / 2;
  m_S23 = ( m_S2 + m_S3 ) / 2;
}

CPivotSet::~CPivotSet() {
}

/*
http://www.earnforex.com/pivot_points_calculator.php
The floor pivot points, presented in the first column of the calculation results table, are the most basic and popular type of pivots used in Forex trading technical analysis. The pivot point is interpreted as the primary support/resistance level - the point at which the main trend will be born. First-third level resistance and support points serve as additional indicators of possible trend reversal or continuation. The rules to calculate floor pivot points are quite simple: 

Pivot (P) = (H + L + C) / 3 

Resistance (R1) = (2 X P) - L 

R2 = P + H - L 

R3 = H + 2 X (P - L) 

Support (S1) = (2 X P) - H 

S2 = P - H + L 

S3 = L - 2 X (H - P) 

Other popular method of calculating a simple TA indicator which helps trader to forecast future trend is Tom DeMark's pivot points. Which are not pivot points exactly, but predicted low and high of the period. To calculate DeMark's pivot points follow these rules: 

If Close < Opencurrent Then X = H + 2 X L + C; 

If Close > Opencurrent Then X = 2 X H + L + C; 

If Close = Opencurrent Then X = H + L + 2 X C; 

New High = X / 2 - L; New Low = X / 2 - H 

Woodie's pivot points are similar to floor pivot points, but are calculated in a somewhat different way, giving more weight to the Close price of the previous period. Use the following rules to calculate Woodie's pivot points: 

Pivot (P) = (H + L + 2 X C) / 4 

Resistance (R1) = (2 X P) - L 

R2 = P + H - L 

Support (S1) = (2 X P) - H 

S2 = P - H + L 

Camarilla pivot points is a set of eight very probable levels which resemble support and resistance values for a current trend. The origin and the precise way to calculate these pivot points are unclear. The most important is that these pivot points work for all traders and help in setting the right stop-loss and take-profit orders. We use the following rules to calculate Camarilla pivot points: 

R4 = (H - L) X 1.1 / 2 + C 

R3 = (H - L) X 1.1 / 4 + C 

R2 = (H - L) X 1.1 / 6 + C 

R1 = (H - L) X 1.1 / 12 + C 

S1 = C - (H - L) X 1.1 / 12 

S2 = C - (H - L) X 1.1 / 6 

S3 = C - (H - L) X 1.1 / 4 

S4 = C - (H - L) X 1.1 / 2 
*/