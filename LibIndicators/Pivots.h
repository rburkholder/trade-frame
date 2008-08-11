#pragma once
#include "TimeSeries.h"

#ifdef RGB
#undef RGB
// windows COLORREF is backwards from what ChartDir is expecting
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#endif
#include "Colour.h"

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
  struct structPivotInfo {
    std::string sName;
    Colour::enumColour colour;
    structPivotInfo( void ) : colour( Colour::Black ) {};
    structPivotInfo( const std::string &sName_, Colour::enumColour colour_ ) 
      : sName( sName_ ), colour( colour_ ) {};
  };

  const std::string &Name( void ) { return m_sName; };

  typedef pair<double, structPivotInfo> pivot_t;
  pivot_t operator[]( unsigned short ix ) { // this probably copies twice or thrice in succession
    assert( ix >= R3 );
    assert( ix <= S3 );
    structPivotInfo pivot( m_sName + m_sPivotNames[ ix ], m_rPivotColours[ ix ] );
    return pivot_t( m_rPivots[ ix ], pivot ); 
  }
  double GetPivotValue( enumPivots ix ) {
    assert( ix >= R3 );
    assert( ix <= S3 );
    return m_rPivots[ ix ];
  };
  std::string GetPivotName( enumPivots ix ) {
    assert( ix >= R3 );
    assert( ix <= S3 );
    return m_sName + m_sPivotNames[ ix ];
  };
  static Colour::enumColour GetPivotColour( enumPivots ix ) {
    assert( ix >= R3 );
    assert( ix <= S3 );
    return m_rPivotColours[ ix ];
  };

protected:
  
  std::string m_sName;

  double m_rPivots[ PivotCount ];
  static std::string m_sPivotNames[ PivotCount ];
  static Colour::enumColour m_rPivotColours[ PivotCount ];

  void CalcHalfPivots();
  void CalcPivots( const std::string &sName, double Hi, double Lo, double Close );
private:
};
