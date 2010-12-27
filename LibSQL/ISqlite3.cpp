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

#include "ISqlite3.h"


namespace ou {
namespace db {

ISqlite3::ISqlite3(void) {
}

ISqlite3::~ISqlite3(void) {
}

void ISqlite3::Open( const std::string& sDbFileName, enumOpenFlags flags ) {
}

void ISqlite3::Close( void ) {
}


} // db
} // ou
