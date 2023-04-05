/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   DragDropInstrumentTarget.h
 * Author: raymond@burkholder.net
 *
 * Created on July 7, 2018, 8:52 PM
 */

#ifndef DRAGDROPINSTRUMENTTARGET_H
#define DRAGDROPINSTRUMENTTARGET_H

#include <functional>

#include <wx/dnd.h>

#include "DragDropInstrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

/* wxDropTarget is defined in a class desiring to be the destination of an
 *   instrument, or OptionInstrument/UnderlyingInstrument pair
 * WxDropTarget has the appropriate 'retrieve complete' function assigned
 *   in order to receive the desired objects in a callback
 */

class DragDropInstrumentTarget: public wxDropTarget {
public:

  DragDropInstrumentTarget( DragDropInstrument *data = nullptr );
  virtual ~DragDropInstrumentTarget( );

  virtual bool GetData();
  virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult defResult);
  virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult);
  virtual bool OnDrop(wxCoord x, wxCoord y);
  virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult defResult);
  virtual void OnLeave();

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef pInstrument_t pOptionInstrument_t;
  typedef pInstrument_t pUnderlyingInstrument_t;

  typedef DragDropInstrument::fOnInstrumentRetrieveComplete_t fOnInstrumentRetrieveComplete_t;
  fOnInstrumentRetrieveComplete_t m_fOnInstrumentRetrieveComplete;  // results of instrument drag, may or may not have contract

  typedef DragDropInstrument::fOnOptionUnderlyingRetrieveComplete_t fOnOptionUnderlyingRetrieveComplete_t;
  fOnOptionUnderlyingRetrieveComplete_t m_fOnOptionUnderlyingRetrieveComplete;

protected:
private:

};

} // namespace tf
} // namespace ou

#endif /* DRAGDROPINSTRUMENTTARGET_H */

