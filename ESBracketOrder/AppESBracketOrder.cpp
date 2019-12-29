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
 * File:   AppESBracketOrder.cpp
 * Author: raymond@burkholder.net
 * Project: ESBracketOrder
 * Created on December 28, 2019, 21:44 PM
 */

#include <fstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "AppESBracketOrder.h"

IMPLEMENT_APP(AppESBracketOrder)

bool AppESBracketOrder::OnInit() {

  wxApp::OnInit();
  wxApp::SetAppDisplayName( "Interval Trader" );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c) 2019 One Unified Net Limited" );

  m_sStateFileName = "IntervalTrader.state";

  return true;

}

void AppESBracketOrder::SaveState( bool bSilent ) {
  if ( !bSilent ) std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( m_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  if ( !bSilent ) std::cout << "  done." << std::endl;
}

void AppESBracketOrder::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( m_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppESBracketOrder::OnClose( wxCloseEvent& event ) { // step 1

  SaveState();

  event.Skip();  // auto followed by Destroy();
}

int AppESBracketOrder::OnExit() { // step 2

  return wxApp::OnExit();
}
