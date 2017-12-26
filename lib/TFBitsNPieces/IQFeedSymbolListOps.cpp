/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "IQFeedSymbolListOps.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

IQFeedSymbolListOps::IQFeedSymbolListOps( ou::tf::iqfeed::InMemoryMktSymbolList& immsl )
  : m_listIQFeedSymbols( immsl ), m_fenceWorker( 0 ) {
}

IQFeedSymbolListOps::~IQFeedSymbolListOps(void) {
	m_worker.Join();  // wait for processing to complete
}

bool IQFeedSymbolListOps::Exists( const std::string& sName ) {
  bool bFound( false );
  try {
    const trd_t& trd = m_listIQFeedSymbols.GetTrd( sName );
    bFound = true;
  }
  catch ( std::runtime_error& e ) {
  }
  return bFound;
}

void IQFeedSymbolListOps::ObtainNewIQFeedSymbolListRemote( void ) {
  if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
    m_worker.Run( MakeDelegate( this, &IQFeedSymbolListOps::WorkerObtainNewIQFeedSymbolListRemote ) );
  }
  else {
    m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
		StatusBusy();
  }
}

void IQFeedSymbolListOps::WorkerObtainNewIQFeedSymbolListRemote( void ) {
	Status( "Downloading Text File ... " );
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::Download, true, iqfeed::detail::sFileNameMarketSymbolsText ); 
	Status( "Saving Binary File ... " );
  m_listIQFeedSymbols.SaveToFile( iqfeed::detail::sFileNameMarketSymbolsBinary );
	StatusDone();
	Done( ccDone );
  m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::ObtainNewIQFeedSymbolListLocal( void ) {
  if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
    m_worker.Run( MakeDelegate( this, &IQFeedSymbolListOps::WorkerObtainNewIQFeedSymbolListLocal ) );
  }
  else {
    m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
		StatusBusy();
  }
}

void IQFeedSymbolListOps::WorkerObtainNewIQFeedSymbolListLocal( void ) {
	Status( "Loading From Text File ... " );
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false, iqfeed::detail::sFileNameMarketSymbolsText ); 
	Status( "Saving Binary File ... " );
  m_listIQFeedSymbols.SaveToFile( iqfeed::detail::sFileNameMarketSymbolsBinary );
	StatusDone();
	Done( ccDone );
  m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::LoadIQFeedSymbolList( void ) {
  if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
    m_worker.Run( MakeDelegate( this, &IQFeedSymbolListOps::WorkerLoadIQFeedSymbolList ) );
  }
  else {
    m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
		StatusBusy();
  }
}

void IQFeedSymbolListOps::WorkerLoadIQFeedSymbolList( void ) {
	Status( "Loading From Binary File ..." );
  m_listIQFeedSymbols.LoadFromFile( iqfeed::detail::sFileNameMarketSymbolsBinary );
	StatusDone();
	Done( ccDone );
  m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::SaveSymbolSubset( const std::string& sFileName, const ou::tf::iqfeed::InMemoryMktSymbolList& subset ) {
	if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
	//  ou::tf::iqfeed::InMemoryMktSymbolList listIQFeedSymbols;
		Status( "Saving subset to " + sFileName + " ..." );
	//  listIQFeedSymbols.HandleParsedStructure( m_listIQFeedSymbols.GetTrd( m_sNameUnderlying ) );
	//  m_listIQFeedSymbols.SelectOptionsByUnderlying( m_sNameOptionUnderlying, listIQFeedSymbols );
		subset.SaveToFile( sFileName );  // __.ser
		StatusDone();
		Done( ccSaved );
	}
	else {
		StatusBusy();
	}
	m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::LoadSymbolSubset( const std::string& sFileName ) {
	if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
		Status( "Loading From " + sFileName + " ..." );
		m_listIQFeedSymbols.LoadFromFile( sFileName );  // __.ser
		StatusDone();
		Done( ccDone );
	}
	else {
		StatusBusy();
	}
	m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::ClearIQFeedSymbolList( void ) {
	if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
		m_listIQFeedSymbols.Clear();
		Status( " Symbol List Cleared." );
		Done( ccCleared );
	}
	else {
		StatusBusy();
	}
	m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::StatusBusy() {
	Status( "IQFeedSymbolListOps is busy" );
}

void IQFeedSymbolListOps::StatusDone() {
	Status( " ... done." );
}

} // namespace tf
} // namespace ou
