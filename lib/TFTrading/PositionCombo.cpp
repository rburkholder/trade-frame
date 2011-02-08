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

#include "StdAfx.h"

#include "PositionCombo.h"

CPositionCombo::CPositionCombo(void) {
}

CPositionCombo::~CPositionCombo(void) {
}

void CPositionCombo::Add( pPosition_t pPosition ) {
  m_vPositions.push_back( pPosition );
}