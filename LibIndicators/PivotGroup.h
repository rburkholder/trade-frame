/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <map>

#include "Pivots.h"

class CPivotGroup {
public:
  CPivotGroup(void);
  CPivotGroup( CBars *pBars );
  virtual ~CPivotGroup(void);

  void CalculatePivotSets( CBars *pBars );
  //CPivotSet *operator[]( unsigned short ix ) { return m_vPivotSetGroup[ ix ]; }; 
  void Clear( void ) { m_mmPivots.clear(); };

  typedef std::multimap<double, CPivotSet::structPivotInfo>::const_iterator const_iterator;
  const_iterator begin() { return m_mmPivots.begin(); };
  const_iterator end() { return m_mmPivots.end(); };

protected:
  std::multimap<double, CPivotSet::structPivotInfo> m_mmPivots; // sorted list of all pivots, along with name assignments
  void AddToMap( CPivotSet &set );
private:
};
