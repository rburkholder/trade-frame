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

#include "ModelExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ModelExecution::ModelExecution(void): ModelBase<ModelExecution>() {
  m_vColumnNames += "Quan", "Price", "Side";
}

ModelExecution::~ModelExecution(void) {
}

} // namespace tf
} // namespace ou
