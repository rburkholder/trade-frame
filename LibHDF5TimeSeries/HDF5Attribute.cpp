/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "HDF5Attribute.h"

const char szInstrumentType[] = "InstrumentType";
const char szUnderlying[] = "Underlying";
const char szStrike[] = "Strike";
const char szSide[] = "Side";
const char szYear[] = "Year";
const char szMonth[] = "Month";
const char szDay[] = "Day";

 CHDF5Attribute::CHDF5Attribute(void) 
 : m_pDataSet( NULL )
{
}

CHDF5Attribute::CHDF5Attribute( const std::string& sPath ) {
  OpenDataSet( sPath );
}

CHDF5Attribute::CHDF5Attribute( const std::string& sPath, InstrumentType::enumInstrumentTypes typeInst ) {
  OpenDataSet( sPath );
  SetInstrumentType( typeInst );
}

CHDF5Attribute::CHDF5Attribute( const std::string& sPath, const structOption& option ) {
  OpenDataSet( sPath );
  SetOptionAttributes( option );
}

CHDF5Attribute::CHDF5Attribute( const std::string& sPath, const structFuture& future ) {
  OpenDataSet( sPath );
  SetFutureAttributes( future );
}

CHDF5Attribute::~CHDF5Attribute(void) {
  CloseDataSet();
}

void CHDF5Attribute::OpenDataSet( const std::string& sPath ) {
  m_pDataSet = new H5::DataSet( dm.GetH5File()->openDataSet( sPath.c_str() ) );
}

void CHDF5Attribute::CloseDataSet ( void ) {
  if ( NULL != m_pDataSet ) {
    m_pDataSet->close();
    m_pDataSet = NULL;
  }
}

void CHDF5Attribute::SetInstrumentType( InstrumentType::enumInstrumentTypes typeInstrument ) {
  H5::DataSpace dspace;
  H5::Attribute attribute( m_pDataSet->createAttribute( szInstrumentType, H5::PredType::NATIVE_INT8, dspace ) );
  attribute.write( H5::PredType::NATIVE_INT8, &typeInstrument );
  attribute.close();
}

InstrumentType::enumInstrumentTypes CHDF5Attribute::GetInstrumentType( void ) {
  InstrumentType::enumInstrumentTypes typeInstrument;
  H5::Attribute attribute( m_pDataSet->openAttribute( szInstrumentType ) );
  attribute.read(H5::PredType::NATIVE_INT8, &typeInstrument );
  attribute.close();
  return typeInstrument;
}

void CHDF5Attribute::SetOptionAttributes( const structOption& option ) {

  SetInstrumentType( InstrumentType::Option );

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

void CHDF5Attribute::GetOptionAttributes( structOption* option ) {

  H5::Attribute attribStrike( m_pDataSet->openAttribute( szStrike ) );
  attribStrike.read(H5::PredType::NATIVE_DOUBLE, &option->dblStrike );
  attribStrike.close();

  H5::Attribute attribSide( m_pDataSet->openAttribute( szStrike ) );
  attribSide.read(H5::PredType::NATIVE_INT8, &option->eSide );
  attribSide.close();

  H5::Attribute attribYear( m_pDataSet->openAttribute( szStrike ) );
  attribYear.read(H5::PredType::NATIVE_UINT16, &option->nYear );
  attribYear.close();

  H5::Attribute attribSMonth( m_pDataSet->openAttribute( szStrike ) );
  attribSMonth.read(H5::PredType::NATIVE_UINT16, &option->nMonth );
  attribSMonth.close();

  H5::Attribute attribDay( m_pDataSet->openAttribute( szStrike ) );
  attribDay.read(H5::PredType::NATIVE_UINT16, &option->nDay );
  attribDay.close();

}

void CHDF5Attribute::SetFutureAttributes( const structFuture& future ) {

  SetInstrumentType( InstrumentType::Option );

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

void CHDF5Attribute::GetFutureAttributes( structFuture* future ) {

  H5::Attribute attribYear( m_pDataSet->openAttribute( szStrike ) );
  attribYear.read(H5::PredType::NATIVE_UINT16, &future->nYear );
  attribYear.close();

  H5::Attribute attribSMonth( m_pDataSet->openAttribute( szStrike ) );
  attribSMonth.read(H5::PredType::NATIVE_UINT16, &future->nMonth );
  attribSMonth.close();

  H5::Attribute attribDay( m_pDataSet->openAttribute( szStrike ) );
  attribDay.read(H5::PredType::NATIVE_UINT16, &future->nDay );
  attribDay.close();

}
