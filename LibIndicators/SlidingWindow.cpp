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
  // replicate the code from the base class just to be sure the objects get cleared out.
  while ( !m_qT.empty() ) {
    //delete m_qT.front();
    //m_qT.pop_front();
    Remove(); // this ensures inheritor has a chance to delete the held object
  }
}

CBar *CSlidingWindowBars::Remove( void ) {
  // default deleter so it doesn't end up no where
  CBar *pBar = CSlidingWindow::Remove();
  delete pBar;
  return NULL;
}

