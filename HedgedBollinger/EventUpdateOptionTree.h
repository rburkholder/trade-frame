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
// Started 2013/10/30

#pragma once

#include <TFOptions/Strike.h>

class EventUpdateOptionTree: public wxEvent {
public:

  EventUpdateOptionTree( wxEventType eventType, int winid, ou::tf::option::Strike&, bool );
  ~EventUpdateOptionTree(void);

  ou::tf::option::Strike& GetStrike( void ) { return m_strike; }
  bool GetWatch( void ) const { return m_bOn; }

  virtual wxEvent* Clone( void ) const { return new EventUpdateOptionTree( *this ); }

protected:
private:

  ou::tf::option::Strike& m_strike;
  bool m_bOn;  // true = watch on, false = watch off

};

wxDECLARE_EVENT(EVENT_UPDATE_OPTION_TREE, EventUpdateOptionTree);


