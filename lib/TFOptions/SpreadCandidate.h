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
 * File:    SpreadCandidate.h
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on May 25, 2019, 1:25 PM
 */

#ifndef SPREADCANDIDATE_H
#define SPREADCANDIDATE_H

#include <TFTrading/Watch.h>

class SpreadCandidate {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  SpreadCandidate();
  SpreadCandidate( const SpreadCandidate& rhs ) = delete;
  SpreadCandidate( const SpreadCandidate&& rhs );
  SpreadCandidate( pWatch_t pWatch );
  ~SpreadCandidate();

  void Clear();
  void SetWatch( pWatch_t pWatch );
  pWatch_t GetWatch();
  bool ValidateSpread( size_t nDuration );

private:
  ou::tf::Quote m_quote;
  size_t m_nDesired;
  size_t m_nUnDesired;
  size_t m_nConsecutiveSpreadOk;
  pWatch_t m_pWatch;
  void UpdateQuote( const ou::tf::Quote& quote );
};

#endif /* SPREADCANDIDATE_H */
