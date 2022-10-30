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
 * File:    Process.cpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: October 20, 2022 21:07:40
 */

#include "Process.hpp"

Process::Process( const config::Choices& choices )
: m_choices( choices )
{
  m_piqfeed = ou::tf::iqfeed::IQFeedProvider::Factory();

  m_piqfeed->OnConnected.Add( MakeDelegate( this, &Process::HandleConnected ) );
  m_piqfeed->Connect();
}

// need control c handler to terminate, as this is an ongoing process

void Process::HandleConnected( int ) {
  // convert/lookup name
  // start watch on l1, l2
  // write on regular intervals
}

void Process::Abort() {
}

void Process::Wait() {
}