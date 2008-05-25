#include "StdAfx.h"
//#include "GTScalp.h"
#include "IQFeedRetrieval.h"

CIQFeedRetrieval::CIQFeedRetrieval( CIQFeedProvider *pProvider ) 
: m_pIQFeedProvider( pProvider ), m_pPort( NULL ), m_bLookingForDetail( false )
{
}

CIQFeedRetrieval::~CIQFeedRetrieval(void) {
  if ( NULL != m_pPort ) {
    ClosePort();
  }
}

void CIQFeedRetrieval::OpenPort( void ) {
  m_bLookingForDetail = false;
  m_pPort = m_pIQFeedProvider->CheckOutLookupPort();
  m_pPort->SetOnPostThreadCrossingResponse( MakeDelegate( this, &CIQFeedRetrieval::OnNewResponse ) );
}

void CIQFeedRetrieval::ClosePort( void ) {
  m_pIQFeedProvider->CheckInLookupPort( m_pPort );
  m_pPort = NULL;
}

//void CIQFeedRetrieval::OnNewResponse( const char *szLine ) {
  // do something with miscellaneous lines
//}