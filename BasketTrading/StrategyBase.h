/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/*
 * File:    StrategyBase.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on June 23, 2019, 9:28 PM
 */

#ifndef STRATEGYBASE_H
#define STRATEGYBASE_H

// has std::move
#include <utility>

namespace Strategy {

// == Common

class Common {
public:

  Common();
  virtual ~Common();

protected:

private:

};

// == Base

template<typename ComboStrategy, typename Combination>
class Base: public Common {
public:

  using combo_t = Combination;

  Base() {}
  Base( const Base&& rhs )
  : m_combo( std::move( rhs.m_combo ) ) {}
  virtual ~Base( ) {}

  Combination& Combo() { return m_combo; }

protected:

  Combination m_combo;

private:

};

} // namespace Strategy

#endif /* STRATEGYBASE_H */