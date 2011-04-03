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

#include "Session.h"

namespace ou {
namespace db {

CSession::CSession( void ): pImpl( new CSessionImplCustom() ) {
}

CSession::~CSession( void ) {
  delete pImpl;
}

void CSession::Open( const std::string& sDbFileName, enumOpenFlags flags ) {
  pImpl->Open( sDbFileName, flags );
}

void CSession::Close( void ) {
  pImpl->Close();
}

bool CSession::Execute( QueryBase& qb ) {
  return pImpl->Execute( qb );
}

bool CSession::Execute( QueryBase::pQueryBase_t pQuery ) {
  return pImpl->Execute( pQuery );
}

void CSession::Reset( QueryBase::pQueryBase_t pQuery ) {
  pImpl->Reset( pQuery );
}

void CSession::CreateTables( void ) {
  pImpl->CreateTables();
}

//boost::int64_t CSession::GetLastRowId( void ) { // relocated to header
//  pImpl->GetLastRowId();
//}


} // db
} // ou
