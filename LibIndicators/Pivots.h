#pragma once
#include "TimeSeries.h"

#include <string>
using namespace std;

class CPivotSet {
public:
  CPivotSet(void);
  CPivotSet( const string &sName, double S3, double S2, double S1, double PV, double R1, double R2, double R3 );
  CPivotSet( const string &sName, double Hi, double Lo, double Close );
  CPivotSet( const string &sName, CBars *bars );

  virtual ~CPivotSet(void);

  const string &Name( void ) { return m_sName; };
  double R3( void ) { return m_R3; };
  double R23( void ) { return m_R23; };
  double R2( void ) { return m_R2; };
  double R12( void ) { return m_R12; };
  double R1( void ) { return m_R1; };
  double PVR1( void ) { return m_PVR1; };
  double PV( void ) { return m_PV; };
  double PVS1( void ) { return m_PVS1; };
  double S1( void ) { return m_S1; };
  double S12( void ) { return m_S12; };
  double S2( void ) { return m_S2; };
  double S23( void ) { return m_S23; };
  double S3( void ) { return m_S3; };

protected:
  
  string m_sName;
  double m_R3;
  double m_R23;
  double m_R2;
  double m_R12;
  double m_R1;
  double m_PVR1;
  double m_PV;
  double m_PVS1;
  double m_S1;
  double m_S12;
  double m_S2;
  double m_S23;
  double m_S3;

  void CalcHalfPivots();
  void CalcPivots( const string &sName, double Hi, double Lo, double Close );
private:
};
