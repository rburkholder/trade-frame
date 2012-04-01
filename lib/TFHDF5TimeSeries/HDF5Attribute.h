/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>

#include "HDF5DataManager.h"
#include <TFTrading/TradingEnumerations.h>
#include <TFTrading/KeyTypes.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class CHDF5Attributes {
public:

  struct structOption {
    //std::string sUnderlying;
    double dblStrike;
    unsigned short nYear;
    unsigned short nMonth;
    unsigned short nDay;
    OptionSide::enumOptionSide eSide;
    structOption( void ) : dblStrike( 0 ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), eSide( OptionSide::Unknown ) {};
    structOption( double dblStrike_, unsigned short nYear_, unsigned short nMonth_, unsigned short nDay_, OptionSide::enumOptionSide eSide_ )
      : dblStrike( dblStrike_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_), eSide( eSide_ ) {};
  };

  struct structFuture {
    unsigned short nYear;
    unsigned short nMonth;
    unsigned short nDay;
    structFuture( void ) :nYear( 0 ), nMonth( 0 ), nDay( 0 ) {};
    structFuture( unsigned short nYear_, unsigned short nMonth_, unsigned short nDay_ )
      : nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_) {};
  };

  CHDF5Attributes(void);
  CHDF5Attributes( const std::string& sPath );
  CHDF5Attributes( const std::string& sPath, InstrumentType::enumInstrumentTypes );
  CHDF5Attributes( const std::string& sPath, const structOption& );
  CHDF5Attributes( const std::string& sPath, const structFuture& );
  ~CHDF5Attributes(void);

  void OpenDataSet( const std::string& sPath );
  void CloseDataSet( void );

  void SetInstrumentType( InstrumentType::enumInstrumentTypes );
  InstrumentType::enumInstrumentTypes GetInstrumentType( void );

  void SetProviderType( keytypes::eidProvider_t );
  keytypes::eidProvider_t GetProviderType( void );

  void SetOptionAttributes( const structOption& );
  void GetOptionAttributes( structOption* );

  void SetFutureAttributes( const structFuture& );
  void GetFutureAttributes( structFuture* );

  void SetSignificantDigits( unsigned char );
  unsigned char GetSignificantDigits( void );

  void SetMultiplier( unsigned short );
  unsigned short GetMultiplier( void );

protected:
private:
  CHDF5DataManager dm;
  H5::DataSet *m_pDataSet;
};

} // namespace tf
} // namespace ou
