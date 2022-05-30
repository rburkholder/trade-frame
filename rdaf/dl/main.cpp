/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Main.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/dl
 * Created: May 29, 2022  14:42
 */

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/SymbolLookup.h>

#include <TFTrading/AcquireFundamentals.h>

#include "Config.hpp"

namespace {
const size_t nInTransit = 40;
}

void HandleConnected( int ) {

}

int main( int argc, char* argv[] ) {

  config::Choices choices;
  config::Load( "rdaf/rdaf_dl.cfg", choices );

  //using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;
  //std::shared_ptr<ou::tf::AcquireFundamentals> m_pAcquireFundamentals_live;

  struct InProgress {
    //vSymbols_iter iterSymbols;
    //pAcquireFundamentals_t pAcquireFundamentals;
  };

  using pIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  pIQFeed_t m_piqfeed;

  m_piqfeed = ou::tf::iqfeed::IQFeedProvider::Factory();

//  m_piqfeed->OnConnected.Add( &HandleConnected );
//  m_piqfeed->Connect();

  return 0;
}
