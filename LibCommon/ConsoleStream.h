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

#include <iostream>
using namespace std;

#include "FastDelegate.h"
using namespace fastdelegate;

class CConsoleStream :  public streambuf {
public:
  CConsoleStream(void);
  virtual ~CConsoleStream(void);
  typedef FastDelegate2<const char*, streamsize> OnNewStringHandler;
  void SetOnNewString( OnNewStringHandler function ) {
    OnNewString = function;
  }
  typedef FastDelegate0<> OnFlushStringHandler;
  void SetOnFlushString( OnFlushStringHandler function ) {
    OnFlushString = function;
  }
protected:
  OnNewStringHandler OnNewString;
  OnFlushStringHandler OnFlushString;

  static const unsigned short BufSize = 1024;
  char buf[ BufSize ]; // arbitrary length sized to get most console length stuff

  //virtual streamsize xsputn( const char_type* s, streamsize n );
  virtual int sync( void );
  virtual int_type overflow( int_type meta );
private:
};
