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

#include "stdafx.h"

#include "AccountOwner.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

AccountOwner::AccountOwner( 
    const idAccountOwner_t& idAccountOwner, 
    const idAccountAdvisor_t& idAccountAdvisor,
    const std::string& sFirstName, const std::string& sLastName ) 
: m_row( idAccountOwner, idAccountAdvisor, sFirstName, sLastName )
{
}

AccountOwner::AccountOwner( const TableRowDef& row ) : m_row( row ) {};

AccountOwner::~AccountOwner(void) {
}

} // namespace tf
} // namespace ou
