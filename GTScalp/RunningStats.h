#pragma once

class CRunningStats {
public:
  CRunningStats(void);
  virtual ~CRunningStats(void);

  double b2; // acceleration
  double b1; // slope
  double b0; // offset

  double meanY;

  double RR;
  double R;

  double SD;

  double BBUpper, BBLower;
  double BBMultiplier;

  void Add( double, double );
  void Remove( double, double );
  virtual void CalcStats( void );

protected:
  unsigned int nX;

  double SumXX, SumX, SumXY, SumY, SumYY;
  double Sxx, Sxy, Syy;
  double SST, SSR, SSE;

  bool CanCalcSlope;
private:
};
