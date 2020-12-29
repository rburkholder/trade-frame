/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started 2015/11/11

#pragma once

#include <TFBitsNPieces/ReadCboeWeeklyOptionsXls.h>

class Process {
public:
  Process();
  virtual ~Process();

  void LoadWeeklies( void );  // later, will need to pass in repository
protected:
private:

  ou::tf::cboe::OptionExpiryDates_t m_cboeExpiries;
  ou::tf::cboe::vUnderlyinginfo_t m_cboeVui;

};
