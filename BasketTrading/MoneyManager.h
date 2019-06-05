/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:   MoneyManager.h
 * Author: raymond@burkholder.net
 *
 * Created on June 5, 2019, 8:37 AM
 */

#ifndef MONEYMANAGER_H
#define MONEYMANAGER_H

#include <set>

#include <OUCommon/ManagerBase.h>

#include <TFTrading/Watch.h>
#include <TFOptions/Option.h>
#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class MoneyManager: public ou::db::ManagerBase<MoneyManager> {
public:

  using pOption_t = ou::tf::option::Option::pOption_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  MoneyManager();
  virtual ~MoneyManager();

  bool Authorize( const std::string& ); // interim brute force solution based upon instrument count
  bool Authorize( pOrder_t&, pPosition_t&, pPortfolio_t& ); // underlying
  bool Authorize( pOrder_t&, pPosition_t&, pPortfolio_t&, pWatch_t& ); // option

  using authorized_t = std::pair<size_t,double>;
  authorized_t Authorized() const;

  void AttachToSession( ou::db::Session* pSession );
  void DetachFromSession( ou::db::Session* pSession );

protected:
private:

  size_t m_nAuthorized; // number of authorizations;
  double m_dblAuthorized;

  using setNames_t = std::set<std::string>;

  setNames_t m_setAuthorized; // allows one authorization per underlying
  setNames_t m_setRejected; // allows one message per rejected

};

} // namespace tf
} // namespace ou

#endif /* MONEYMANAGER_H */

