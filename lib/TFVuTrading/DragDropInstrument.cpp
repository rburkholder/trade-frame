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

#include "DragDropInstrument.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
  
const char DragDropDataInstrument::szFormatIQFeedSymbolName[]           = "TradeFrameInstrumentIQFeedSymbolName";
//const char DragDropDataInstrument::szFormatInteractiveBrokersContract[] = "TradeFrameInstrumentInteractiveBrokersContract";
const char DragDropDataInstrument::szFormatInstrumentClass[]            = "TradeFrameInstrumentClass";
const char DragDropDataInstrument::szFormatInstrumentFunction[]         = "TradeFrameInstrumentFunction";

const wxDataFormat DragDropDataInstrument::DataFormatInstrumentIQFeedSymbolName( DragDropDataInstrument::szFormatIQFeedSymbolName );
//const wxDataFormat DragDropDataInstrument::DataFormatInstrumentInteractiveBrokersContract( DragDropDataInstrument::szFormatInteractiveBrokersContract );
const wxDataFormat DragDropDataInstrument::DataFormatInstrumentClass( DragDropDataInstrument::szFormatInstrumentClass );
const wxDataFormat DragDropDataInstrument::DataFormatInstrumentFunction( DragDropDataInstrument::szFormatInstrumentFunction );

DragDropDataInstrument::DragDropDataInstrument(const std::string& sIQFeedSymbolName )
: m_DataFormat( DataFormatInstrumentIQFeedSymbolName), m_sIQFeedSymbolName( sIQFeedSymbolName )
{
}

DragDropDataInstrument::DragDropDataInstrument(pInstrument_t pInstrument )
: m_DataFormat( DataFormatInstrumentClass ), m_pInstrument( pInstrument )
{
}

DragDropDataInstrument::DragDropDataInstrument( fOnInstrumentRetrieveInitiate_t&& fOnInstrumentRetrieveInitiate ) 
: m_DataFormat( DataFormatInstrumentFunction ), m_fOnInstrumentRetrieveInitiate( std::move( fOnInstrumentRetrieveInitiate ) )
{
}

DragDropDataInstrument::DragDropDataInstrument( )
: m_DataFormat( wxDF_PRIVATE )
{
}

DragDropDataInstrument::~DragDropDataInstrument( ) {
}

size_t DragDropDataInstrument::GetFormatCount(Direction dir) const {
  if ( Get == dir ) {
    return 1;
  }
  else {
    return 0;
  }
}

void DragDropDataInstrument::GetAllFormats(wxDataFormat *formats, Direction dir) const {
  // when messing with this, update GetFormatCount()
  if ( Get == dir ) {
//    formats[ 0 ] = DataFormatInstrumentIQFeedSymbolName;
//    formats[ 1 ] = DataFormatInstrumentClass;
//    formats[ 2 ] = DataFormatInstrumentFunction;
    formats[ 0 ] = m_DataFormat;
  }
  else 
    assert( 0 );
}

bool DragDropDataInstrument::GetDataHere(const wxDataFormat &format, void *buf) const {
  bool bCopied( false );
  if ( DataFormatInstrumentIQFeedSymbolName == format ) {
    //std::cout << "GetDataHere SymbolName" << std::endl;
    strcpy( reinterpret_cast<char*>( buf ), m_sIQFeedSymbolName.c_str() );
    bCopied = true;
  }
  if ( DataFormatInstrumentClass == format ) {
    //std::cout << "GetDataHere Inst Class" << std::endl;
    *reinterpret_cast<pInstrument_t*>( buf ) = m_pInstrument;
    bCopied = true;
  }
  if ( DataFormatInstrumentFunction == format ) {
    fOnInstrumentRetrieveInitiate_ptr p( const_cast<fOnInstrumentRetrieveInitiate_ptr>( &m_fOnInstrumentRetrieveInitiate));
    *reinterpret_cast<fOnInstrumentRetrieveInitiate_ptr*>( buf ) = p;
    bCopied = true;
  }
  return bCopied;
}

size_t DragDropDataInstrument::GetDataSize(const wxDataFormat &format) const {
  if ( DataFormatInstrumentIQFeedSymbolName == format ) {
    return m_sIQFeedSymbolName.size() + 1;
  }
  if ( DataFormatInstrumentClass == format ) {
    return sizeof( pInstrument_t );
  }
  if ( DataFormatInstrumentFunction == format ) {
    return sizeof(fOnInstrumentRetrieveInitiate_ptr);
  }
  return 0;
}

const std::string& DragDropDataInstrument::GetIQFeedSymbolName() const { 
//  if ( DataFormatInstrumentIQFeedSymbolName == m_DataFormat ) {
    return m_sIQFeedSymbolName; 
//  }
//  else {
//   return std::string;
//  }
}

DragDropDataInstrument::pInstrument_t DragDropDataInstrument::GetInstrument() {
//  if ( DataFormatInstrumentClass == m_DataFormat ) {
    return m_pInstrument;
//  }
}

DragDropDataInstrument::fOnInstrumentRetrieveInitiate_t& DragDropDataInstrument::GetInstrumentBuildInitiate() {
  return m_fOnInstrumentRetrieveInitiate;
}

wxDataFormat DragDropDataInstrument::GetPreferredFormat(Direction dir) const {
//  if ( Get == dir ) {
    return DataFormatInstrumentFunction;
//  }
//  else {
//    return wxDataFormat( wxDF_PRIVATE );
//  }
}

// TODO: need to perform && std::move on reference so that lambda doesn't go out of scope
// which is probably the problem for the segment fault earlier
bool DragDropDataInstrument::SetData(const wxDataFormat &format, size_t len, const void *buf) {
  bool bCopied( false );
  if ( DataFormatInstrumentIQFeedSymbolName == format ) {
    m_DataFormat = format;
    m_sIQFeedSymbolName = std::move( std::string( reinterpret_cast<const char*>( buf ) ) );
    bCopied = true;
  }
  if ( DataFormatInstrumentClass == format ) {
    assert( sizeof( pInstrument_t ) == len );
    m_DataFormat = format;
    m_pInstrument = *reinterpret_cast<const pInstrument_t*>( buf );
    bCopied = true;
  }
  if ( DataFormatInstrumentFunction == format ) {
    assert( sizeof( fOnInstrumentRetrieveInitiate_ptr ) == len );
    m_DataFormat = format;
    fOnInstrumentRetrieveInitiate_ptr p = *reinterpret_cast<const fOnInstrumentRetrieveInitiate_ptr*>( buf );
    m_fOnInstrumentRetrieveInitiate = *p;
    bCopied = true;
  }
  return bCopied;
}

bool DragDropDataInstrument::IsSupported(const wxDataFormat &format, Direction dir) const {
//  if ( DataFormatInstrumentIQFeedSymbolName == format ) return true;
//  if ( DataFormatInstrumentClass == format ) return true;
//  if ( DataFormatInstrumentFunction == format ) return true;
  if ( format == m_DataFormat ) return true;
  return false;
  }
  
} // namespace tf
} // namespace ou
