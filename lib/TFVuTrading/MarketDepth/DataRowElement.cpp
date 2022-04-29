/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    PriceRowElement.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 10, 2021 17:54
 */

#include "DataRowElement.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

// class PriceRowElementIndicatorStatic

DataRowElementIndicatorStatic::DataRowElementIndicatorStatic( const std::string& sFormat, bool& bChanged )
: DataRowElement<std::string>( sFormat, bChanged )
{}

void DataRowElementIndicatorStatic::Append( const std::string& sValue ) {
  if ( 0 == m_sValue.size() ) {
    m_sValue = sValue;
  }
  else {
    m_sValue += " " + sValue;
  }
  m_bChanged = true;
}

void DataRowElementIndicatorStatic::UpdateWinRowElement() {
  if ( nullptr != m_pWinRowElement ) {
    m_pWinRowElement->SetText( m_sValue );
  }
}

// class PriceRowElementIndicatorDynamic

DataRowElementIndicatorDynamic::DataRowElementIndicatorDynamic( const std::string& sFormat, bool& bChanged )
: DataRowElement<std::string>( sFormat, bChanged )
, m_bListChanged( false )
{}

void DataRowElementIndicatorDynamic::Set( const std::string& sValue ) {
  bool bSet( false );
  if ( 1 == m_setIndicator.size() ) {
    setIndicator_t::const_iterator iter = m_setIndicator.find( sValue );
    if ( m_setIndicator.end() == iter ) {
      bSet = true;
    }
  }
  if ( bSet ) {
    m_setIndicator.clear();
    m_setIndicator.insert( sValue );
    m_bListChanged = true;
    m_bChanged = true;
  }
}

void DataRowElementIndicatorDynamic::Add( const std::string& sValue ) {
  setIndicator_t::const_iterator iter = m_setIndicator.find( sValue );
  if ( m_setIndicator.end() == iter ) {
    m_setIndicator.insert( sValue );
    m_bListChanged = true;
    m_bChanged = true;
  }
}

void DataRowElementIndicatorDynamic::Del( const std::string& sValue ) {
  setIndicator_t::const_iterator iter = m_setIndicator.find( sValue );
  if ( m_setIndicator.end() != iter ) {
    m_setIndicator.erase( iter );
    m_bListChanged = true;
    m_bChanged = true;
  }
}

void DataRowElementIndicatorDynamic::UpdateWinRowElement() {
  if ( nullptr != m_pWinRowElement ) {
    if ( m_bListChanged ) {
      m_sValue.clear();
      bool bSpace( false );
      for ( const setIndicator_t::value_type& vt: m_setIndicator ) {
        if ( bSpace ) {
          m_sValue += " ";
        }
        else {
          bSpace = true;
        }
        m_sValue += vt;
      }
      m_bListChanged = false;
    }
    m_pWinRowElement->SetText( m_sValue );
  }
}

} // market depth
} // namespace tf
} // namespace ou
