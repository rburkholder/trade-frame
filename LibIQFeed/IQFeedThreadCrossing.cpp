#include "StdAfx.h"
#include "IQFeedThreadCrossing.h"

#include <iostream>
#include <stdexcept>
using namespace std;

CIQFeedThreadCrossing::CIQFeedThreadCrossing() : m_bActive( false ), m_nActivations( 0 ) {
  InitializeCriticalSection( &csProtectQueues );
}

CIQFeedThreadCrossing::~CIQFeedThreadCrossing() {
  if ( !m_responses.empty() ) {
    cout << "CIQFeedThreadCrossing::m_responses is " << m_responses.size() << endl;
  }
  // TODO:  // destroy the buffer properly
  // need to empty out queue
  while ( !m_responses.empty() ) {
    CCharBuffer *cb = m_responses.front();
    //delete cb;
    m_EmptyBuffers.CheckIn( cb );
    m_responses.pop();
  }
  // CReusableCharBuffers destructor probably needs fixing
}

void CIQFeedThreadCrossing::Activate( void ) {
  ASSERT( !m_bActive );
  m_bActive = true;
  ++m_nActivations;
}

void CIQFeedThreadCrossing::Deactivate( void ) {
  ASSERT( m_bActive );
  m_bActive = false;
}

void CIQFeedThreadCrossing::OpenSocket( const char *pAddress, unsigned short port ) {
  //ASSERT( bActive );
  m_socket.SetObject( this );
  m_socket.Open( pAddress, port );
}

void CIQFeedThreadCrossing::SendToSocket( const char *pCommand ) {
  ASSERT( m_bActive );
  m_socket.Send( pCommand );
}

void CIQFeedThreadCrossing::CloseSocket() {
  //ASSERT( bActive );
  m_socket.Close();
}

void CIQFeedThreadCrossing::QueueResponse(unsigned short nChars, const char *buf) {
  // todo:  I've been caught here once with data still arriving,  need to track from whence it comes
  ASSERT( m_bActive );
  EnterCriticalSection( &csProtectQueues );
  CCharBuffer *cb = m_EmptyBuffers.CheckOut( nChars, buf );
  m_responses.push( cb );
  LeaveCriticalSection( &csProtectQueues );
}

void CIQFeedThreadCrossing::ProcessResponse() {
  ASSERT( m_bActive );
  ASSERT( !m_responses.empty() );
  EnterCriticalSection( &csProtectQueues );
  CCharBuffer *cb = m_responses.front();
  m_responses.pop();
  LeaveCriticalSection( &csProtectQueues );

  if ( NULL != OnNewResponse ) OnNewResponse( cb->Buffer() ); 

  EnterCriticalSection( &csProtectQueues );
  m_EmptyBuffers.CheckIn( cb );
  LeaveCriticalSection( &csProtectQueues );
}

