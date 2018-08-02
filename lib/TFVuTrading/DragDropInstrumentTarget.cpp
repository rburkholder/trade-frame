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

// need to allow DragDropInstrumentTarget delete 'data'
DragDropInstrumentTarget::DragDropInstrumentTarget( DragDropDataInstrument *data ): wxDropTarget( data ) {
}

DragDropInstrumentTarget::~DragDropInstrumentTarget( ) {
}

bool DragDropInstrumentTarget::GetData() {
  
  bool bResult = wxDropTarget::GetData();
  return bResult;
}
 
wxDragResult DragDropInstrumentTarget::OnData(wxCoord x, wxCoord y, wxDragResult defResult) { // second step of two
  bool bResult = GetData();
  //wxDataObject obj = wxDropTarget::GetDataObject();
  //DragDropDataInstrument* dddi = dynamic_cast<DragDropDataInstrument*>( wxDropTarget::GetDataObject() );
  DragDropDataInstrument* dddi = (DragDropDataInstrument*)wxDropTarget::GetDataObject();
  //DragDropDataInstrument* dddi = wxDropTarget::GetDataObject();
  if ( dddi->IsSupported( DragDropDataInstrument::DataFormatInstrumentClass ) ) {
//    if ( nullptr != m_fOnInstrument ) {
//      DragDropDataInstrument::fOnInstrumentRetrieveInitiate_t 
 //       fOnInstrumentRetrieveInitiate( reinterpret_cast<DragDropDataInstrument*>( wxDropTarget::GetDataObject() )->GetInstrumentBuildInitiate() );
  //    fOnInstrumentRetrieveInitiate([this](pInstrument_t pInstrument){m_fOnInstrument( pInstrument );
   //   });
      //m_fOnInstrument( reinterpret_cast<DragDropDataInstrument*>( wxDropTarget::GetDataObject() )->GetInstrument() );
    //}
  }
  if ( dddi->IsSupported( DragDropDataInstrument::DataFormatInstrumentFunction ) ) {
      DragDropDataInstrument::fOnInstrumentRetrieveInitiate_t& fOnInstrumentRetrieveInitiate = dddi->GetInstrumentBuildInitiate();
      if ( nullptr != fOnInstrumentRetrieveInitiate ) {
        fOnInstrumentRetrieveInitiate([this](pInstrument_t pInstrument){
          m_fOnInstrument(pInstrument);
        });
      }
  }
  if ( dddi->IsSupported( DragDropDataInstrument::DataFormatInstrumentIQFeedSymbolName) ) {
  }
  return defResult;
}
 
wxDragResult DragDropInstrumentTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) {
  return defResult;
}
 
bool DragDropInstrumentTarget::OnDrop(wxCoord x, wxCoord y) {  // first step of two
  return true;
}
 
wxDragResult DragDropInstrumentTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult defResult) {
  return defResult;
}
 
void DragDropInstrumentTarget::OnLeave() {
}
 
} // namespace tf
} // namespace ou