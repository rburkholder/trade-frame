#pragma once

#include "CommonDatabaseFunctions.h"

#include <string>

#include <assert.h>

class CAlternateInstrumentNames : public CCommonDatabaseFunctions<CAlternateInstrumentNames>{
public:
  CAlternateInstrumentNames(void);
  virtual ~CAlternateInstrumentNames(void);
  static const char nMaxSymbolNameSize = 20;
  void Save( const std::string &Key, unsigned short id, const std::string &Alternate );
  void Get( const std::string &Key, unsigned short id, std::string *pAlternate );
protected:
  struct structKey {
    unsigned short nId;
    char nNameLength;
    char Name[ nMaxSymbolNameSize ];
    structKey( void ) : nId( 0 ), nNameLength( 0 ) {};
    structKey( unsigned short id, const std::string &sName ) : nId( id ), nNameLength( 0 ) {
      assert( sName.size() <= nMaxSymbolNameSize );
      nNameLength = (char) sName.size();
      strncpy( Name, sName.c_str(), nNameLength );
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
