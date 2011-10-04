/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "ModelPosition.h"

ModelPosition::ModelPosition(void) {
  m_vColumnNames += "Name", "Instrument", "Algorithm",
    "Side Pend", "Quan Pend", "Side Active", "Quan Active", 
    "Constructed Value", "Mkt Value", "UnRlzd PL", "Rlzd PL", "Comm.", "Net";
}

ModelPosition::~ModelPosition(void) {
}
