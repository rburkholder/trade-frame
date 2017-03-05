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

#include <boost/shared_ptr.hpp>

#include <wx/bitmap.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class EventDrawChart: public wxEvent {
public:
  
  typedef boost::shared_ptr<wxBitmap> pwxBitmap_t;
  
  EventDrawChart( wxEventType eventType, int winid, pwxBitmap_t pBitmap ); // bitmap destroyed in event consumer
  ~EventDrawChart(void);

  pwxBitmap_t GetBitmap( void ) { return m_pBitmap; }

  virtual EventDrawChart* Clone( void ) const { return new EventDrawChart( *this ); }
protected:
private:
  pwxBitmap_t m_pBitmap;
};

} // namespace tf
} // namespace ou

wxDECLARE_EVENT(EVENT_DRAW_CHART, ou::tf::EventDrawChart);