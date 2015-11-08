/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <TFBitsNPieces/InstrumentState.h>

class SignalsWithQuotes {
public:

  SignalsWithQuotes(void);
  ~SignalsWithQuotes(void);

  void ZeroPosition( const ou::tf::Quote& quote, InstrumentState* pis );
//  void ZeroPosition( const ou::tf::CTrade& trade ) {};

  void LongPosition( const ou::tf::Quote& quote, InstrumentState* pis );
//  void LongPosition( const ou::tf::CTrade& trade ) {};

  void ShortPosition( const ou::tf::Quote& quote, InstrumentState* pis );
//  void ShortPosition( const ou::tf::CTrade& trade ) {};

protected:
private:
};

