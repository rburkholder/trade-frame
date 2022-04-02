/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    AcquireFundamentals.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on September 18, 2021, 14:21
 */

#include <functional>

#include <TFTrading/Watch.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class AcquireFundamentals {
public:

  using pWatch_t = Watch::pWatch_t;

  using pAcquireFundamentals_t = std::shared_ptr<AcquireFundamentals>;
  using fDone_t = std::function<void(pWatch_t)>;

  AcquireFundamentals( pWatch_t&& pWatch_, fDone_t&& fDone_ );
  ~AcquireFundamentals();

  static pAcquireFundamentals_t Factory( pWatch_t&& pWatch, fDone_t&& fDone ) {
    return std::make_shared<AcquireFundamentals>( std::move( pWatch ), std::move( fDone ) );
  }

  void Start();

private:

  pWatch_t pWatch;
  fDone_t fDone;

  void HandleFundamentals( const Watch::Fundamentals& fundamentals );
  void HandleTrade( const Trade& trade );

};

} // namespace tf
} // namespace ou
