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

protected:
  void CalcHalfPivots();
  void CalcPivots( const string &sName, double Hi, double Lo, double Close );
private:
};
