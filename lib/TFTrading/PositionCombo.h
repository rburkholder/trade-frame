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

#include <vector>

#include "Position.h"

// used for tracking profit/loss across a combo of positions

class CPositionCombo {
public:

  typedef CPosition::pPosition_t pPosition_t;

  CPositionCombo(void);
  ~CPositionCombo(void);

  void Add( pPosition_t );

protected:

private:

  std::vector<pPosition_t> m_vPositions;

};

