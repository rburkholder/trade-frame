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

#include "CommonDatabaseFunctions.h"

#include <iostream>

class CDbValueStream : 
  public std::streambuf, 
  public CCommonDatabaseFunctions<CDbValueStream> {
public:
  CDbValueStream(void);
  ~CDbValueStream(void);
  //char *GetBuf( void ) { return pbase(); };
  //unsigned long GetLength( void ) { return pptr() - pbase(); };
  void Save( void *pKey, size_t nKeyLength );
protected:
  static const unsigned short BufSize = 1024;
  char buf[ BufSize ]; // arbitrary length sized 
  virtual int_type overflow( int_type meta );
  virtual int sync( void );
private:
};
