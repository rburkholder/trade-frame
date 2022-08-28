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
 * File:    PriceRowElement.h
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
  WinRowElement* GetWinRowElement() { return m_pWinRowElement; }

  virtual void UpdateWinRowElement();

  virtual void Set( const T );
  void Set( const T, bool );
  void Inc();
  void Add( T );
  T Get() const;

  using fMouseClick_t = WinRowElement::fMouseClick_t;
  void Set( fMouseClick_t&& fLeft, fMouseClick_t&& fRight );

protected:

  T m_value;

  bool& m_bChanged; // reference to global
  bool m_bHighlight;

  boost::format m_format;
  std::string m_sValue; // value to be placed in WinRowElement

  WinRowElement* m_pWinRowElement;

private:
};

template<typename T>
DataRowElement<T>::DataRowElement( const std::string& sFormat, bool& bChanged )
: m_bChanged( bChanged )
, m_bHighlight( false )
, m_format( sFormat )
, m_pWinRowElement( nullptr )
, m_value {}
{}

template<typename T>
DataRowElement<T>::~DataRowElement() {
  m_pWinRowElement = nullptr;
}

template<typename T>
void DataRowElement<T>::Set( const T value ) {
  if ( m_value != value ) {
    m_value = value;
    m_bChanged = true;
  }
}

template<typename T>
void DataRowElement<T>::Set( const T value, bool bHighlight ) {
  if ( ( m_value != value ) || ( m_bHighlight != bHighlight ) ) {
    m_value = value;
    m_bHighlight = bHighlight;
    m_bChanged = true;
  }
}

template<typename T>
T DataRowElement<T>::Get() const {
  return m_value;
}

template<typename T>
void DataRowElement<T>::Inc()  {
  m_value++;
}

template<typename T>
void DataRowElement<T>::Add( T value )  {
  m_value += value;
}

template<typename T>
void DataRowElement<T>::SetWinRowElement( WinRowElement* pwre ) {
  // TODO: is there a way to clear an attached WinRowElement
  //   will need to reset, refresh, then unattach in caller
  // TODO: clear the FMouseClick_t callbacks?
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
    m_pWinRowElement->SetText( m_sValue, m_bHighlight );
  }
}

template<typename T>
void DataRowElement<T>::Set( fMouseClick_t&& fLeft, fMouseClick_t&& fRight ) {
  assert( m_pWinRowElement );
  m_pWinRowElement->Set( std::move( fLeft ), std::move( fRight ) );
}

// class DataRowElementPrice

class DataRowElementPrice: public DataRowElement<int> {
public:
protected:
private:
};

// class PriceRowElementIndicatorStatic

class DataRowElementIndicatorStatic: public DataRowElement<std::string> {
public:
  DataRowElementIndicatorStatic( const std::string& sFormat, bool& bChanged );
  virtual void UpdateWinRowElement();
  void Append( const std::string& );
protected:
private:
};

// class PriceRowElementIndicatorDynamic

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
