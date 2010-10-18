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
#include "CountryCode.h"

const std::string CCountryCode::m_sSqlCreate( 
  "create table countrycodes ( \
   alpha2 TEXT CONSTRAINT pk_countrycodes PRIMARY KEY, \
   name TEXT NOT NULL \
   );" );
const std::string CCountryCode::m_sSqlSelect( "SELECT name from countrycodes where alpha2 = :id;" );
const std::string CCountryCode::m_sSqlInsert( "INSERT INTO countrycodes (alpha2, name) VALUES ( :id, :name );" );
const std::string CCountryCode::m_sSqlUpdate( "UPDATE countrycodes SET name = :name WHERE alpha2 = :id;" );
const std::string CCountryCode::m_sSqlDelete( "DELETE FROM countrycodes WHERE alpha2 = :id;" );

CCountryCode::CCountryCode( const std::string& sAlpha2, const std::string& sName )
: m_sAlpha2( sAlpha2 ), m_sName( sName )
{
}

CCountryCode::CCountryCode( const std::string& sAlpha2, sqlite3_stmt* pStmt )
: m_sAlpha2( sAlpha2 ), m_sName( reinterpret_cast<const char*>( sqlite3_column_text( pStmt, 0 ) ) )
{
}

CCountryCode::~CCountryCode( void ) {
}

int CCountryCode::BindDbKey( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn = sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":id" ), m_sAlpha2.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;
}

int CCountryCode::BindDbVariables( sqlite3_stmt* pStmt ) {
  int rtn( 0 );
  rtn += sqlite3_bind_text( 
    pStmt, sqlite3_bind_parameter_index( pStmt, ":name" ), m_sName.c_str(), -1, SQLITE_TRANSIENT );
  return rtn;  // should be 0 if all goes well
}

void CCountryCode::CreateDbTable( sqlite3* pDb ) {

  char* pMsg;
  int rtn;

  rtn = sqlite3_exec( pDb, m_sSqlCreate.c_str(), 0, 0, &pMsg );

  if ( SQLITE_OK != rtn ) {
    std::string sErr( "Error creating table countrycodes: " );
    sErr += pMsg;
    sqlite3_free( pMsg );
    throw std::runtime_error( sErr );
  }

  // http://www.iso.org/iso/country_codes/iso_3166_code_lists.htm
  // http://en.wikipedia.org/wiki/ISO_3166-1

  // built as of 2010/10/17

  rtn = 0;
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('AFGHANISTAN', 'AF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ÅLAND ISLANDS', 'AX');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ALBANIA', 'AL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ALGERIA', 'DZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('AMERICAN SAMOA', 'AS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ANDORRA', 'AD');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ANGOLA', 'AO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ANGUILLA', 'AI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ANTARCTICA', 'AQ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ANTIGUA AND BARBUDA', 'AG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ARGENTINA', 'AR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ARMENIA', 'AM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ARUBA', 'AW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('AUSTRALIA', 'AU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('AUSTRIA', 'AT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('AZERBAIJAN', 'AZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BAHAMAS', 'BS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BAHRAIN', 'BH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BANGLADESH', 'BD');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BARBADOS', 'BB');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BELARUS', 'BY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BELGIUM', 'BE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BELIZE', 'BZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BENIN', 'BJ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BERMUDA', 'BM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BHUTAN', 'BT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BOLIVIA, PLURINATIONAL STATE OF', 'BO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BOSNIA AND HERZEGOVINA', 'BA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BOTSWANA', 'BW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BOUVET ISLAND', 'BV');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BRAZIL', 'BR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BRITISH INDIAN OCEAN TERRITORY', 'IO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BRUNEI DARUSSALAM', 'BN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BULGARIA', 'BG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BURKINA FASO', 'BF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('BURUNDI', 'BI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CAMBODIA', 'KH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CAMEROON', 'CM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CANADA', 'CA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CAPE VERDE', 'CV');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CAYMAN ISLANDS', 'KY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CENTRAL AFRICAN REPUBLIC', 'CF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CHAD', 'TD');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CHILE', 'CL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CHINA', 'CN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CHRISTMAS ISLAND', 'CX');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('COCOS (KEELING) ISLANDS', 'CC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('COLOMBIA', 'CO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('COMOROS', 'KM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CONGO', 'CG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CONGO, THE DEMOCRATIC REPUBLIC OF THE', 'CD');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('COOK ISLANDS', 'CK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('COSTA RICA', 'CR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CÔTE D'IVOIRE', 'CI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CROATIA', 'HR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CUBA', 'CU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CYPRUS', 'CY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('CZECH REPUBLIC', 'CZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('DENMARK', 'DK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('DJIBOUTI', 'DJ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('DOMINICA', 'DM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('DOMINICAN REPUBLIC', 'DO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ECUADOR', 'EC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('EGYPT', 'EG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('EL SALVADOR', 'SV');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('EQUATORIAL GUINEA', 'GQ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ERITREA', 'ER');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ESTONIA', 'EE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ETHIOPIA', 'ET');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FALKLAND ISLANDS (MALVINAS)', 'FK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FAROE ISLANDS', 'FO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FIJI', 'FJ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FINLAND', 'FI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FRANCE', 'FR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FRENCH GUIANA', 'GF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FRENCH POLYNESIA', 'PF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('FRENCH SOUTHERN TERRITORIES', 'TF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GABON', 'GA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GAMBIA', 'GM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GEORGIA', 'GE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GERMANY', 'DE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GHANA', 'GH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GIBRALTAR', 'GI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GREECE', 'GR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GREENLAND', 'GL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GRENADA', 'GD');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GUADELOUPE', 'GP');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GUAM', 'GU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GUATEMALA', 'GT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GUERNSEY', 'GG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GUINEA', 'GN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GUINEA-BISSAU', 'GW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('GUYANA', 'GY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('HAITI', 'HT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('HEARD ISLAND AND MCDONALD ISLANDS', 'HM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('HOLY SEE (VATICAN CITY STATE)', 'VA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('HONDURAS', 'HN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('HONG KONG', 'HK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('HUNGARY', 'HU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ICELAND', 'IS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('INDIA', 'IN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('INDONESIA', 'ID');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('IRAN, ISLAMIC REPUBLIC OF', 'IR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('IRAQ', 'IQ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('IRELAND', 'IE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ISLE OF MAN', 'IM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ISRAEL', 'IL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ITALY', 'IT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('JAMAICA', 'JM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('JAPAN', 'JP');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('JERSEY', 'JE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('JORDAN', 'JO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('KAZAKHSTAN', 'KZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('KENYA', 'KE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('KIRIBATI', 'KI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('KOREA, DEMOCRATIC PEOPLE'S REPUBLIC OF', 'KP');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('KOREA, REPUBLIC OF', 'KR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('KUWAIT', 'KW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('KYRGYZSTAN', 'KG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LAO PEOPLE'S DEMOCRATIC REPUBLIC', 'LA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LATVIA', 'LV');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LEBANON', 'LB');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LESOTHO', 'LS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LIBERIA', 'LR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LIBYAN ARAB JAMAHIRIYA', 'LY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LIECHTENSTEIN', 'LI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LITHUANIA', 'LT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('LUXEMBOURG', 'LU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MACAO', 'MO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MACEDONIA, THE FORMER YUGOSLAV REPUBLIC OF', 'MK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MADAGASCAR', 'MG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MALAWI', 'MW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MALAYSIA', 'MY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MALDIVES', 'MV');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MALI', 'ML');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MALTA', 'MT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MARSHALL ISLANDS', 'MH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MARTINIQUE', 'MQ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MAURITANIA', 'MR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MAURITIUS', 'MU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MAYOTTE', 'YT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MEXICO', 'MX');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MICRONESIA, FEDERATED STATES OF', 'FM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MOLDOVA, REPUBLIC OF', 'MD');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MONACO', 'MC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MONGOLIA', 'MN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MONTENEGRO', 'ME');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MONTSERRAT', 'MS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MOROCCO', 'MA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MOZAMBIQUE', 'MZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('MYANMAR', 'MM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NAMIBIA', 'NA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NAURU', 'NR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NEPAL', 'NP');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NETHERLANDS', 'NL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NETHERLANDS ANTILLES', 'AN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NEW CALEDONIA', 'NC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NEW ZEALAND', 'NZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NICARAGUA', 'NI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NIGER', 'NE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NIGERIA', 'NG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NIUE', 'NU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NORFOLK ISLAND', 'NF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NORTHERN MARIANA ISLANDS', 'MP');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('NORWAY', 'NO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('OMAN', 'OM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PAKISTAN', 'PK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PALAU', 'PW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PALESTINIAN TERRITORY, OCCUPIED', 'PS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PANAMA', 'PA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PAPUA NEW GUINEA', 'PG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PARAGUAY', 'PY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PERU', 'PE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PHILIPPINES', 'PH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PITCAIRN', 'PN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('POLAND', 'PL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PORTUGAL', 'PT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('PUERTO RICO', 'PR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('QATAR', 'QA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('RÉUNION', 'RE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ROMANIA', 'RO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('RUSSIAN FEDERATION', 'RU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('RWANDA', 'RW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAINT BARTHÉLEMY', 'BL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAINT HELENA, ASCENSION AND TRISTAN DA CUNHA', 'SH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAINT KITTS AND NEVIS', 'KN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAINT LUCIA', 'LC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAINT MARTIN', 'MF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAINT PIERRE AND MIQUELON', 'PM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAINT VINCENT AND THE GRENADINES', 'VC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAMOA', 'WS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAN MARINO', 'SM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAO TOME AND PRINCIPE', 'ST');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SAUDI ARABIA', 'SA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SENEGAL', 'SN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SERBIA', 'RS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SEYCHELLES', 'SC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SIERRA LEONE', 'SL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SINGAPORE', 'SG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SLOVAKIA', 'SK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SLOVENIA', 'SI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SOLOMON ISLANDS', 'SB');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SOMALIA', 'SO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SOUTH AFRICA', 'ZA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SOUTH GEORGIA AND THE SOUTH SANDWICH ISLANDS', 'GS');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SPAIN', 'ES');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SRI LANKA', 'LK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SUDAN', 'SD');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SURINAME', 'SR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SVALBARD AND JAN MAYEN', 'SJ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SWAZILAND', 'SZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SWEDEN', 'SE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SWITZERLAND', 'CH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('SYRIAN ARAB REPUBLIC', 'SY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TAIWAN, PROVINCE OF CHINA', 'TW');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TAJIKISTAN', 'TJ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TANZANIA, UNITED REPUBLIC OF', 'TZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('THAILAND', 'TH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TIMOR-LESTE', 'TL');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TOGO', 'TG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TOKELAU', 'TK');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TONGA', 'TO');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TRINIDAD AND TOBAGO', 'TT');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TUNISIA', 'TN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TURKEY', 'TR');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TURKMENISTAN', 'TM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TURKS AND CAICOS ISLANDS', 'TC');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('TUVALU', 'TV');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('UGANDA', 'UG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('UKRAINE', 'UA');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('UNITED ARAB EMIRATES', 'AE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('UNITED KINGDOM', 'GB');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('UNITED STATES', 'US');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('UNITED STATES MINOR OUTLYING ISLANDS', 'UM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('URUGUAY', 'UY');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('UZBEKISTAN', 'UZ');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('VANUATU', 'VU');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('VENEZUELA, BOLIVARIAN REPUBLIC OF', 'VE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('VIET NAM', 'VN');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('VIRGIN ISLANDS, BRITISH', 'VG');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('VIRGIN ISLANDS, U.S.', 'VI');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('WALLIS AND FUTUNA', 'WF');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('WESTERN SAHARA', 'EH');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('YEMEN', 'YE');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ZAMBIA', 'ZM');", 0, 0, 0 );
  rtn += sqlite3_exec( pDb, "insert into countrycodes (name,alpha2) VALUES ('ZIMBABWE', 'ZW');", 0, 0, 0 );

  if ( SQLITE_OK != rtn ) {
    throw std::runtime_error( "CCountryCode::CreateDbTable: could not insert records" );
  }
}