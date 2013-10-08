/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

namespace ou { // One Unified
namespace tf { // TradeFrame

class PivotGroup {
public:
  PivotGroup(void);
  PivotGroup( Bars* pBars );
  virtual ~PivotGroup(void);

  void CalculatePivotSets( Bars* pBars );
  //PivotSet *operator[]( unsigned short ix ) { return m_vPivotSetGroup[ ix ]; }; 
  void Clear( void ) { m_mmPivots.clear(); };

  typedef std::multimap<double, PivotSet::structPivotInfo>::const_iterator const_iterator;
  const_iterator begin() { return m_mmPivots.begin(); };
  const_iterator end() { return m_mmPivots.end(); };

protected:
  std::multimap<double, PivotSet::structPivotInfo> m_mmPivots; // sorted list of all pivots, along with name assignments
  void AddToMap( PivotSet &set );
private:
};

} // namespace tf
} // namespace ou
