#pragma once

//#include "BufferedSocket.h"
#include "IQFeed.h"
#include "IQFeedProvider.h"

class CIQFeedRetrieval {
public:
  CIQFeedRetrieval( CIQFeedProvider *pProvider );
  virtual ~CIQFeedRetrieval(void);
  void OpenPort( void );
  void ClosePort( void );

protected:
  bool m_bLookingForDetail;
  CIQFeedThreadCrossing *m_pPort;

  CIQFeedProvider *m_pIQFeedProvider;
  virtual void OnNewResponse( const char *szLine ) = 0;
private:
};
