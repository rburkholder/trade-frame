/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

//#include "StdAfx.h"

#include "HDF5Attribute.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

const char szInstrumentType[] = "InstrumentType";
const char szProviderType[] = "ProviderType";
const char szUnderlying[] = "Underlying";
const char szStrike[] = "Strike";
const char szSide[] = "Side";
const char szYear[] = "Year";
const char szMonth[] = "Month";
const char szDay[] = "Day";
const char szMultiplier[] = "Multiplier";
const char szSignificantDigits[] = "SignificantDigits";
const char szSignature[] = "Signature";

 HDF5Attributes::HDF5Attributes( HDF5DataManager& dm )
 : m_pDataSet( NULL ), m_dm( dm )
{
}

HDF5Attributes::HDF5Attributes( HDF5DataManager& dm, const std::string& sPath ): m_dm( dm ) {
  OpenDataSet( sPath );
}

HDF5Attributes::HDF5Attributes( HDF5DataManager& dm, const std::string& sPath, InstrumentType::EInstrumentType typeInst ): m_dm( dm ) {
  OpenDataSet( sPath );
  SetInstrumentType( typeInst );
}

HDF5Attributes::HDF5Attributes( HDF5DataManager& dm, const std::string& sPath, const structOption& option ): m_dm( dm ) {
  OpenDataSet( sPath );
  SetOptionAttributes( option );
}

HDF5Attributes::HDF5Attributes( HDF5DataManager& dm, const std::string& sPath, const structFuture& future ): m_dm( dm ) {
  OpenDataSet( sPath );
  SetFutureAttributes( future );
}

HDF5Attributes::~HDF5Attributes(void) {
  CloseDataSet();
}

void HDF5Attributes::OpenDataSet( const std::string& sPath ) {
  m_pDataSet = new H5::DataSet( m_dm.GetH5File()->openDataSet( sPath.c_str() ) );
}

void HDF5Attributes::CloseDataSet ( void ) {
  if ( NULL != m_pDataSet ) {
    m_pDataSet->close();
    m_pDataSet = NULL;
  }
}

void HDF5Attributes::SetInstrumentType( InstrumentType::EInstrumentType typeInstrument ) {
  H5::DataSpace dspace;
  H5::Attribute attribute( m_pDataSet->createAttribute( szInstrumentType, H5::PredType::NATIVE_INT8, dspace ) );
  attribute.write( H5::PredType::NATIVE_INT8, &typeInstrument );
  attribute.close();
}

InstrumentType::EInstrumentType HDF5Attributes::GetInstrumentType( void ) {
  InstrumentType::EInstrumentType typeInstrument;
  H5::Attribute attribute( m_pDataSet->openAttribute( szInstrumentType ) );
  attribute.read(H5::PredType::NATIVE_INT8, &typeInstrument );
  attribute.close();
  return typeInstrument;
}

void HDF5Attributes::SetSignature( boost::uint64_t sig ) {
  H5::DataSpace dspace;
  H5::Attribute attribute( m_pDataSet->createAttribute( szSignature, H5::PredType::NATIVE_UINT64, dspace ) );
  attribute.write( H5::PredType::NATIVE_UINT64, &sig );
  attribute.close();
}

boost::uint64_t HDF5Attributes::GetSignature() {
  boost::uint64_t sig;
  H5::Attribute attribute( m_pDataSet->openAttribute( szSignature ) );
  attribute.read(H5::PredType::NATIVE_UINT64, &sig );
  attribute.close();
  return sig;
}

void HDF5Attributes::SetProviderType( keytypes::eidProvider_t id ) {
  H5::DataSpace dspace;
  H5::Attribute attribute( m_pDataSet->createAttribute( szProviderType, H5::PredType::NATIVE_UINT16, dspace ) );
  attribute.write( H5::PredType::NATIVE_UINT16, &id );
  attribute.close();
}

keytypes::eidProvider_t HDF5Attributes::GetProviderType( void ) {
  keytypes::eidProvider_t id;
  H5::Attribute attribute( m_pDataSet->openAttribute( szProviderType ) );
  attribute.read(H5::PredType::NATIVE_UINT16, &id );
  attribute.close();
  return id;
}

void HDF5Attributes::SetSignificantDigits( unsigned char sd ) {
  H5::DataSpace dspace;
  H5::Attribute attribute( m_pDataSet->createAttribute( szSignificantDigits, H5::PredType::NATIVE_UINT8, dspace ) );
  attribute.write( H5::PredType::NATIVE_UINT8, &sd );
  attribute.close();
}

unsigned char HDF5Attributes::GetSignificantDigits( void ) {
  unsigned char sd;
  H5::Attribute attribute( m_pDataSet->openAttribute( szSignificantDigits ) );
  attribute.read(H5::PredType::NATIVE_UINT8, &sd );
  attribute.close();
  return sd;
}

void HDF5Attributes::SetMultiplier( unsigned short mult ) {
  H5::DataSpace dspace;
  H5::Attribute attribute( m_pDataSet->createAttribute( szMultiplier, H5::PredType::NATIVE_UINT16, dspace ) );
  attribute.write( H5::PredType::NATIVE_UINT16, &mult );
  attribute.close();
}

unsigned short HDF5Attributes::GetMultiplier( void ) {
  unsigned short mult;
  H5::Attribute attribute( m_pDataSet->openAttribute( szMultiplier ) );
  attribute.read(H5::PredType::NATIVE_UINT16, &mult );
  attribute.close();
  return mult;
}

void HDF5Attributes::SetOptionAttributes( const structOption& option ) {

  //SetInstrumentType( InstrumentType::Option );

  H5::DataSpace dspace;

  H5::Attribute attribStrike( m_pDataSet->createAttribute( szStrike, H5::PredType::NATIVE_DOUBLE, dspace ) );
  attribStrike.write( H5::PredType::NATIVE_DOUBLE, &option.dblStrike );
  attribStrike.close();

  H5::Attribute attribSide( m_pDataSet->createAttribute( szSide, H5::PredType::NATIVE_INT8, dspace ) );
  attribSide.write( H5::PredType::NATIVE_INT8, &option.eSide );
  attribSide.close();

  H5::Attribute attribYear( m_pDataSet->createAttribute( szYear, H5::PredType::NATIVE_UINT16, dspace ) );
  attribYear.write( H5::PredType::NATIVE_UINT16, &option.nYear );
  attribYear.close();

  H5::Attribute attribSMonth( m_pDataSet->createAttribute( szMonth, H5::PredType::NATIVE_UINT16, dspace ) );
  attribSMonth.write( H5::PredType::NATIVE_UINT16, &option.nMonth );
  attribSMonth.close();

  H5::Attribute attribDay( m_pDataSet->createAttribute( szDay, H5::PredType::NATIVE_UINT16, dspace ) );
  attribDay.write( H5::PredType::NATIVE_UINT16, &option.nDay );
  attribDay.close();
}

void HDF5Attributes::GetOptionAttributes( structOption* option ) {

  H5::Attribute attribStrike( m_pDataSet->openAttribute( szStrike ) );
  attribStrike.read(H5::PredType::NATIVE_DOUBLE, &option->dblStrike );
  attribStrike.close();

  H5::Attribute attribSide( m_pDataSet->openAttribute( szSide ) );
  attribSide.read(H5::PredType::NATIVE_INT8, &option->eSide );
  attribSide.close();

  H5::Attribute attribYear( m_pDataSet->openAttribute( szYear ) );
  attribYear.read(H5::PredType::NATIVE_UINT16, &option->nYear );
  attribYear.close();

  H5::Attribute attribSMonth( m_pDataSet->openAttribute( szMonth ) );
  attribSMonth.read(H5::PredType::NATIVE_UINT16, &option->nMonth );
  attribSMonth.close();

  H5::Attribute attribDay( m_pDataSet->openAttribute( szDay ) );
  attribDay.read(H5::PredType::NATIVE_UINT16, &option->nDay );
  attribDay.close();

}

void HDF5Attributes::SetFutureAttributes( const structFuture& future ) {

  //SetInstrumentType( InstrumentType::Future );

  H5::DataSpace dspace;

  H5::Attribute attribYear( m_pDataSet->createAttribute( szYear, H5::PredType::NATIVE_UINT16, dspace ) );
  attribYear.write( H5::PredType::NATIVE_UINT16, &future.nYear );
  attribYear.close();

  H5::Attribute attribSMonth( m_pDataSet->createAttribute( szMonth, H5::PredType::NATIVE_UINT16, dspace ) );
  attribSMonth.write( H5::PredType::NATIVE_UINT16, &future.nMonth );
  attribSMonth.close();

  H5::Attribute attribDay( m_pDataSet->createAttribute( szDay, H5::PredType::NATIVE_UINT16, dspace ) );
  attribDay.write( H5::PredType::NATIVE_UINT16, &future.nDay );
  attribDay.close();
}

void HDF5Attributes::GetFutureAttributes( structFuture* future ) {

  H5::Attribute attribYear( m_pDataSet->openAttribute( szYear ) );
  attribYear.read(H5::PredType::NATIVE_UINT16, &future->nYear );
  attribYear.close();

  H5::Attribute attribSMonth( m_pDataSet->openAttribute( szMonth ) );
  attribSMonth.read(H5::PredType::NATIVE_UINT16, &future->nMonth );
  attribSMonth.close();

  H5::Attribute attribDay( m_pDataSet->openAttribute( szDay ) );
  attribDay.read(H5::PredType::NATIVE_UINT16, &future->nDay );
  attribDay.close();

}

} // namespace tf
} // namespace ou
