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

// started 2013-04-06

#pragma once

#include "TradingEnumerations.h"
#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// maintains ratio to base currency
// needs to have a base currency marker, on second thought, this is the user's account profile

// portfolios can not be cross currency, one currency only
// but.... positions may be in one currency, but commissions will be in base currency
//   commission field will therefore need a currency designator
//     whereas portfolio, position, order, execution will have implied currency

// the below information is already available in the Currency enumeration.  
// once fields are added, then the table may become useful

/*
class Currency {
public:
  Currency(void);
  ~Currency(void);

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "currencyid", idCurrency );
      ou::db::Field( a, "description" , sDescription );
    }
  };
protected:
private:
};
*/
} // namespace tf
} // namespace ou
