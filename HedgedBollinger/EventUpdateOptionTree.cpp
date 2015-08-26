/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "EventUpdateOptionTree.h"

wxDEFINE_EVENT(EVENT_UPDATE_OPTION_TREE, EventUpdateOptionTree);

EventUpdateOptionTree::EventUpdateOptionTree( wxEventType eventType, int winid, ou::tf::option::Strike& strike, bool bOn ) 
  : wxEvent( winid, eventType), m_strike( strike ), m_bOn( bOn )
{
}

EventUpdateOptionTree::~EventUpdateOptionTree(void) {
}
