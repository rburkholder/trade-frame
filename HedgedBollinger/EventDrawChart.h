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
// Started 2013/11/04

#pragma once

class EventDrawChart: public wxEvent {
public:
  EventDrawChart( wxEventType eventType, int winid, wxBitmap* pBitmap ); // bitmap goes away with event destruction
  ~EventDrawChart(void);

  wxBitmap* GetBitmap( void ) { return m_pBitmap; }

  virtual EventDrawChart* Clone( void ) const { return new EventDrawChart( *this ); }
protected:
private:
  wxBitmap* m_pBitmap;
};

wxDECLARE_EVENT(EVENT_DRAW_CHART, EventDrawChart);