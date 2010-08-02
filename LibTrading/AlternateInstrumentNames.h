/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>
#include <cassert>

#include "LibBerkeleyDb\CommonDatabaseFunctions.h"

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning( disable : 4996 )

class CAlternateInstrumentNames : public CCommonDatabaseFunctions<CAlternateInstrumentNames>{
public:
  CAlternateInstrumentNames(void);
  ~CAlternateInstrumentNames(void);
  void Save( const std::string &ProviderName, const std::string &InstrumentName, const std::string &AlternateInstrumentName );
  void Get( const std::string &ProviderName, const std::string &InstrumentName, std::string *pAlternate );
protected:
  static const char nMaxKeySize = 30;
  static const char nMaxSymbolNameSize = 15;
  struct structKey {
    char nKeyLength;  // key is provider name + instrument name
    char Key[ nMaxKeySize ];
    structKey( void ) : nKeyLength( 0 ) {};
    structKey( const std::string &sProviderName, const std::string &sInstrumentName ) : nKeyLength( 0 ) {
      assert( ( sProviderName.size() + sInstrumentName.size() ) <= nMaxKeySize );
      nKeyLength = (char) sProviderName.size() + (char) sInstrumentName.size();
      std::string t = sProviderName + sInstrumentName;
      strncpy( Key, t.c_str(), nKeyLength );
    };
  };
  struct structValue {
    char nValueLength;
    char Value[ nMaxSymbolNameSize ];
    structValue( void ) : nValueLength( 0 ) {};
    structValue( const std::string &sValue ) {
      assert( sValue.size() <= nMaxSymbolNameSize );
      nValueLength = (char) sValue.size();
      strncpy( Value, sValue.c_str(), nValueLength );
    };
  };
private:
};

// sample code which was in ProviderInterface.h:

//  std::map<std::string, std::string> m_mapAlternateNames;  // caching map to save database lookups
//  CAlternateInstrumentNames m_lutAlternateInstrumentNames;

  // need to redo this and place into CInstrument? => CInstrument has basic AlternateName stuff
//  void SetAlternateInstrumentName( const std::string& OriginalInstrumentName, const std::string& AlternateIntrumentName );
//  void GetAlternateInstrumentName( const std::string& OriginalInstrumentName, std::string* pAlternateInstrumentName );

/*
template <typename P, typename S>
void CProviderInterface<P,S>::SetAlternateInstrumentName(const std::string &OriginalInstrumentName, const std::string &AlternateIntrumentName) {
  m_lutAlternateInstrumentNames.Save( m_sName, OriginalInstrumentName, AlternateIntrumentName );
  std::map<std::string, std::string>::iterator iter 
    = m_mapAlternateNames.find( OriginalInstrumentName );
  if ( m_mapAlternateNames.end() == iter ) {
    m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, AlternateIntrumentName ) );
  }
  else m_mapAlternateNames[ OriginalInstrumentName ] = AlternateIntrumentName;
}

template <typename P, typename S>
void CProviderInterface<P,S>::GetAlternateInstrumentName(const std::string &OriginalInstrumentName, std::string *pAlternateInstrumentName) {
  std::map<std::string, std::string>::iterator iter 
    = m_mapAlternateNames.find( OriginalInstrumentName );
  if ( m_mapAlternateNames.end() != iter ) {
    pAlternateInstrumentName->assign( iter->second );
  }
  else {
    try {
      m_lutAlternateInstrumentNames.Get( m_sName, OriginalInstrumentName, pAlternateInstrumentName );
      m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, *pAlternateInstrumentName ) );
    }
    catch ( std::out_of_range e ) {
      m_mapAlternateNames.insert( std::pair<std::string, std::string>( OriginalInstrumentName, OriginalInstrumentName ) );
      pAlternateInstrumentName->assign( OriginalInstrumentName );
    }
    catch ( std::exception e ) {
      std::cout << "CProviderInterface::GetAlternateInstrumentName has error: " << e.what() << std::endl;
    }
  }
}
*/

