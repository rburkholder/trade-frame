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

#include "StdAfx.h"

#include "CurlGetMktSymbols.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

CurlGetMktSymbols::CurlGetMktSymbols(void)
  : m_hCurl( 0 )
{
  curl_global_init(CURL_GLOBAL_ALL);

  m_hCurl = curl_easy_init();
  curl_easy_setopt(m_hCurl, CURLOPT_URL, "http://www.dtniq.com/product/mktsymbols_v2.zip");
  curl_easy_setopt(m_hCurl, CURLOPT_WRITEFUNCTION, &CurlGetMktSymbols::WriteMemoryCallback);
  curl_easy_setopt(m_hCurl, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(m_hCurl, CURLOPT_HEADERFUNCTION, &CurlGetMktSymbols::HeaderFunctionCallback);
  curl_easy_setopt(m_hCurl, CURLOPT_HEADERDATA, this);
  curl_easy_setopt(m_hCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  CURLcode code;
  code = curl_easy_perform(m_hCurl);
  curl_easy_cleanup(m_hCurl);
}

CurlGetMktSymbols::~CurlGetMktSymbols(void) {
  curl_global_cleanup();
}

size_t CurlGetMktSymbols::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void* self) {
  CurlGetMktSymbols* p = reinterpret_cast<CurlGetMktSymbols*>( self );
  return size * nmemb;
}

size_t CurlGetMktSymbols::HeaderFunctionCallback(void* ptr, size_t size, size_t nmemb, void* self) {
  CurlGetMktSymbols* p = reinterpret_cast<CurlGetMktSymbols*>( self );
  return size * nmemb;
}

} // namespace tf
} // namespace ou