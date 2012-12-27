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
#include <stdexcept>

#include "FastDelegate.h"
using namespace fastdelegate;

#include "ReusableBuffers.h"

// http://www.cplusplus.com/reference/iostream/streambuf/setp.html
// http://blogs.awesomeplay.com/elanthis/archives/2007/12/10/444/
// http://cboard.cprogramming.com/cplusplus-programming/111019-trying-override-streambuf-use-cout.html
// http://www.dreamincode.net/code/snippet2499.htm

namespace ou {

template <typename charT, typename traits = std::char_traits<charT> >
class ConsoleStreamBuf:  public std::basic_streambuf<charT, traits> {
public:

  typedef traits traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

  static const size_t nCharT = 512;

  struct Buf {
    size_t n;
    charT buf[ nCharT ];
    Buf( void ): n( 0 ) {};
  };

  ConsoleStreamBuf(void);
  virtual ~ConsoleStreamBuf(void);

  typedef FastDelegate1<Buf*> OnEmitStringHandler;
  void SetOnEmitString( OnEmitStringHandler function ) {
    OnEmitString = function;
  }
  //typedef FastDelegate0<> OnFlushStringHandler;
  //void SetOnFlushString( OnFlushStringHandler function ) {
  //  OnFlushString = function;
  //}

  void ReturnBuffer( Buf* buf ) { buf->n = 0; m_buffers.CheckInL( buf ); };
protected:
  virtual int_type overflow( int_type meta );
  virtual int_type sync( void );
private:

  //charT m_buf[ nBufSize ]; // arbitrary length sized to get most console length stuff
  Buf* m_pBuf;
  BufferRepository<Buf> m_buffers;

  OnEmitStringHandler OnEmitString;
  //OnFlushStringHandler OnFlushString;
};

template <typename charT, typename traits>
ConsoleStreamBuf<charT,traits>::ConsoleStreamBuf( void ): std::basic_streambuf<charT, traits>() {
  m_pBuf = m_buffers.CheckOutL();
  setp( m_pBuf->buf, m_pBuf->buf + nCharT - 1 );  // allow for one of our own terminating characters
}

template <typename charT, typename traits>
ConsoleStreamBuf<charT,traits>::~ConsoleStreamBuf( void ) {
  if ( 0 != m_pBuf->n ) {
    m_pBuf->n = 0; 
  }
  m_buffers.CheckInL( m_pBuf );
}

template <typename charT, typename traits>
typename ConsoleStreamBuf<charT, traits>::int_type 
ConsoleStreamBuf<charT,traits>::sync() {
  return traits_type::eq_int_type( this->overflow(traits_type::eof() ), traits_type::eof() ) ? -1 : 0;
}

template <typename charT, typename traits>
typename ConsoleStreamBuf<charT, traits>::int_type 
ConsoleStreamBuf<charT,traits>::overflow( typename ConsoleStreamBuf<charT, traits>::int_type meta ) {

//  charT* ibegin = this->out_buf_;
  charT* iend = this->pptr();

  // If this is the end, add an eof character to the buffer.
  // This is why the pointers passed to setp are off by 1
  // (to reserve room for this).
  if( !traits_type::eq_int_type( meta, traits_type::eof() ) ) {
      *iend++ = traits_type::to_char_type( meta );
  }

  m_pBuf->n = iend - pbase();
  //if ( NULL != OnNewString ) OnNewString( pbase(), (int) ( pptr() - pbase() - 1 ) ); // assumes CR at end
  if ( NULL != OnEmitString ) OnEmitString( m_pBuf ); 
  //if ( NULL != OnFlushString ) OnFlushString();
  //setp( pbase(), epptr() );  // had this first but following line just resets everything.  better?  does the same thing?
  m_pBuf = m_buffers.CheckOutL();
  setp( m_pBuf->buf, m_pBuf->buf + nCharT - 1 );  // allow for one of our own terminating characters
//  if ( NULL != OnFlushString ) OnFlushString();

  return traits_type::not_eof( meta );

}

} // ou