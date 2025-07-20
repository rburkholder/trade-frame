/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    OptionManager.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 19, 2025 11:40:02
 */

// inspired by
//   Collector/Process.cpp
//   Collector/Collect.cpp
//   Collector/CollectATM.cpp

#include "OptionManager.hpp"

OptionManager::OptionManager(
  pIQFeed_t pIQFeed
)
: m_pIQFeed( pIQFeed )
{
  assert( m_pIQFeed->Connected() );
  m_fedrate.SetWatchOn( m_pIQFeed );
  m_pOptionEngine = std::make_unique<ou::tf::option::Engine>( m_fedrate );
}

OptionManager::~OptionManager() {

  m_pOptionEngine.reset();
  m_fedrate.SetWatchOff();
}

void OptionManager::SaveSeries( const std::string& sFile, const std::string& sPath ) {
  //m_fedrate.SaveSeries( sHdf5FileName );
}

