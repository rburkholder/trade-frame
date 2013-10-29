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

// http://www.boost.org/doc/libs/1_54_0/doc/html/atomic/usage_examples.html

#include <boost/atomic.hpp>

namespace ou { // One Unified

class SpinLock {
private:

  typedef enum ELockState {Locked, Unlocked} LockState;
  boost::atomic<LockState> m_state;

public:

  SpinLock(): m_state(Unlocked) {}

  void wait() {
    while (m_state.exchange(Locked, boost::memory_order_acquire) == Locked) {
      /* busy-wait */
    }
    m_state.store(Unlocked, boost::memory_order_release);
  }

  void lock() {
    while (m_state.exchange(Locked, boost::memory_order_acquire) == Locked) {
      /* busy-wait */
    }
  }

  void unlock() {
    m_state.store(Unlocked, boost::memory_order_release);
  }

};

} // namespace ou
