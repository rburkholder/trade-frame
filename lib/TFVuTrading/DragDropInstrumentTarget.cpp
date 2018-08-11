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
DragDropInstrumentTarget::DragDropInstrumentTarget( DragDropInstrument* data ): wxDropTarget( data ) {
}

DragDropInstrumentTarget::~DragDropInstrumentTarget( ) {
  wxDropTarget::SetDataObject( nullptr );
}

bool DragDropInstrumentTarget::GetData() {
  
  bool bResult = wxDropTarget::GetData();
  return bResult;
}
 
bool DragDropInstrumentTarget::OnDrop(wxCoord x, wxCoord y) {  // first step of two
  return true;
}
 
wxDragResult DragDropInstrumentTarget::OnData(wxCoord x, wxCoord y, wxDragResult defResult) { // second step of two
  std::cout << "DragDropInstrumentTarget::OnData" << std::endl;
  bool bResult = GetData();
  //wxDataObject obj = wxDropTarget::GetDataObject();
  //DragDropDataInstrument* dddi = dynamic_cast<DragDropDataInstrument*>( wxDropTarget::GetDataObject() );
  DragDropInstrument* ddi = dynamic_cast<DragDropInstrument*>( wxDropTarget::GetDataObject() );
  //DragDropDataInstrument* dddi = wxDropTarget::GetDataObject();
  if ( ddi->IsSupported( DragDropInstrument::DataFormatClass ) ) {
//    if ( nullptr != m_fOnInstrument ) {
//      DragDropDataInstrument::fOnInstrumentRetrieveInitiate_t 
 //       fOnInstrumentRetrieveInitiate( reinterpret_cast<DragDropDataInstrument*>( wxDropTarget::GetDataObject() )->GetInstrumentBuildInitiate() );
  //    fOnInstrumentRetrieveInitiate([this](pInstrument_t pInstrument){m_fOnInstrument( pInstrument );
   //   });
      //m_fOnInstrument( reinterpret_cast<DragDropDataInstrument*>( wxDropTarget::GetDataObject() )->GetInstrument() );
    //}
  }
  if ( ddi->IsSupported( DragDropInstrument::DataFormatFunction_Instrument ) ) {
      DragDropInstrument::fOnInstrumentRetrieveInitiate_t& fOnInstrumentRetrieveInitiate = ddi->GetInstrumentRetrieveInitiate();
      if ( nullptr != fOnInstrumentRetrieveInitiate ) {
        fOnInstrumentRetrieveInitiate([this](pInstrument_t pInstrument){
          if ( nullptr != m_fOnInstrumentRetrieveComplete ) {
            m_fOnInstrumentRetrieveComplete(pInstrument);
          }
          
        });
      }
  }
  if ( ddi->IsSupported( DragDropInstrument::DataFormatFunction_OptionUnderlying ) ) {
      DragDropInstrument::fOnOptionUnderlyingRetrieveInitiate_t fOnOptionUnderlyingRetrieveInitiate = std::move( ddi->GetOptionUnderlyingRetrieveInitiate() );
      if ( nullptr != fOnOptionUnderlyingRetrieveInitiate ) {
        fOnOptionUnderlyingRetrieveInitiate([this](pOptionInstrument_t pOptionInstrument, pUnderlyingInstrument_t pUnderlyingInstrument ){
          if ( nullptr != m_fOnOptionUnderlyingRetrieveComplete ) {
            m_fOnOptionUnderlyingRetrieveComplete( pOptionInstrument, pUnderlyingInstrument );
          }
        });
      }
  }
  if ( ddi->IsSupported( DragDropInstrument::DataFormatIQFeedSymbolName) ) {
  }
  return defResult;
}
 
wxDragResult DragDropInstrumentTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) {
  return defResult;
}
 
wxDragResult DragDropInstrumentTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult defResult) {
  return defResult;
}
 
void DragDropInstrumentTarget::OnLeave() {
}
 
} // namespace tf
} // namespace ou