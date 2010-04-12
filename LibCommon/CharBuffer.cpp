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

#include "StdAfx.h"

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <string.h> 
#include <algorithm>

// custom off

#include "CharBuffer.h"

// CCharBuffer

CCharBuffer::CCharBuffer(void) {
  m_szBuffer = new char[ m_nBasicBufferSize ];
  m_nBufferSize = m_nBasicBufferSize;
  *m_szBuffer = 0;
  m_nChar = 1; 
}

CCharBuffer::CCharBuffer( int nChars, const char *szBuf ) { 
  m_nBufferSize = std::max<int>( nChars, m_nBasicBufferSize );
  m_szBuffer = new char[ m_nBufferSize ];
  strcpy_s( m_szBuffer, m_nBufferSize, szBuf );
  m_nChar = nChars;
}

CCharBuffer::~CCharBuffer() {
  delete [] m_szBuffer;
  m_szBuffer = NULL;
}

CCharBuffer *CCharBuffer::Assign( int nChars, const char *szBuf ) {
  if ( nChars > m_nBufferSize ) {
    delete [] m_szBuffer;
    m_nBufferSize = nChars;
    m_szBuffer = new char[ m_nBufferSize ];
  }
  strcpy_s( m_szBuffer, m_nBufferSize, szBuf );
  m_nChar = nChars;
  return this;
}

//CCharBuffer *CCharBuffer::Duplicate(int nChars, const char *szBuf) {
//  Copy( nChars, szBuf );
//  return this;
//}

