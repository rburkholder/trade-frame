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
 * File:   DragDropInstrumentTarget.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on July 7, 2018, 8:52 PM
 */

// debugging only:
//#include <iostream>

#include "DragDropInstrumentTarget.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

DragDropInstrumentTarget::DragDropInstrumentTarget( DragDropDataInstrument *data ): wxDropTarget( data ) {
}

DragDropInstrumentTarget::~DragDropInstrumentTarget( ) {
}

bool DragDropInstrumentTarget::GetData() {
  return wxDropTarget::GetData();
}
 
wxDragResult DragDropInstrumentTarget::OnData(wxCoord x, wxCoord y, wxDragResult defResult) { // second step of two
  bool bResult = GetData();
  //std::cout << "DragDropInstrumentTarget OnData: " << bResult << "," << defResult << std::endl;
  if ( nullptr != m_fOnIQFeedSymbolName ) {
    m_fOnIQFeedSymbolName( reinterpret_cast<DragDropDataInstrument*>( wxDropTarget::GetDataObject() )->GetIQFeedSymbolName() );
  }
  return defResult;
}
 
wxDragResult DragDropInstrumentTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) {
  //std::cout << "DragDropInstrumentTarget OnDragOver" << std::endl;
  return defResult;
}
 
bool DragDropInstrumentTarget::OnDrop(wxCoord x, wxCoord y) {  // first step of two
  //std::cout << "DragDropInstrumentTarget OnDrop" << std::endl;
  return true;
}
 
wxDragResult DragDropInstrumentTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult defResult) {
  //std::cout << "DragDropInstrumentTarget OnEnter" << std::endl;
  return defResult;
}
 
void DragDropInstrumentTarget::OnLeave() {
   
}
 
} // namespace tf
} // namespace ou