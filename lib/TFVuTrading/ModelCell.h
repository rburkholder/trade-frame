/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// Started 2013/11/18

#pragma once

#include <string>

#include <boost/lexical_cast.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

// this may cause problems or may result from problems if some other include hiearchy uses the file differently
#define WINRGB
#include <OUCommon/Colour.h>

// when ready for owner draw
// depends upon amount of flicker
// gtscalp/datarow (composition of elements), datarowelements(data change management), visibleelement (draw onto graphics area) for 

namespace ou { // One Unified
namespace tf { // TradeFrame

// idea for traits comes from
// http://stackoverflow.com/questions/6006614/c-static-polymorphism-crtp-and-using-typedefs-from-derived-classes?rq=1

template<typename CRTP>
struct ModelCell_traits;

template<typename CRTP> // CRTP, with conversion specifics, defines value_type.
class ModelCell {
public:

  typedef typename ModelCell_traits<CRTP>::value_type value_type;
  typedef FastDelegate1<const wxString&> FunctionSetText_t;

  ModelCell(void);
  ModelCell(FunctionSetText_t);
  virtual ~ModelCell(void);

  void SetValue( const value_type& val );
  const value_type GetValue( void ) const { return m_val; };

  void SetFunctionSetText( FunctionSetText_t function ) {
    m_functionSetText = function;
  }

  const wxString& GetText( void ) { 
    if ( m_bChanged ) {
      Val2String();
      m_bChanged = false;
    }
    return m_sCellText;
  }

  void UpdateGui( void );

  template<typename F>
  void UpdateGui( F f ) {
    typedef void type;
    if ( m_bChanged ) {
      Val2String();
      f( m_sCellText );
      m_bChanged = false;
    }
  }

protected:
  bool m_bChanged;  // may need a spinlock for this variable to handle background thread updates
  value_type m_val;
  wxString m_sCellText;
  void Val2String( void );
  void InitializeValue( void );
private:
  FunctionSetText_t m_functionSetText;
};

// ======================

class ModelCellInt;

template<>
struct ModelCell_traits<ModelCellInt> {
  typedef int value_type;
};

class ModelCellInt: public ModelCell<ModelCellInt> {
public:

  ModelCellInt( void ) {};
  ModelCellInt(FunctionSetText_t function): ModelCell<ModelCellInt>( function ) {};
  //~ModelCellInt( void );

  void InitializeValue( void ) { m_val = 0; }
protected:
private:
};

// ======================

class ModelCellDouble;

template<>
struct ModelCell_traits<ModelCellDouble> {
  typedef double value_type;
};

class ModelCellDouble: public ModelCell<ModelCellDouble> {
public:

  ModelCellDouble( void ): m_nPrecision( 2 ) {};
  ModelCellDouble(FunctionSetText_t function): ModelCell<ModelCellDouble>( function ), m_nPrecision( 2 ) {};
  //~ModelCellDouble( void );

  void InitializeValue( void ) { m_val = 0.0; }

  void SetPrecision( unsigned int n ) { m_nPrecision = n; };
protected:
private:
  unsigned int m_nPrecision;  // not use yet
};

// ======================

class ModelCellString;

template<>
struct ModelCell_traits<ModelCellString> {
  typedef std::string value_type;
};

class ModelCellString: public ModelCell<ModelCellString> {
public:

  ModelCellString( void ) {};
  ModelCellString(FunctionSetText_t function): ModelCell<ModelCellString>( function ) {};
  //~ModelCellInt( void );

  void Val2String( void ) { m_sCellText = m_val.c_str(); };
protected:
private:
};

// ======================

template<typename CRTP>
ModelCell<CRTP>::ModelCell( void )
: m_bChanged( true ), m_functionSetText( 0 ) 
{ // gets initial value into gui
  InitializeValue();
}

template<typename CRTP>
ModelCell<CRTP>::ModelCell( FunctionSetText_t function )
: m_bChanged( true ), m_functionSetText( function ) 
{ // gets initial value into gui
  InitializeValue();
}

template<typename CRTP>
ModelCell<CRTP>::~ModelCell(void) {
}

template<typename CRTP>
void ModelCell<CRTP>::SetValue( const value_type& val ) {
  if ( m_val != val ) {
    m_val = val;
    m_bChanged = true;
  }
}

template<typename CRTP>
void ModelCell<CRTP>::UpdateGui( void ) {
  if ( m_bChanged && ( 0 != m_functionSetText ) ) {
    Val2String();
    m_functionSetText( m_sCellText );
    m_bChanged = false;
  }
}

template<typename CRTP>
void ModelCell<CRTP>::Val2String( void ) {
  if ( &CRTP::Val2String != &ModelCell<CRTP>::Val2String ) {
    static_cast<CRTP*>(this)->Val2String();
  }
  else {
    std::string s( boost::lexical_cast<std::string>( m_val ) ); 
    m_sCellText = s.c_str();
  }
}

template<typename CRTP>
void ModelCell<CRTP>::InitializeValue( void ) {
  if ( &CRTP::InitializeValue != &ModelCell<CRTP>::InitializeValue ) {
    static_cast<CRTP*>(this)->InitializeValue();
  }
}

} // namespace tf
} // namespace ou
