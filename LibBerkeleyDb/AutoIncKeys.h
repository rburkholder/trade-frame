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

// 2010/09/12
//  unsigned long autoinc keys for uniquely identifying records
//  for best performance, database/table should be kept open.

#include <string>

#include "KeyValuePairs.h"

//
// CAutoIncKeys
//

class CAutoIncKeys: public CKeyValuePairs<unsigned long>
{
public:

  typedef unsigned long keyValue_t;

  CAutoIncKeys(void);
  ~CAutoIncKeys(void);

  void SetNextId( const std::string& key, keyValue_t );
  keyValue_t GetNextId( const std::string& sKeyName ); 
  keyValue_t GetCurrentId( const std::string& sKeyName );

protected:

private:

  static const std::string m_sDbName;

};

//
// CAutoIncKey
//

class CAutoIncKey: public CAutoIncKeys
{
public:

  typedef unsigned long keyValue_t;

  CAutoIncKey( const std::string& sKeyName );
  ~CAutoIncKey(void);

  void SetNextId( keyValue_t );
  keyValue_t GetNextId( void ); 
  keyValue_t GetCurrentId( void );

protected:

  std::string m_sKeyName;

private:
};

