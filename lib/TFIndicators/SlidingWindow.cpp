/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "SlidingWindow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

SlidingWindowBars::SlidingWindowBars( size_t WindowSizeSeconds, size_t WindowSizeCount) :
  SlidingWindow<Bar>( WindowSizeSeconds, WindowSizeCount ) {
}

SlidingWindowBars::~SlidingWindowBars() {
  // replicate the code from the base class just to be sure the objects get cleared out.
  while ( !m_qT.empty() ) {
    //delete m_qT.front();
    //m_qT.pop_front();
    Remove(); // this ensures inheritor has a chance to delete the held object
  }
}

Bar* SlidingWindowBars::Remove() {
  // default deleter so it doesn't end up no where
  Bar *pBar = SlidingWindow::Remove();
  delete pBar;
  return NULL;
}

} // namespace tf
} // namespace ou
