/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// Started 2013/10/28

#pragma once

// code follows:
// http://www.boost.org/doc/libs/1_54_0/doc/html/atomic/usage_examples.html

#include <atomic>

namespace ou { // One Unified

class SpinLock {
private:

  using LockState = enum ELockState {Locked, Unlocked};
  std::atomic<LockState> m_state;

public:

  SpinLock() { unlock(); }
  ~SpinLock() { unlock(); }  // locks on same item need to release before item on stack disappears

  void wait() {
    while ( m_state.exchange(Locked, std::memory_order_acquire) == Locked ) {
      /* busy-wait */
    }
    m_state.store(Unlocked, std::memory_order_release);
  }

  void lock() {
    while ( m_state.exchange(Locked, std::memory_order_acquire) == Locked ) {
      /* busy-wait */
    }
  }

  void unlock() {
    m_state.store( Unlocked, std::memory_order_release );
  }

};

} // namespace ou
