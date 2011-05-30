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

// try and get rid of the virtual and make into Curiously Recurring Template
class SmartVarBase: public boost::noncopyable {
  // simple base class for use in vector with static_cast
public:
  virtual const std::string& AsString( void ) = 0;
protected:
private:
};

/////////////////////////

template<class DT> class SmartVar: public SmartVarBase {
public:
  SmartVar<DT>( void );
  SmartVar<DT>( 
    DT dtBlank // GUI shows blank with this value
    );
  ~SmartVar<DT>( void );

  DT& operator=(const DT& rhs );
  DT& Value( void ) { return m_dtItem; };
  const std::string& AsString( void );
  void SetBlank( const DT& dtBlank ) { m_dtBlank = dtBlank; };

  typedef FastDelegate0<> OnUpdateHandler;
  void SetOnUpdate( OnUpdateHandler function ) {
    OnUpdate = function;
  }

protected:
  OnUpdateHandler OnUpdate;
private:
  DT m_dtBlank;
  DT m_dtItem;
  std::string m_sItem;

  bool m_bValueUpdated;  // indicates when string has been updated
  bool m_bEventCleared;  // don't signal another event until current one processed (

//  COLORREF colourBackground;
//  COLORREF colourForeground;

};

// Constructors
template<class DT> SmartVar<DT>::SmartVar( DT dtBlank ) 
: m_bValueUpdated( false ), m_bEventCleared( true ), m_dtBlank( dtBlank ), m_dtItem( dtBlank ), 
{
}

template<class DT> SmartVar<DT>::SmartVar( void ) 
: m_bValueUpdated( false ), m_bEventCleared( true )
{
}

template<> SmartVar<int>::SmartVar( void ) 
: m_bValueUpdated( false ), m_bEventCleared( true ), m_dtBlank( 0 ), m_dtItem( 0 ) {
}

template<> SmartVar<unsigned int>::SmartVar( void ) 
: m_bValueUpdated( false ), m_bEventCleared( true ), m_dtBlank( 0 ), m_dtItem( 0 ) {
}

template<> SmartVar<double>::SmartVar( void ) 
: m_bValueUpdated( false ), m_bEventCleared( true ), m_dtBlank( 0.0 ), m_dtItem( 0.0 ) {
}

// Destructor
template<class DT> SmartVar<DT>::~SmartVar() {
}

// Assignment
template<class DT> DT& SmartVar<DT>::operator=(const DT &rhs) {
  if ( m_dtItem != rhs ) {
    m_dtItem = rhs;
    m_bValueUpdated = true;
    if ( m_bEventCleared ) {
      m_bEventCleared = false;
      if ( NULL != OnUpdate ) OnUpdate();
    }
    
  }
  return m_dtItem;
}

// Convert to string for display
template<class DT> const std::string& SmartVar<DT>::AsString() {
  if ( m_bValueUpdated ) {
    if ( m_dtBlank == m_dtItem ) {
      m_sItem = "";
    }
    else {
      m_sItem = boost::lexical_cast<std::string>( m_dtItem );
    }
    m_bValueUpdated = false;
  }
  m_bEventCleared = true;
  return m_sItem;
}

} // ou

