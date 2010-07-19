/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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
#pragma once

#include <string>

#include "boost/shared_ptr.hpp"

#include "LibCommon/Delegate.h"

#include "Instrument.h"
#include "Position.h"

class CPortfolio {
public:

  struct structUpdatePortfolioRecord {
    CInstrument *pInstrument;
    int nPosition;
    double dblPrice;
    double dblAverageCost;
    structUpdatePortfolioRecord( CInstrument *pInstrument_, int nPosition_, double dblPrice_, double dblAverageCost_ )
      : pInstrument( pInstrument_ ), nPosition( nPosition_ ), dblPrice( dblPrice_ ), dblAverageCost( dblAverageCost_ ) {};
  };

  typedef boost::shared_ptr<CPortfolio> pPortfolio_t;
  typedef const structUpdatePortfolioRecord &UpdatePortfolioRecord_t;
  typedef Delegate<UpdatePortfolioRecord_t>::OnMessageHandler UpdatePortfolioRecordHandler_t;

  CPortfolio( const std::string &sPortfolioName );
  ~CPortfolio(void);

  void AddPosition( const std::string &sName, CInstrument *pInstrument );
  void Delete( const std::string &sName );
  void Rename( const std::string &sName );
  CPosition::pPosition_t Get( const std::string &sName );

  // need an on change event so delta can be recalculated on value or record addition (quote, trade, portfolio record, order, execution )


protected:
  std::string m_sPortfolioName;

  // has series of orders, COrder
  // has series of executions, CExecution
private:
};
