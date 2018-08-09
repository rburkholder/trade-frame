/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   PortfolioGreek.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on July 31, 2018, 11:16 AM
 */

#include "PortfolioGreek.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PortfolioGreek::PortfolioGreek( const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner, EPortfolioType ePortfolioType_, 
    currency_t eCurrency, const std::string& sDescription )
: Portfolio( idPortfolio, idAccountOwner, idOwner, ePortfolioType_, eCurrency, sDescription )
{
}

PortfolioGreek::~PortfolioGreek( ) {
}

PortfolioGreek::pPositionGreek_t PortfolioGreek::AddPosition( const std::string& sName, pPositionGreek_t pPositionGreek ) {
  Portfolio::AddPosition( sName, pPositionGreek );
  return pPositionGreek;
}
  
void PortfolioGreek::AddSubPortfolio( pPortfolioGreek_t& pPortfolioGreek ) {
  pPortfolio_t pPortfolio( boost::dynamic_pointer_cast<Portfolio>( pPortfolioGreek ) );
  Portfolio::AddSubPortfolio( pPortfolio );
}

void PortfolioGreek::RemoveSubPortfolio( const idPortfolio_t& idPortfolio ) {
  Portfolio::RemoveSubPortfolio( idPortfolio );
}

std::ostream& operator<<( std::ostream& os, const PortfolioGreek& portfolio ) {

  os 
    << (Portfolio) portfolio
    ;
  return os;
}

} // namespace tf
} // namespace ou
