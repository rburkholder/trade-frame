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
 * File:    DataRowElement.h
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 10, 2021 17:54
 */

#pragma once

#include <set>
#include <string>

#include <boost/format.hpp>

#include "WinRowElement.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

template<typename T>
class DataRowElement {
public:

  DataRowElement( const std::string& sFormat, bool& bChanged );
  virtual ~DataRowElement();

  void SetWinRowElement( WinRowElement* );

  virtual void UpdateWinRowElement();

  virtual void Set( T );
  T Get() const;

protected:

  bool& m_bChanged; // reference to global
  T m_value;

  boost::format m_format;
  std::string m_sValue; // value to be placed in WinRowElement

  WinRowElement* m_pWinRowElement;

private:
};

template<typename T>
DataRowElement<T>::DataRowElement( const std::string& sFormat, bool& bChanged )
: m_format( sFormat ), m_bChanged( bChanged ),
  m_pWinRowElement( nullptr ),
  m_value {}
{}

template<typename T>
DataRowElement<T>::~DataRowElement() {
  m_pWinRowElement = nullptr;
}

template<typename T>
void DataRowElement<T>::Set( T value ) {
  m_value = value;
  m_bChanged = true;
}

template<typename T>
T DataRowElement<T>::Get() const {
  return m_value;
}

template<typename T>
void DataRowElement<T>::SetWinRowElement( WinRowElement* pwre ) {
  m_pWinRowElement = pwre;
}

template<typename T>
void DataRowElement<T>::UpdateWinRowElement() {
  if ( 0 != m_value ) {
    m_format % m_value;
    m_sValue = m_format.str();
  }
  else {
    m_sValue.clear();
  }
  if ( nullptr != m_pWinRowElement ) {
    m_pWinRowElement->SetText( m_sValue );
  }
}

// class DataRowElementPrice

class DataRowElementPrice: public DataRowElement<int> {
public:
protected:
private:
};

// class DataRowElementIndicatorStatic

class DataRowElementIndicatorStatic: public DataRowElement<std::string> {
public:
  DataRowElementIndicatorStatic( const std::string& sFormat, bool& bChanged );
  virtual void UpdateWinRowElement();
  void Set( const std::string& );
protected:
private:
};

// class DataRowElementIndicatorDynamic

class DataRowElementIndicatorDynamic: public DataRowElement<std::string> {
public:
  DataRowElementIndicatorDynamic( const std::string& sFormat, bool& bChanged );
  virtual void UpdateWinRowElement();
  virtual void Set( const std::string& );
  void Add( const std::string& );
  void Del( const std::string& );
protected:
private:
  using setIndicator_t = std::set<std::string>;
  setIndicator_t m_setIndicator;
  bool m_bListChanged;
};

} // market depth
} // namespace tf
} // namespace ou
