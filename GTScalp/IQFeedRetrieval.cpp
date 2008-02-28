#include "StdAfx.h"
#include "GTScalp.h"
#include "IQFeedRetrieval.h"

CIQFeedRetrieval::CIQFeedRetrieval(void) {
  bLookingForDetail = false;
  m_pPort = NULL;
}

CIQFeedRetrieval::~CIQFeedRetrieval(void) {
  if ( NULL != m_pPort ) {
    ClosePort();
  }
}

void CIQFeedRetrieval::OpenPort( void ) {
  bLookingForDetail = false;
  m_pPort = theApp.m_pIQFeed->CheckOutLookupPort();
  m_pPort->SetOnPostThreadCrossingResponse( MakeDelegate( this, &CIQFeedRetrieval::OnNewResponse ) );
}

void CIQFeedRetrieval::ClosePort( void ) {
  theApp.m_pIQFeed->CheckInLookupPort( m_pPort );
  m_pPort = NULL;
}

//void CIQFeedRetrieval::OnNewResponse( const char *szLine ) {
  // do something with miscellaneous lines
//}