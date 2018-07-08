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
 * File:   DragDropInstrument.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on July 6, 2018, 11:25 AM
 */

/*
 * May need an array of types at some point:
 *   iqfeed symbol name
 *   interactive brokers contract id
 *   native instrument class
 */


#include "DragDropInstrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  
const char DragDropDataInstrument::szFormatIQFeedSymbolName[]           = "TradeFrameInstrumentIQFeedSymbolName";
const char DragDropDataInstrument::szFormatInteractiveBrokersContract[] = "TradeFrameInstrumentInteractiveBrokersContract";
const char DragDropDataInstrument::szFormatInstrumentClass[]            = "TradeFrameInstrumentClass";

const wxDataFormat DragDropDataInstrument::DataFormatInstrumentIQFeedSymbolName( DragDropDataInstrument::szFormatIQFeedSymbolName );
const wxDataFormat DragDropDataInstrument::DataFormatInstrumentInteractiveBrokersContract( DragDropDataInstrument::szFormatInteractiveBrokersContract );
const wxDataFormat DragDropDataInstrument::DataFormatInstrumentClass( DragDropDataInstrument::szFormatInstrumentClass );

DragDropDataInstrument::DragDropDataInstrument(const std::string& sIQFeedSymbolName )
:  m_sIQFeedSymbolName( sIQFeedSymbolName )
{
}

DragDropDataInstrument::DragDropDataInstrument( )
:  DragDropDataInstrument( "" )
{
}

DragDropDataInstrument::~DragDropDataInstrument( ) {
}

void DragDropDataInstrument::GetAllFormats(wxDataFormat *formats, Direction dir) const {
  if ( Get == dir ) {
    formats[ 0 ] = DataFormatInstrumentIQFeedSymbolName;
  }
  else 
    assert( 0 );
}

bool DragDropDataInstrument::GetDataHere(const wxDataFormat &format, void *buf) const {
  bool bCopied( false );
  if ( DataFormatInstrumentIQFeedSymbolName == format ) {
    strcpy( reinterpret_cast<char*>( buf ), m_sIQFeedSymbolName.c_str() );
    bCopied = true;
  }
  return bCopied;
}

size_t DragDropDataInstrument::GetDataSize(const wxDataFormat &format) const {
  return m_sIQFeedSymbolName.size() + 1;
}

size_t DragDropDataInstrument::GetFormatCount(Direction dir) const {
  if ( Get == dir ) {
    return 1;
  }
  else {
    return 0;
  }
}

wxDataFormat DragDropDataInstrument::GetPreferredFormat(Direction dir) const {
  if ( Get == dir ) {
    return DataFormatInstrumentIQFeedSymbolName;
  }
  else {
    return wxDataFormat( wxDF_INVALID );
  }
}

bool DragDropDataInstrument::SetData(const wxDataFormat &format, size_t len, const void *buf) {
  bool bCopied( false );
  if ( DataFormatInstrumentIQFeedSymbolName == format ) {
    m_sIQFeedSymbolName = std::move( std::string( reinterpret_cast<const char*>( buf ) ) );
    bCopied = true;
  }
  return bCopied;
}

bool DragDropDataInstrument::IsSupported(const wxDataFormat &format, Direction dir) const {
  return DataFormatInstrumentIQFeedSymbolName == format;
  }
  
} // namespace tf
} // namespace ou
