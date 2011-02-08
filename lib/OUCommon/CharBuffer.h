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

namespace ou {

// this  thing may be obsolete as CCharBuffer can be a vector<>, 
//   and CReusableCharBuffers is only need when running with multiple threads

class CCharBuffer {
public:
  CCharBuffer(void);  // allocate default sized buffer
  CCharBuffer( int nChars, const char *szBuf ); // allocate with nchars, nchars includes terminator, if used
  ~CCharBuffer(void);
  //CCharBuffer *Duplicate( int nChars, const char *szBuf );
  CCharBuffer *Assign( int nChars, const char *szBuf );  // assigns zero terminated string, nchars includes space for terminator

  int Size( void ) { return m_nChar; };
  const char *Buffer( void ) { return m_szBuffer; };
  
protected:
  static const int m_nBasicBufferSize = 100;  // default size of buffer
  int m_nBufferSize;  // number of characters including terminator
  int m_nChar; // number of char actually in buffer, including terminator
  char *m_szBuffer;
private:
};

} // ou