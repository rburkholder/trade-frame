/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include "SlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

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

} // namespace tf
} // namespace ou
