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

#include "boost/shared_ptr.hpp"

#include "Instrument.h"

class CPortfolioRecord {
public:
  CPortfolioRecord(void);
  ~CPortfolioRecord(void);

  typedef boost::shared_ptr<CPortfolioRecord> pPortfolioRecord_t;

protected:
  CInstrument *m_pInstrument;
  int m_nPosition;
  double m_dblPrice;
  double m_dblAverageCost;
private:
};
