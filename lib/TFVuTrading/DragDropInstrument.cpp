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
  
const char DragDropInstrument::szFormatIQFeedSymbolName[]          = "TradeFrameIQFeedSymbolName";
const char DragDropInstrument::szFormatClass[]                     = "TradeFrameClass";
const char DragDropInstrument::szFormatFunction_Instrument[]       = "TradeFrameFunction_Instrument";
const char DragDropInstrument::szFormatFunction_OptionUnderlying[] = "TradeFrameFunction_OptionUnderlying";

const wxDataFormat DragDropInstrument::DataFormatIQFeedSymbolName( DragDropInstrument::szFormatIQFeedSymbolName );
const wxDataFormat DragDropInstrument::DataFormatClass( DragDropInstrument::szFormatClass );
const wxDataFormat DragDropInstrument::DataFormatFunction_Instrument( DragDropInstrument::szFormatFunction_Instrument );
const wxDataFormat DragDropInstrument::DataFormatFunction_OptionUnderlying( DragDropInstrument::szFormatFunction_OptionUnderlying );

DragDropInstrument::DragDropInstrument(const std::string& sIQFeedSymbolName )
: m_DataFormat( DataFormatIQFeedSymbolName), m_sIQFeedSymbolName( sIQFeedSymbolName )
{
  //std::cout << "DragDropInstrument::DragDropInstrument(const std::string& sIQFeedSymbolName )" << std::endl;
}

DragDropInstrument::DragDropInstrument(pInstrument_t pInstrument )
: m_DataFormat( DataFormatClass ), m_pInstrument( pInstrument )

{
  //std::cout << "DragDropInstrument::DragDropInstrument(pInstrument_t pInstrument )" << std::endl;
}

DragDropInstrument::DragDropInstrument( fOnInstrumentRetrieveInitiate_t&& fOnInstrumentRetrieveInitiate )
: m_DataFormat( DataFormatFunction_Instrument ), m_fOnInstrumentRetrieveInitiate( std::move( fOnInstrumentRetrieveInitiate ) )
{
  //std::cout << "DragDropInstrument::DragDropInstrument( fOnInstrumentRetrieveInitiate_t&& fOnInstrumentRetrieveInitiate )" << std::endl;
}

DragDropInstrument::DragDropInstrument( fOnOptionUnderlyingRetrieveInitiate_t&& fOnOptionUnderlyingRetrieveInitiate )
: m_DataFormat( DataFormatFunction_OptionUnderlying ), m_fOnOptionUnderlyingRetrieveInitiate( std::move( fOnOptionUnderlyingRetrieveInitiate ) )
{
  //std::cout << "DragDropInstrument::DragDropInstrument( fOnOptionUnderlyingRetrieveInitiate_t&& fOnOptionUnderlyingRetrieveInitiate )" << std::endl;
}

DragDropInstrument::DragDropInstrument( )
: m_DataFormat( wxDF_PRIVATE )
{
  //std::cout << "DragDropInstrument::DragDropInstrument( )" << std::endl;
}

DragDropInstrument::~DragDropInstrument( ) {
  //std::cout << "DragDropInstrument::~DragDropInstrument( )" << std::endl;
}

size_t DragDropInstrument::GetFormatCount(Direction dir) const {
  if ( Get == dir ) {
    return 1;
  }
  else {
    return 0;
  }
}

void DragDropInstrument::GetAllFormats(wxDataFormat *formats, Direction dir) const {
  //std::cout << "DragDropInstrument::GetAllFormats: " << m_DataFormat.GetId() << std::endl;
  // when messing with this, update GetFormatCount()
  if ( Get == dir ) {
//    formats[ 0 ] = DataFormatIQFeedSymbolName;
//    formats[ 1 ] = DataFormatClass;
//    formats[ 2 ] = DataFormatFunction_Instrument;
    formats[ 0 ] = m_DataFormat;
  }
  else 
    assert( 0 );
}

bool DragDropInstrument::GetDataHere(const wxDataFormat &format, void *buf) const {
  //std::cout << "DragDropInstrument::GetDataHere: " << format.GetId() << std::endl;
  bool bCopied( false );
  if ( DataFormatIQFeedSymbolName == format ) {
    //std::cout << "GetDataHere SymbolName" << std::endl;
    strcpy( reinterpret_cast<char*>( buf ), m_sIQFeedSymbolName.c_str() );
    bCopied = true;
  }
  if ( DataFormatClass == format ) {
    //std::cout << "GetDataHere Inst Class" << std::endl;
    *reinterpret_cast<pInstrument_t*>( buf ) = m_pInstrument;
    bCopied = true;
  }
  if ( DataFormatFunction_Instrument == format ) {
    fOnInstrumentRetrieveInitiate_ptr p( const_cast<fOnInstrumentRetrieveInitiate_ptr>( &m_fOnInstrumentRetrieveInitiate) );
    *reinterpret_cast<fOnInstrumentRetrieveInitiate_ptr*>( buf ) = p;
    bCopied = true;
  }
  if ( DataFormatFunction_OptionUnderlying == format ) {
    fOnOptionUnderlyingRetrieveInitiate_ptr p( const_cast<fOnOptionUnderlyingRetrieveInitiate_ptr>( &m_fOnOptionUnderlyingRetrieveInitiate) );
    *reinterpret_cast<fOnOptionUnderlyingRetrieveInitiate_ptr*>( buf ) = p;
    bCopied = true;
  }
  return bCopied;
}

size_t DragDropInstrument::GetDataSize(const wxDataFormat &format) const {
  if ( DataFormatIQFeedSymbolName == format ) {
    return m_sIQFeedSymbolName.size() + 1;
  }
  if ( DataFormatClass == format ) {
    return sizeof( pInstrument_t );
  }
  if ( DataFormatFunction_Instrument == format ) {
    return sizeof(fOnInstrumentRetrieveInitiate_ptr);
  }
  if ( DataFormatFunction_OptionUnderlying == format ) {
    return sizeof(fOnOptionUnderlyingRetrieveInitiate_ptr);
  }
  return 0;
}

const std::string& DragDropInstrument::GetIQFeedSymbolName() const { 
//  if ( DataFormatInstrumentIQFeedSymbolName == m_DataFormat ) {
    return m_sIQFeedSymbolName; 
//  }
//  else {
//   return std::string;
//  }
}

DragDropInstrument::pInstrument_t DragDropInstrument::GetInstrument() {
//  if ( DataFormatInstrumentClass == m_DataFormat ) {
    return m_pInstrument;
//  }
}

DragDropInstrument::fOnInstrumentRetrieveInitiate_t& DragDropInstrument::GetInstrumentRetrieveInitiate() {
  return m_fOnInstrumentRetrieveInitiate;
}

DragDropInstrument::fOnOptionUnderlyingRetrieveInitiate_t& DragDropInstrument::GetOptionUnderlyingRetrieveInitiate() {
  return m_fOnOptionUnderlyingRetrieveInitiate;
}

wxDataFormat DragDropInstrument::GetPreferredFormat(Direction dir) const {
  //std::cout << "DragDropInstrument::GetPreferredFormat" << std::endl;
//  if ( Get == dir ) {
    //return DataFormatFunction_Instrument;
  return m_DataFormat;
//  }
//  else {
//    return wxDataFormat( wxDF_PRIVATE );
//  }
}

// TODO: need to perform && std::move on reference so that lambda doesn't go out of scope
// which is probably the problem for the segment fault earlier
bool DragDropInstrument::SetData(const wxDataFormat &format, size_t len, const void *buf) {
  //std::cout << "DragDropInstrument::SetData: " << format.GetId() << std::endl;
  bool bCopied( false );
  if ( DataFormatIQFeedSymbolName == format ) {
    m_DataFormat = format;
    m_sIQFeedSymbolName = std::move( std::string( reinterpret_cast<const char*>( buf ) ) );
    bCopied = true;
  }
  if ( DataFormatClass == format ) {
    assert( sizeof( pInstrument_t ) == len );
    m_DataFormat = format;
    m_pInstrument = *reinterpret_cast<const pInstrument_t*>( buf );
    bCopied = true;
  }
  if ( DataFormatFunction_Instrument == format ) {
    assert( sizeof( fOnInstrumentRetrieveInitiate_ptr ) == len );
    m_DataFormat = format;
    fOnInstrumentRetrieveInitiate_ptr p = *reinterpret_cast<const fOnInstrumentRetrieveInitiate_ptr*>( buf );
    m_fOnInstrumentRetrieveInitiate = *p;
    bCopied = true;
  }
  if ( DataFormatFunction_OptionUnderlying == format ) {
    assert( sizeof( fOnOptionUnderlyingRetrieveInitiate_ptr ) == len );
    m_DataFormat = format;
    fOnOptionUnderlyingRetrieveInitiate_ptr p = *reinterpret_cast<const fOnOptionUnderlyingRetrieveInitiate_ptr*>( buf );
    m_fOnOptionUnderlyingRetrieveInitiate = std::move( *p );
    bCopied = true;
  }
  return bCopied;
}

bool DragDropInstrument::IsSupported(const wxDataFormat &format, Direction dir) const {
  //std::cout << "DragDropInstrument::IsSupported: " << format.GetId() << "," << m_DataFormat.GetId() << std::endl;
//  if ( DataFormatInstrumentIQFeedSymbolName == format ) return true;
//  if ( DataFormatInstrumentClass == format ) return true;
//  if ( DataFormatInstrumentFunction == format ) return true;
  if ( format == m_DataFormat ) return true;
  return false;
  }
  
} // namespace tf
} // namespace ou

/*
DragDropInstrument::DragDropInstrument( fOnOptionUnderlyingRetrieveInitiate_t&& fOnOptionUnderlyingRetrieveInitiate )
DragDropInstrument::GetAllFormats: TradeFrameFunction_OptionUnderlying
DragDropInstrument::GetPreferredFormat
DragDropInstrument::GetDataHere: TradeFrameFunction_OptionUnderlying
DragDropInstrumentTarget::OnData
DragDropInstrument::GetPreferredFormat
DragDropInstrument::SetData: TradeFrameFunction_OptionUnderlying
DragDropInstrument::IsSupported: TradeFrameClass,TradeFrameFunction_OptionUnderlying
DragDropInstrument::IsSupported: TradeFrameFunction_Instrument,TradeFrameFunction_OptionUnderlying
DragDropInstrument::IsSupported: TradeFrameFunction_OptionUnderlying,TradeFrameFunction_OptionUnderlying
DragDropInstrument::IsSupported: TradeFrameIQFeedSymbolName,TradeFrameFunction_OptionUnderlying
DragDropInstrument::~DragDropInstrument( )
 */