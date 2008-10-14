#pragma once

#include <string>

#include "Delegate.h"
#include "GuiThreadCrossing.h"

class CCrossThreadCout :  public CGuiThreadCrossing {
  DECLARE_DYNAMIC(CCrossThreadCout)
public:
  CCrossThreadCout(void);
  virtual ~CCrossThreadCout(void);
  void SendLineXThread( std::string *pLine );
  void SendNewLineXThread( void );
  virtual void HandleXThreadLine( const std::string &s ) = 0;
  virtual void HandleXThreadNewLine( void ) = 0;
protected:
	DECLARE_MESSAGE_MAP()
private:
  LRESULT OnLine( WPARAM w, LPARAM l );
  LRESULT OnNewLine( WPARAM w, LPARAM l );
};
