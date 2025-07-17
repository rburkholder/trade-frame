/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

/*
 * File:    DbRecordTag.cpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 26, 2025 10:38:26
 */

 #include "TableTag.hpp"

 namespace db {
 namespace table {

  const std::string Tag::c_TableName( "tag" );
/*
  create table tag (
    tag_name text not null,
    symbol_name text not null,
    constraint PK_tag primary key( tag_name, symbol_name )
    );
*/

 } // namespace table
 } // namespace db
