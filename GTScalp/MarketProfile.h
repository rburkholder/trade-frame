#pragma once

#include <map>
#include <string>

#include "BarFactory.h"
#include "TimeSeries.h"

class CMarketProfile {
public:
  CMarketProfile(void);
  virtual ~CMarketProfile(void);
  void Add( const Trade &trade );
  void Draw();
protected:
  unsigned int m_nBucketSize;  // how many price points in each bucket
  BarFactory m_bf;
  void OnNewBar( const Bar &bar );
  void OnBarUpdate( const Bar &bar );
  Bars *m_pBars;
  std::map<int, std::string *> m_mapProfile;
  typedef std::pair<int, std::string *> m_mapProfile_Pair;
  int m_nMin, m_nMax;
private:
};
