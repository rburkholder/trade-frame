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

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "ModelOrder.h"

ModelOrder::ModelOrder(void) {
  m_vColumnNames += "Status", "Type", "Side", "Price1", "Price2", 
    "Ordered", "Remaining", "Filled", "Fill Price", "Comm";
}


ModelOrder::~ModelOrder(void) {
}
