#include "StdAfx.h"
#include "SlidingWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSlidingWindowBars::CSlidingWindowBars(unsigned int WindowSizeSeconds, unsigned int WindowSizeCount) : 
  CSlidingWindow<CBar>( WindowSizeSeconds, WindowSizeCount ) {
}

CSlidingWindowBars::~CSlidingWindowBars() {
}

CBar *CSlidingWindowBars::Remove( void ) {
  // default deleter so it doesn't end up no where
  CBar *pBar = CSlidingWindow::Remove();
  delete pBar;
  return NULL;
}

