/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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
#include "PortfolioRecord.h"

CPortfolioRecord::CPortfolioRecord(void) 
: m_pInstrument( NULL ), m_nPosition( 0 ), m_dblPrice( 0 ), m_dblAverageCost( 0 )
{
}

CPortfolioRecord::~CPortfolioRecord(void) {
}
