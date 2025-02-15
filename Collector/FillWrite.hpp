/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    FillWrite.hpp
 * Author:  raymond@burkholder.net
 * Project: Collector
 * Created: January 5, 2024 11:56:31
 */

// manage the continuous fill/write process of a time series

#pragma once

#include <array>
#include <atomic>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>

namespace ou { // namespace one unified net
namespace tf { // namespace tradeframe

using fFillWrite_Hdf5Attribute_t = std::function<void(ou::tf::HDF5Attributes&)>; // one time callback on first write

template<typename T> // timeseries, eg, ou::tf::Trades, ou::tf::Quotes
class FillWrite {
public:
  FillWrite( const std::string& sDataPath, fFillWrite_Hdf5Attribute_t&& );
  ~FillWrite();
  void Append( const typename T::datum_t& );
  void Write(); // todo: use local timer
protected:
private:

  using rFillWrite_t = std::array<T,2>;
  rFillWrite_t m_rFillWrite; // one writes while one collects

  using ixFillWrite = std::atomic<typename rFillWrite_t::size_type>;
  ixFillWrite m_ixFilling;
  ixFillWrite m_ixWriting;

  std::string m_sDataPath;

  bool m_bHdf5AttributesSet;
  fFillWrite_Hdf5Attribute_t m_fFillWrite_Hdf5Attribute;

};

template<typename T>
FillWrite<T>::FillWrite( const std::string& sDataPath, fFillWrite_Hdf5Attribute_t&& f )
: m_bHdf5AttributesSet( false )
, m_fFillWrite_Hdf5Attribute( std::move( f ) )
, m_ixFilling( 0 )
, m_ixWriting( 1 )
, m_sDataPath( sDataPath )
{
  assert( m_fFillWrite_Hdf5Attribute );
}

template<typename T>
FillWrite<T>::~FillWrite() {}

template<typename T>
void FillWrite<T>::Append( const typename T::datum_t& t ) {
  m_rFillWrite[m_ixFilling].Append( t );
}

template<typename T>
void FillWrite<T>::Write() {

  assert( 0 == m_rFillWrite[m_ixWriting].Size() );

  typename rFillWrite_t::size_type ix {};
  ix = m_ixWriting.exchange(  2 ); // take the writing index
  ix = m_ixFilling.exchange( ix ); // and make it the new filling index
  ix = m_ixWriting.exchange( ix ); // and write what was being filled
  assert( 2 == ix );

  if ( 0 != m_rFillWrite[ m_ixWriting ].Size() ) {
    ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );
    ou::tf::HDF5WriteTimeSeries<T> wts( dm, true, true, 5, 256 );
    wts.Write( m_sDataPath, &m_rFillWrite[ m_ixWriting ] );

    if ( !m_bHdf5AttributesSet ) {
      m_bHdf5AttributesSet = true;
      ou::tf::HDF5Attributes attrT( dm, m_sDataPath );
      attrT.SetSignature( T::datum_t::Signature() );
      m_fFillWrite_Hdf5Attribute( attrT ); // set typename T specific attributes
    }

    m_rFillWrite[ m_ixWriting ].Clear();
  }
}

} // namespace tf
} // namespace ou
