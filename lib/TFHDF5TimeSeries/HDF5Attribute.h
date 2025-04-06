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

#include <boost/cstdint.hpp>

#include <TFTrading/TradingEnumerations.h>
#include <TFTrading/KeyTypes.h>

#include "HDF5DataManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class HDF5Attributes{
public:

  struct structOption {
    //std::string sUnderlying;
    double dblStrike;
    unsigned short nYear;
    unsigned short nMonth;
    unsigned short nDay;
    OptionSide::EOptionSide eSide;
    structOption() : dblStrike( 0 ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), eSide( OptionSide::Unknown ) {};
    structOption( double dblStrike_, unsigned short nYear_, unsigned short nMonth_, unsigned short nDay_, OptionSide::EOptionSide eSide_ )
    : dblStrike( dblStrike_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_), eSide( eSide_ ) {};
  };

  struct structFuture {
    unsigned short nYear;
    unsigned short nMonth;
    unsigned short nDay;
    structFuture() :nYear( 0 ), nMonth( 0 ), nDay( 0 ) {};
    structFuture( unsigned short nYear_, unsigned short nMonth_, unsigned short nDay_ )
    : nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_) {};
  };

  HDF5Attributes( HDF5DataManager& dm );
  HDF5Attributes( HDF5DataManager& dm, const std::string& sPath );
  HDF5Attributes( HDF5DataManager& dm, const std::string& sPath, InstrumentType::EInstrumentType );
  HDF5Attributes( HDF5DataManager& dm, const std::string& sPath, const structOption& );
  HDF5Attributes( HDF5DataManager& dm, const std::string& sPath, const structFuture& );
  ~HDF5Attributes();

  void SetSignature( boost::uint64_t ); // left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64
  boost::uint64_t GetSignature();

  void SetInstrumentType( InstrumentType::EInstrumentType );
  InstrumentType::EInstrumentType GetInstrumentType();

  void SetProviderType( keytypes::eidProvider_t );
  keytypes::eidProvider_t GetProviderType();

  void SetOptionAttributes( const structOption& );
  void GetOptionAttributes( structOption* );

  void SetFutureAttributes( const structFuture& );
  void GetFutureAttributes( structFuture* );

  void SetSignificantDigits( unsigned char );
  unsigned char GetSignificantDigits();

  void SetMultiplier( unsigned short );
  unsigned short GetMultiplier();

protected:
  void OpenDataSet( const std::string& sPath );
  void CloseDataSet();
private:
  HDF5DataManager& m_dm;
  H5::DataSet *m_pDataSet;
};

} // namespace tf
} // namespace ou
