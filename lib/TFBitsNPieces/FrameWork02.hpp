/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    FrameWork02.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFBitsNPieces
 * Created: June 20, 2022 18:16
 */

#pragma once

#include <sstream>

#include <boost/filesystem.hpp>

#include <OUCommon/TimeSource.h>

#include <TFTrading/ProviderInterface.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

template<typename CRTP>
class FrameWork02 {
public:

  FrameWork02();
  virtual ~FrameWork02();

protected:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;

  std::string m_sTSDataStreamStarted;
  uint16_t m_nTSDataStreamSequence;

private:

};

template<typename CRTP>
FrameWork02<CRTP>::FrameWork02() :
  m_nTSDataStreamSequence {}
{

  std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

  std::stringstream ss;
  ss.str( "" );
  // http://www.boost.org/doc/libs/1_54_0/doc/html/date_time/date_time_io.html
  boost::posix_time::time_facet* pFacet( new boost::posix_time::time_facet( "%Y-%m-%d %H:%M:%S%F" ) );
  ss.imbue( std::locale( ss.getloc(), pFacet ) );
  ss << ou::TimeSource::GlobalInstance().External();
  m_sTSDataStreamStarted = ss.str();

}

template<typename CRTP>
FrameWork02<CRTP>::~FrameWork02() {
}

} // namespace tf
} // namespace ou

