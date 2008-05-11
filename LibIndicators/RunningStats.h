#pragma once

class CRunningStats {
public:
  CRunningStats(void);
  CRunningStats(double BBMultiplier);
  virtual ~CRunningStats(void);
  void SetBBMultiplier( double dbl ) { m_BBMultiplier = dbl; };
  double GetBBMultiplier( void ) { return m_BBMultiplier; };

  double b2; // acceleration
  double b1; // slope
  double b0; // offset

  double meanY;

  double RR;
  double R;

  double SD;

  double BBUpper, BBLower;

  void Add( double, double );
  void Remove( double, double );
  virtual void CalcStats( void );

protected:
  unsigned int nX, nY;

  double SumXX, SumX, SumXY, SumY, SumYY;
  double Sxx, Sxy, Syy;
  double SST, SSR, SSE;

  double m_BBMultiplier;
  //bool CanCalcSlope;
private:
};
