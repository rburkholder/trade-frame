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
#include "ReusableCharBuffers.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

// CCharBuffer

CCharBuffer::CCharBuffer(void) {
  m_szBuffer = new char[ m_nBasicBufferSize ];
  //m_szBuffer = (char*) malloc( m_nBasicBufferSize );
  m_nBufferSize = m_nBasicBufferSize;
  *m_szBuffer = 0;
  m_nChar = 1; 
}

CCharBuffer::CCharBuffer( int nChars, const char *szBuf ) { 
  m_nBufferSize = max( nChars, m_nBasicBufferSize );
  m_szBuffer = new char[ m_nBufferSize ];
  //m_szBuffer = (char*) malloc( m_nBasicBufferSize );
  strcpy_s( m_szBuffer, m_nBufferSize, szBuf );
  m_nChar = nChars;
}

CCharBuffer::~CCharBuffer() {
  delete [] m_szBuffer;
  //free( m_szBuffer );
  m_szBuffer = NULL;
}

CCharBuffer *CCharBuffer::Assign( int nChars, const char *szBuf ) {
  if ( nChars > m_nBufferSize ) {
    delete [] m_szBuffer;
    //m_szBuffer = (char*) realloc( m_szBuffer, nChars );
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

// CReusableCharBuffers

CReusableCharBuffers::CReusableCharBuffers(void) {
  //InitializeCriticalSection( &csLockQueue );
}

CReusableCharBuffers::~CReusableCharBuffers(void) {
  CCharBuffer *cb;
  while ( !m_qBuffer.empty() ) {
    cb = m_qBuffer.front();
    m_qBuffer.pop();
    delete cb;
  }
}

void CReusableCharBuffers::CheckIn(CCharBuffer *pBuf) {
  //EnterCriticalSection( &csLockQueue );
  boost::mutex::scoped_lock lock(m_mutex);
  m_qBuffer.push( pBuf );
  //LeaveCriticalSection( &csLockQueue );
}

CCharBuffer *CReusableCharBuffers::CheckOut(int nChars, const char *szBuf) {
  CCharBuffer *pBuf;
  //EnterCriticalSection( &csLockQueue );
  boost::mutex::scoped_lock lock(m_mutex);
  if ( m_qBuffer.empty() ) {
    pBuf = new CCharBuffer( nChars, szBuf );
  }
  else {
    pBuf = m_qBuffer.front();
    m_qBuffer.pop();
    pBuf->Assign( nChars, szBuf );
  }
  //LeaveCriticalSection( &csLockQueue );
  return pBuf;
}
