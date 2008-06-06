#pragma once

#include "CommonDatabaseFunctions.h"

#include <string>
#include <assert.h>

class CAlternateInstrumentNames : public CCommonDatabaseFunctions<CAlternateInstrumentNames>{
public:
  CAlternateInstrumentNames(void);
  virtual ~CAlternateInstrumentNames(void);
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
      string t = sProviderName + sInstrumentName;
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
