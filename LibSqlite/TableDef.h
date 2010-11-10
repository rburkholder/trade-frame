/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>


//
// various Objects used for processing the TableDef
//

// TableDef_CreateTables



// TableDef_BuildStatement

// TableDef_BindForWrite

// TableDef_ColumnForRead

//
// CFieldDefBase
//

class CFieldDefBase {
public:
  CFieldDefBase( const std::string& sFieldName, const std::string& sFieldType )
  : m_sFieldName( sFieldName ), m_sFieldType( sFieldType )
  {};
  virtual ~CFieldDefBase( void ) {};
protected:
  std::string m_sFieldName;
  std::string m_sFieldType;
private:
};

//
// CFieldDef
//

template<typename V> // V: variable for data
class CFieldDef: public CDbFieldDefBase {
public:
  CFieldDef( const std::string& sDbFieldName, const std::string& sDbFieldType, V& var );
  ~CFieldDef( void ) {};
protected:
private:
  V* m_var;
};

template<typename V>
CFieldDef::CDbFieldDef( const std::string& sFieldName, const std::string& sFieldType, V& v )
: CFieldDefBase( sFieldName, sFieldType ),
  m_v( v )
{
}

//
// CTableDefBase
//

class CTableDefBase {
public:

  typedef boost::shared_ptr<CTableDefBase> pCTableDefBase_t;

  CTableDefBase( void ) {};
  virtual ~CTableDefBase( void ) {};
protected:
  // definition of fields

  typedef std::vector<CFieldDefBase*> vFields_t;
  typedef vFields_t::iterator vFields_iter_t;
  vFields_t m_vFields;

  // also need to keep table of active records?
private:
};

//
// CTableDef
//

template<class TD>  // TD: TableDef
class CTableDef: public CTableDefBase {
public:
  CTableDef( void ): CTableDefBase() {};
  ~CTableDef( void ) {};
protected:
private:
};

