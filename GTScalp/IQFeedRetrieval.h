#pragma once

//#include "BufferedSocket.h"
#include "IQFeed.h"

class CIQFeedRetrieval {
public:
  CIQFeedRetrieval(void);
  virtual ~CIQFeedRetrieval(void);
  void OpenPort( void );
  void ClosePort( void );

protected:
  bool bLookingForDetail;
  CIQFeedThreadCrossing *m_pPort;

  virtual void OnNewResponse( const char *szLine ) = 0;
private:
};
