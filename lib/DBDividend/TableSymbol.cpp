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
 * File:    TableSymbol.cpp
 * Author:  raymond@burkholder.net
 * Project: Dividend
 * Created: April 26, 2025 10:46:03
 */

 #include "TableSymbol.hpp"

 namespace db {
 namespace table {

  const std::string Symbol::c_TableName( "symbol" );

/*
  create table symbol (
    symbol_name text not null primary key,
    symbol_fullname text not null,
    date_created text not null,
    date_updated text not null,
    notes text not null
    );
*/

 } // namespace table
 } // namespace db
