/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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


// class DT:  primitive data type (self assignment not checked)
//  DT should be plain old datatype

// contains single data point
// emits event when data point changes
// supplies a formatted string for output
//   blank if no value, formatted value if non-null
// may then incorporate code from CVisibleItemInDevContext to do fancy graphical self-drawing
// designed for CListCtrl custom draw
// will need to use Delegate for handling multiple views, if upper layer does not supply that functionality
// should we be storing the object to be updated, or does the delegate know where to go for its update?

#include <string>

#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>

#include "FastDelegate.h"
using namespace fastdelegate;

//#include "VisibleItemAttributes.h"

namespace ou {

/////////////////////////

// try and get rid of the virtual and make into Curiously Recurring Template... can't, base class isn't templated
// could use boost::fusion here instead.  would get rid of the need for a base class
class SmartVarBase: public boost::noncopyable {
  // simple base class for use in vector with static_cast
public:
  virtual const std::string& str( void ) = 0;
protected:
private:
};

/////////////////////////

template<typename T> class SmartVar: public SmartVarBase {
public:
  SmartVar<T>( void );
  SmartVar<T>( 
    T tBlank // GUI shows blank with this value
    );
  ~SmartVar<T>( void );

  const T& operator=(const T& rhs );
  const SmartVar<T>& operator=( const SmartVar<T>& rhs );

  T& Value( void ) { return m_tItem; };
  const std::string& str( void );
  void SetBlank( const T& tBlank ) { m_tBlank = tBlank; };

  typedef FastDelegate0<> OnUpdateHandler;
  void SetOnUpdate( OnUpdateHandler function ) {
    OnUpdate = function;
  }

  bool operator<( const SmartVar<T>& rhs ) { return m_tItem < rhs.m_tItem; };
  bool operator>( const SmartVar<T>& rhs ) { return m_tItem > rhs.m_tItem; };
  bool operator==( const SmartVar<T>& rhs ) { return m_tItem == rhs.m_tItem; };
  bool operator!=( const SmartVar<T>& rhs ) { return m_tItem != rhs.m_tItem; };
  bool operator>=( const SmartVar<T>& rhs ) { return m_tItem >= rhs.m_tItem; };
  bool operator<=( const SmartVar<T>& rhs ) { return m_tItem <= rhs.m_tItem; };

protected:
  OnUpdateHandler OnUpdate;
private:
  T m_tItem;
  T m_tBlank;
  std::string m_sItem;

  bool m_bValueUpdated;  // indicates when string has been updated
  bool m_bEventCleared;  // don't signal another event until current one processed (

  void Init(  );  // needs a specialization for each type.

//  COLORREF colourBackground;
//  COLORREF colourForeground;


};

// Constructors
template<typename T> SmartVar<T>::SmartVar( T tBlank ) 
: m_bValueUpdated( false ), m_bEventCleared( true ), m_tBlank( tBlank ), m_tItem( tBlank )
{
  Init(  );
}

template<typename T> SmartVar<T>::SmartVar( void ) 
: m_bValueUpdated( false ), m_bEventCleared( true )
{
  Init(  );
}

// Destructor
template<class T> SmartVar<T>::~SmartVar() {
}

// may need to protect cross thread updates here

// Assignment
template<typename T> const T& SmartVar<T>::operator=(const T &rhs) {
  if ( m_tItem != rhs ) {
    m_tItem = rhs;
    m_bValueUpdated = true;
    if ( m_bEventCleared ) {
      m_bEventCleared = false;
      if ( NULL != OnUpdate ) OnUpdate();
    }
  }
  return m_tItem;
}

template<typename T> const SmartVar<T>& SmartVar<T>::operator=( const SmartVar<T>& rhs ) {
  if ( m_tItem != rhs.m_tItem ) {
    m_tItem = rhs.m_tItem;
    m_bValueUpdated = true;
    if ( m_bEventCleared ) {
      m_bEventCleared = false;
      if ( NULL != OnUpdate ) OnUpdate();
    }
  }
  return *this;
}

// Convert to string for display
template<typename T> const std::string& SmartVar<T>::str() {
  if ( m_bValueUpdated ) {
    if ( m_tBlank == m_tItem ) {
      m_sItem = "";
    }
    else {
      m_sItem = boost::lexical_cast<std::string>( m_tItem );
    }
    m_bValueUpdated = false;
  }
  m_bEventCleared = true;
  return m_sItem;
}

} // ou

