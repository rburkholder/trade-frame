/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif

#include <curl/curl.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class CurlGetMktSymbols {
public:
  CurlGetMktSymbols(void);
  ~CurlGetMktSymbols(void);
  char* Buffer( void ) const { return m_buf; };
  size_t Size( void ) const { return m_size; };
protected:
private:
  bool m_bSizeFound;
  CURL* m_hCurl;
  char* m_buf;
  char* m_offset;  // offset into buf for writing more stuff
  size_t m_size;
  static size_t WriteMemoryCallback( void* contents, size_t size, size_t nmemb, void* self );
  static size_t HeaderFunctionCallback( void* ptr, size_t size, size_t nmemb, void* self );
  static int ProgressFunctionCallback( void *self, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow );
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
