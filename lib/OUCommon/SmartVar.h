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

#include <sstream>
#include <string>

#include "FastDelegate.h"
using namespace fastdelegate;

#include <boost/utility.hpp>

//#include "VisibleItemAttributes.h"

namespace ou {

/////////////////////////

// try and get rid of the virtual and make into Curiously Recurring Template
class CSmartVarBase: public boost::noncopyable {
  // simple base class for use in vector with static_cast
public:
  virtual const std::string& String( void ) = 0;
protected:
private:
};

/////////////////////////

template<class DT> class CSmartVar: public CSmartVarBase {
public:
  CSmartVar<DT>( void );
  CSmartVar<DT>( 
    DT dtBlank // GUI shows blank with this value
    );
  ~CSmartVar<DT>( void );

  DT& operator=(const DT& rhs );
  DT& Value( void ) { return m_dtItem; };
  virtual const std::string& String( void );
  void SetBlank( DT dtBlank ) { m_dtBlank = dtBlank; };

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

  bool m_bValueUpdated;

  std::stringstream m_ss;

//  COLORREF colourBackground;
//  COLORREF colourForeground;

};

// Constructors
template<class DT> CSmartVar<DT>::CSmartVar( DT dtBlank ) 
: m_bValueUpdated( false ), m_dtBlank( dtBlank ), m_dtItem( dtBlank )
{
}

template<class DT> CSmartVar<DT>::CSmartVar( void ) 
: m_bValueUpdated( false )
{
}

// Destructor
template<class DT> CSmartVar<DT>::~CSmartVar() {
}

// Assignment
template<class DT> DT& CSmartVar<DT>::operator =(const DT &rhs) {
  if ( m_dtItem != rhs ) {
    m_dtItem = rhs;
    m_bValueUpdated = true;
    if ( NULL != OnUpdate ) OnUpdate();
  }
  return m_dtItem;
}

// Convert to string for display
template<class DT> const std::string& CSmartVar<DT>::String() {
  if ( m_bValueUpdated ) {
    m_ss.str( "" );
    if ( m_dtBlank != m_dtItem ) {
      m_ss << m_dtItem;
    }
    m_bValueUpdated = false;
  }
//  return m_ss.str().c_str();
  m_sItem = m_ss.str(); 
  return m_sItem;
}

} // ou