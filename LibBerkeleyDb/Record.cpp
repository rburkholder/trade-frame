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

#include "Record.h"

extern std::string const sDbFileName;

CAutoIncKeys ProcessFieldSk::m_keyAutoInc( sDbFileName );
CKeyValuePairs ProcessFieldSk::m_kvStrings( sDbFileName, "_ProcessFieldSkStrings" );

ProcessFieldSk::ProcessFieldSk( fldStored_t& key )
: ProcessFieldBase<ProcessFieldSk,fldStored_t>( key )
 
{
}

