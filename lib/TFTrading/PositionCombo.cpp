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

// 2015/02/15code is not ready, probably deprecated

#include "stdafx.h"

#include "PositionCombo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PositionCombo::PositionCombo(void) {
}

PositionCombo::~PositionCombo(void) {
}

void PositionCombo::Add( pPosition_t pPosition ) {
  m_vPositions.push_back( pPosition );
}

} // namespace tf
} // namespace ou
