#pragma once
#include "TimeSeries.h"

#include <string>
#include <utility>

class CPivotSet {
public:
  CPivotSet(void);
  CPivotSet( const string &sName, double S3, double S2, double S1, double PV, double R1, double R2, double R3 );
  CPivotSet( const string &sName, double Hi, double Lo, double Close );
  CPivotSet( const string &sName, CBars *bars );

  virtual ~CPivotSet(void);

  enum enumPivots { R3, R23, R2, R12, R1, PVR1, PV, PVS1, S1, S12, S2, S23, S3, PivotCount };

  const std::string &Name( void ) { return m_sName; };
  typedef pair<double, string> pivot_t;
  pivot_t operator[]( unsigned short ix ) { 
    assert( ix < PivotCount ); 
    return pivot_t( m_rPivots[ ix ], m_sName + m_sPivotNames[ ix ] ); 
  }

protected:
  
  std::string m_sName;

  double m_rPivots[ PivotCount ];
  static std::string m_sPivotNames[ PivotCount ];

  void CalcHalfPivots();
  void CalcPivots( const std::string &sName, double Hi, double Lo, double Close );
private:
};
