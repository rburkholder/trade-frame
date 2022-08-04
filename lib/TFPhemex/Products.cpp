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
 * File:    Products.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: July 26, 2022 14:52:52
 */

#include <iostream>

#include <boost/json.hpp>

#include "Products.hpp"

namespace json = boost::json;

namespace ou {
namespace tf {
namespace phemex {
namespace products {

namespace {

template<class T>  // put this in an include for the other decoders
void extract( json::object const& obj, json::string_view key, T& t ) {
  bool bExists( true );
  try {
    if ( obj.at( key ).is_null() ) { bExists = false; } // check for existence, provide for initialization
  }
  catch (...) {
    bExists = false;
  }
  if ( bExists ) {
    t = json::value_to<T>( obj.at( key ) );  // some problems with 0/1 as bool
  }
  else {
    T init {};
    t = init;
  }
}

} // namespace anonymous

// == Currency ==

Currency tag_invoke( json::value_to_tag<Currency>, json::value const& jv ) {
  Currency msg;
  json::object const& obj = jv.as_object();
  extract( obj, "currency", msg.currency );
  extract( obj, "name", msg.name );
  extract( obj, "code", msg.code );
  extract( obj, "valueScale", msg.valueScale );
  extract( obj, "minValueEv", msg.minValueEv );
  extract( obj, "maxValueEv", msg.maxValueEv );
  extract( obj, "needAddrTag", msg.needAddrTag );
  extract( obj, "status", msg.status );
  return msg;
}

void Decode( json::array const& array, vCurrency_t& vCurrency ) {
  try {
    for ( const auto& item: array ) {
      vCurrency.push_back( json::value_to<Currency>( item ) );
    }
  }
  catch (...) {
    std::cerr << "phemex::currency::decode error" << std::endl;
  }
}

// == Product ==

Product tag_invoke( json::value_to_tag<Product>, json::value const& jv ) {
  Product msg;
  json::object const& obj = jv.as_object();
  extract( obj, "symbol", msg.symbol );
  extract( obj, "code", msg.code );
  extract( obj, "type", msg.type );
  extract( obj, "displaySymbol", msg.displaySymbol );
  extract( obj, "indexSymbol", msg.indexSymbol );
  extract( obj, "markSymbol", msg.markSymbol );
  extract( obj, "fundingRateSymbol", msg.fundingRateSymbol );
  extract( obj, "fundingRate8hSymbol", msg.fundingRate8hSymbol );
  extract( obj, "contractUnderlyingAssets", msg.contractUnderlyingAssets );
  extract( obj, "settleCurrency", msg.settleCurrency );
  extract( obj, "quoteCurrency", msg.quoteCurrency );
  extract( obj, "baseCurrency", msg.baseCurrency );
  extract( obj, "contractSize", msg.contractSize );
  extract( obj, "lotSize", msg.lotSize );
  extract( obj, "tickSize", msg.tickSize );
  extract( obj, "baseTickSize", msg.baseTickSize );
  extract( obj, "baseTickSizeEv", msg.baseTickSizeEv );
  extract( obj, "quoteTickSize", msg.quoteTickSize );
  extract( obj, "quoteTickSizeEv", msg.quoteTickSizeEv );
  extract( obj, "priceScale", msg.priceScale );
  extract( obj, "ratioScale", msg.ratioScale );
  extract( obj, "pricePrecision", msg.pricePrecision );
  extract( obj, "baseQtyPrecision", msg.baseQtyPrecision );
  extract( obj, "quoteQtyPrecision", msg.quoteQtyPrecision );
  extract( obj, "minPriceEp", msg.minPriceEp );
  extract( obj, "maxPriceEp", msg.maxPriceEp );
  extract( obj, "maxOrderQty", msg.maxOrderQty );
  extract( obj, "minOrderValue", msg.minOrderValue );
  extract( obj, "minOrderValueEv", msg.minOrderValueEv );
  extract( obj, "maxBaseOrderSize", msg.maxBaseOrderSize );
  extract( obj, "maxBaseOrderSizeEv", msg.maxBaseOrderSizeEv );
  extract( obj, "maxOrderValue", msg.maxOrderValue );
  extract( obj, "maxOrderValueEv", msg.maxOrderValueEv );
  extract( obj, "description", msg.description );
  extract( obj, "status", msg.status );
  extract( obj, "tipOrderQty", msg.tipOrderQty );
  extract( obj, "defaultTakerFee", msg.defaultTakerFee );
  extract( obj, "defaultTakerFeeEr", msg.defaultTakerFeeEr );
  extract( obj, "defaultMakerFee", msg.defaultMakerFee );
  extract( obj, "defaultMakerFeeEr", msg.defaultMakerFeeEr );
  extract( obj, "listTime", msg.listTime );
  extract( obj, "ieoOpenPriceEp", msg.ieoOpenPriceEp );
  extract( obj, "ieoInitDurationMs", msg.ieoInitDurationMs );
  extract( obj, "buyPriceUpperLimitPct", msg.buyPriceUpperLimitPct );
  extract( obj, "sellPriceLowerLimitPct", msg.sellPriceLowerLimitPct );
  return msg;
}

void Decode( json::array const& array, vProduct_t& vProduct ) {
  for ( const auto& item: array ) {
    try {
      vProduct.push_back( json::value_to<Product>( item ) );
    }
    catch (...) {
      std::cerr << "phemex::product::decode error" << std::endl;
    }
  }
}

// == riskLimits ==

riskLimits_detail tag_invoke( json::value_to_tag<riskLimits_detail>, json::value const& jv ) {
  riskLimits_detail detail;
  json::object const& obj = jv.as_object();
  extract( obj, "limit", detail.limit );
  extract( obj, "initialMargin", detail.initialMargin );
  extract( obj, "initialMarginEr", detail.initialMarginEr );
  extract( obj, "maintenanceMargin", detail.maintenanceMargin );
  extract( obj, "maintenanceMarginEr", detail.maintenanceMarginEr );
  return detail;
}

void Decode( json::array const& array, vriskLimits_t& vriskLimits ) {
  for ( const auto& item: array ) {
    try {
      riskLimits limits;
      limits.symbol = json::value_to<std::string>( item.at( "symbol" ) );
      limits.steps = json::value_to<std::string>( item.at( "steps" ) );

      json::array riskLimits = json::value_to<json::array>( item.at( "riskLimits" ) );
      for ( const auto& limit: riskLimits ) {
        riskLimits_detail detail;
        limits.vRiskLimits_detail.push_back( json::value_to<riskLimits_detail>( limit ) );
      }

      vriskLimits.emplace_back( std::move( limits ) ); // can change this to map based upon symbol
    }
    catch (...) {
      std::cerr << "phemex::riskLimits::decode error" << std::endl;
    }
  }
}

// == Leverages ==

Leverages tag_invoke( json::value_to_tag<Leverages>, json::value const& jv ) {
  Leverages leverages;
  json::object const& obj = jv.as_object();
  extract( obj, "initialMargin", leverages.initialMargin );
  extract( obj, "initialMarginEr", leverages.initialMarginEr );
  extract( obj, "options", leverages.options );
  return leverages;
}

void Decode( json::array const& array, vLeverages_t& vLeverages ) {
  try {
    for ( const auto& item: array ) {
      vLeverages.push_back( json::value_to<Leverages>( item ) );
    }
  }
  catch (...) {
    std::cerr << "phemex::Leverages::decode error" << std::endl;
  }
}

} // namespace products
} // namespace phemex
} // namespace tf
} // namespace ou
