/************************************************************************
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    FXMacroDataClient.hpp
 * Author:  Robert Tidball, info@fxmacrodata.com
 * Project: lib/extras
 * Created: July 9, 2026
 */

/*
 * What this is:
 *   A small, header-only helper for the FXMacroData REST API
 *   (https://api.fxmacrodata.com/v1).  FXMacroData publishes macroeconomic
 *   releases (policy rates, CPI, GDP, employment, ...), release calendars,
 *   FX spot rates, COT positioning and commodity prices, taken from central
 *   bank and statistics office sources.  Responses are JSON.
 *
 * What it does:
 *   Builds request URLs for each endpoint, and supplies the X-API-Key header
 *   to send with them.  It does no networking itself, so it can be paired
 *   with whatever HTTP client an application already uses (for example the
 *   boost::beast code in lib/TFAlpaca).
 *
 *   USD announcements, the USD release calendar and the USD data catalogue
 *   can be requested without a key.  Other currencies, forex, COT,
 *   commodities and predictions need a key.  The key goes in the X-API-Key
 *   request header, never in the URL.
 *
 * Example:
 *   ou::tf::extras::FXMacroDataClient client( key );
 *   std::string url = client.Announcements( "usd", "inflation" );
 *   // GET url, adding client.ApiKeyHeaderName() : client.ApiKeyHeaderValue()
 *   //   when client.HasApiKey()
 *
 * More info:
 *   https://fxmacrodata.com/documentation/reference?utm_source=github&utm_medium=referral&utm_campaign=trade-frame&utm_content=header
 *
 * Questions:
 *   info@fxmacrodata.com
 */

#pragma once

#include <map>
#include <sstream>
#include <string>
#include <utility>

namespace ou {
namespace tf {
namespace extras {

class FXMacroDataClient {
public:

  explicit FXMacroDataClient(
    std::string apiKey = {}
  , std::string baseUrl = "https://api.fxmacrodata.com/v1"
  )
  : m_apiKey( std::move( apiKey ) ), m_baseUrl( std::move( baseUrl ) ) {}

  // request header carrying the key; only send it when HasApiKey()
  bool HasApiKey() const { return !m_apiKey.empty(); }
  static const char* ApiKeyHeaderName() { return "X-API-Key"; }
  const std::string& ApiKeyHeaderValue() const { return m_apiKey; }

  // endpoint urls
  std::string DataCatalogue( const std::string& currency ) const { return Url( "/data_catalogue/" + Lower( currency ) ); }
  std::string Announcements( const std::string& currency, const std::string& indicator ) const { return Url( "/announcements/" + Lower( currency ) + "/" + indicator ); }
  std::string Calendar( const std::string& currency ) const { return Url( "/calendar/" + Lower( currency ) ); }
  std::string Predictions( const std::string& currency, const std::string& indicator ) const { return Url( "/predictions/" + Lower( currency ) + "/" + indicator ); }
  std::string Forex( const std::string& base, const std::string& quote ) const { return Url( "/forex/" + Lower( base ) + "/" + Lower( quote ) ); }
  std::string Cot( const std::string& currency ) const { return Url( "/cot/" + Lower( currency ) ); }
  std::string CommoditiesLatest() const { return Url( "/commodities/latest" ); }
  std::string Commodity( const std::string& indicator ) const { return Url( "/commodities/" + indicator ); }
  std::string Curves( const std::string& currency ) const { return Url( "/curves/" + Lower( currency ) ); }
  std::string CurveProxies( const std::string& currency ) const { return Url( "/curve_proxies/" + Lower( currency ) ); }
  std::string ForwardCurves( const std::string& currency ) const { return Url( "/forward_curves/" + Lower( currency ) ); }
  std::string MarketSessions() const { return Url( "/market_sessions" ); }
  std::string RiskSentiment() const { return Url( "/risk_sentiment" ); }
  std::string News( const std::string& currency ) const { return Url( "/news/" + Lower( currency ) ); }
  std::string PressReleases( const std::string& currency ) const { return Url( "/press-releases/" + Lower( currency ) ); }
  std::string CentralBankers( const std::string& currency ) const { return Url( "/central_bankers/" + Lower( currency ) ); }

  // optional query parameters, eg start_date / end_date as YYYY-MM-DD
  using params_t = std::map<std::string, std::string>;
  std::string Announcements( const std::string& currency, const std::string& indicator, const params_t& params ) const {
    return Url( "/announcements/" + Lower( currency ) + "/" + indicator, params );
  }
  std::string Forex( const std::string& base, const std::string& quote, const params_t& params ) const {
    return Url( "/forex/" + Lower( base ) + "/" + Lower( quote ), params );
  }

private:

  std::string m_apiKey;
  std::string m_baseUrl;

  std::string Url( const std::string& path, const params_t& params = {} ) const {
    std::ostringstream out;
    out << m_baseUrl << path;
    char separator = '?';
    for ( const params_t::value_type& param: params ) {
      out << separator << param.first << '=' << param.second;
      separator = '&';
    }
    return out.str();
  }

  static std::string Lower( std::string value ) {
    for ( char& c: value ) if ( ( 'A' <= c ) && ( 'Z' >= c ) ) c = static_cast<char>( c - 'A' + 'a' );
    return value;
  }

};

} // namespace extras
} // namespace tf
} // namespace ou
