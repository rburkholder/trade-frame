#pragma once

#include <map>
using namespace std;

class CRMMPointStat {
public:
  CRMMPointStat(void) { m_cntObjectsAtPrice = 1; };
  virtual ~CRMMPointStat(void) {};

  unsigned int m_cntObjectsAtPrice;
};

class CRunningMinMax {
public:
  CRunningMinMax(void);
  virtual ~CRunningMinMax(void);

  virtual void Add( double );
  virtual void Remove( double );

  double m_dblMax;
  double m_dblMin;

protected:
  map<double, CRMMPointStat*> m_mapPointStats;
  typedef pair <double, CRMMPointStat*> m_mapPointStats_pair;
private:
};
