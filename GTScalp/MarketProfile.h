#pragma once
#include "BarFactory.h"
#include "TimeSeries.h"
#include <map>
using namespace std;

class CMarketProfile {
public:
  CMarketProfile(void);
  virtual ~CMarketProfile(void);
  void Add( const CTrade &trade );
  void Draw();
protected:
  unsigned int m_nBucketSize;  // how many price points in each bucket
  CBarFactory m_bf;
  void OnNewBar( const CBar &bar );
  void OnBarUpdate( const CBar &bar );
  CBars *m_pBars;
  map<int, string *> m_mapProfile;
  typedef pair<int, string *> m_mapProfile_Pair;
  int m_nMin, m_nMax;
private:
};
