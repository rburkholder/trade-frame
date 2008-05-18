#pragma once

#include "Pivots.h"

#include <map>

class CPivotGroup {
public:
  CPivotGroup(void);
  CPivotGroup( CBars *pBars );
  virtual ~CPivotGroup(void);

  void CalculatePivotSets( CBars *pBars );
  //CPivotSet *operator[]( unsigned short ix ) { return m_vPivotSetGroup[ ix ]; }; 
  void Clear( void ) { m_mmPivots.clear(); };

  typedef std::multimap<double, string>::const_iterator const_iterator;
  const_iterator begin() { return m_mmPivots.begin(); };
  const_iterator end() { return m_mmPivots.end(); };

protected:

  std::multimap<double, string> m_mmPivots; // sorted list of all pivots, along with name assignments

  void AddToMap( CPivotSet &set );


private:
};
