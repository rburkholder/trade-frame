#include "StdAfx.h"
#include "SlidingWindow.h"

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

