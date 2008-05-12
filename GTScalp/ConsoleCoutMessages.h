#pragma once
#include "consolemessages.h"
#include "..\LibCommon\ConsoleStream.h"

#include <iostream>
using namespace std;

class CConsoleCoutMessages :
  public CConsoleMessages {
public:
  CConsoleCoutMessages(CWnd* pParent /*=NULL*/);
  virtual ~CConsoleCoutMessages(void);
protected:
  streambuf *m_pOldStreambuf;
  CConsoleStream m_ConsoleStream;

  void HandleLine( const char* s, int n );
  void HandleEndOfLine( void );
private:
};
