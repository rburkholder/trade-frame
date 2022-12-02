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
  DataRowElement( const DataRowElement& ) = delete; // can't be copied, &bChanged needs to be changed
  virtual ~DataRowElement();

  void SetWinRowElement( bool bFirst, WinRowElement* );
  WinRowElement* GetWinRowElement() { return m_pWinRowElement; }

  virtual void UpdateWinRowElement();

  using EColour = WinRowElement::EColour;

  virtual void Set( const T );
  void Set( const T, bool bHighlight );
  void Set( const T, EColour bg );
  void Set( const T, EColour fg, EColour bg );
  void Inc();
  void Add( T );
  T Get() const;

protected:

  bool& m_bChanged; // reference to global

  T m_value;

  boost::format m_format;
  std::string m_sValue; // value to be placed in WinRowElement

  bool m_bHighlight;

  EColour m_colourBackground;
  EColour m_colourForeground;

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
, m_colourBackground( EColour::White )
, m_colourForeground( EColour::Black )
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
void DataRowElement<T>::Set( const T value, EColour bg ) {
  if ( ( m_value != value ) || ( bg != m_colourBackground ) ) {
    m_value = value;
    m_colourBackground = bg;
    m_bChanged = true;
  }
}

template<typename T>
void DataRowElement<T>::Set( const T value, EColour fg, EColour bg ) {
  if ( ( m_value != value ) || ( fg != m_colourForeground ) || ( bg != m_colourBackground ) ) {
    m_value = value;
    m_colourBackground = bg;
    m_colourForeground = fg;
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
void DataRowElement<T>::SetWinRowElement( bool bFirst, WinRowElement* pwre ) {
  // TODO: is there a way to clear an attached WinRowElement
  //   will need to reset, refresh, then unattach in caller
  // TODO: clear the FMouseClick_t callbacks?
  m_pWinRowElement = pwre;
  if ( bFirst ) {
    m_colourBackground = pwre->GetColourBackground();
    m_colourForeground = pwre->GetColourForeground();
  }
}

// TODO:
//   update WinRowElement upon setting, or trigger bChanged
//   the refresh is creating excessive computation
//   should only need to update the gui element upon:
//     a) attachment
//     b) update
//   ie, all this stuff here should only be performed when:
//     a) m_value or a colour changes
//     b) new WinRowElement attached
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
    m_pWinRowElement->SetColourBackground( m_colourBackground );
    m_pWinRowElement->SetColourForeground( m_colourForeground );
    m_pWinRowElement->SetText( m_sValue, m_bHighlight );
  }
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
