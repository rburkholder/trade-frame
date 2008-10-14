#pragma once
#include "consolemessages.h"
#include "..\LibCommon\ConsoleStream.h"
#include "CrossThreadCout.h"

#include <iostream>
using namespace std;

class CConsoleCoutMessages :
  public CConsoleMessages, CCrossThreadCout {
public:
  CConsoleCoutMessages(CWnd* pParent /*=NULL*/);
  virtual ~CConsoleCoutMessages(void);
protected:
  streambuf *m_pOldStreambuf;
  CConsoleStream m_ConsoleStream;

  void HandleLine( const char* s, int n );
  void HandleEndOfLine( void );

  virtual void HandleXThreadLine( const std::string &s );
  virtual void HandleXThreadNewLine( void );

private:
};
