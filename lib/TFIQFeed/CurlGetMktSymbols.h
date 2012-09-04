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

class CurlGetMktSymbols {
public:
  CurlGetMktSymbols(void);
  ~CurlGetMktSymbols(void);
protected:
private:
  CURL* m_hCurl;
  static size_t WriteMemoryCallback( void* contents, size_t size, size_t nmemb, void* self );
  static size_t HeaderFunctionCallback( void* ptr, size_t size, size_t nmemb, void* self );
};

} // namespace tf
} // namespace ou