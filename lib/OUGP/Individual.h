/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "RootNode.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class Individual {
public:

  unsigned int m_id;

  struct Signals_t {
    RootNode rnLongEnter;
    RootNode rnLongExit;
    RootNode rnShortEnter;
    RootNode rnShortExit;
  } m_Signals;

  double m_dblRawFitness; // absolute dollars value
  double m_dblRelativeFitness;  // maxrawfitness - rawfitness, larger numbers therefore worse (aka standardized fitness)
  double m_dblAdjustedFitness;  // 1 / ( 1 + rf ), range 0 to 1, with 1 being best
  double m_dblNormalizedFitness;  // af / sum(af), range 0 to 1, with 1 being best, sum is 1

  Individual(void);
  ~Individual(void);

  void TreeToString( std::stringstream& ss ) const;

  bool operator>( const Individual& rhs ) const { return m_dblNormalizedFitness > rhs.m_dblNormalizedFitness; };
  bool operator<( const Individual& rhs ) const { return m_dblNormalizedFitness < rhs.m_dblNormalizedFitness; };

protected:
private:
  bool m_bComputed;
  unsigned int m_nCount;
};

} // namespace gp
} // namespace ou
