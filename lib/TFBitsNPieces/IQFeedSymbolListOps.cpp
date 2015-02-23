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
    std::cout << "IQFeedSymbolList::WorkerObtainNewIQFeedSymbolListRemote is busy" << std::endl;
  }
}

void IQFeedSymbolListOps::WorkerObtainNewIQFeedSymbolListRemote( void ) {
  std::cout << "Downloading Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::Download, true, iqfeed::detail::sFileNameMarketSymbolsText ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( iqfeed::detail::sFileNameMarketSymbolsBinary );
  std::cout << " ... done." << std::endl;
  m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::ObtainNewIQFeedSymbolListLocal( void ) {
  if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
    m_worker.Run( MakeDelegate( this, &IQFeedSymbolListOps::WorkerObtainNewIQFeedSymbolListLocal ) );
  }
  else {
    m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
    std::cout << "IQFeedSymbolList::WorkerObtainNewIQFeedSymbolListLocal is busy" << std::endl;
  }
}

void IQFeedSymbolListOps::WorkerObtainNewIQFeedSymbolListLocal( void ) {
  std::cout << "Loading From Text File ... " << std::endl;
  ou::tf::iqfeed::LoadMktSymbols( m_listIQFeedSymbols, ou::tf::iqfeed::MktSymbolLoadType::LoadTextFromDisk, false, iqfeed::detail::sFileNameMarketSymbolsText ); 
  std::cout << "Saving Binary File ... " << std::endl;
  m_listIQFeedSymbols.SaveToFile( iqfeed::detail::sFileNameMarketSymbolsBinary );
  std::cout << " ... done." << std::endl;
  m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::LoadIQFeedSymbolList( void ) {
  if ( 0 == m_fenceWorker.fetch_add( 1, boost::memory_order_acquire ) ) {
    m_worker.Run( MakeDelegate( this, &IQFeedSymbolListOps::WorkerLoadIQFeedSymbolList ) );
  }
  else {
    m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
    std::cout << "IQFeedSymbolList::WorkerLoadIQFeedSymbolList is busy" << std::endl;
  }
}

void IQFeedSymbolListOps::WorkerLoadIQFeedSymbolList( void ) {
  std::cout << "Loading From Binary File ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( iqfeed::detail::sFileNameMarketSymbolsBinary );
  std::cout << " ... done." << std::endl;
  m_fenceWorker.fetch_sub( 1, boost::memory_order_release );
}

void IQFeedSymbolListOps::SaveSymbolSubset( const std::string& sFileName, const ou::tf::iqfeed::InMemoryMktSymbolList& subset ) {
//  ou::tf::iqfeed::InMemoryMktSymbolList listIQFeedSymbols;
  std::cout << "Saving subset to " << sFileName << " ..." << std::endl;
//  listIQFeedSymbols.HandleParsedStructure( m_listIQFeedSymbols.GetTrd( m_sNameUnderlying ) );
//  m_listIQFeedSymbols.SelectOptionsByUnderlying( m_sNameOptionUnderlying, listIQFeedSymbols );
  subset.SaveToFile( sFileName );  // __.ser
  std::cout << " ... done." << std::endl;
}

void IQFeedSymbolListOps::LoadSymbolSubset( const std::string& sFileName ) {
  std::cout << "Loading From " << sFileName << " ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( sFileName );  // __.ser
  std::cout << " ... done." << std::endl;
}

void IQFeedSymbolListOps::ClearIQFeedSymbolList( void ) {
  m_listIQFeedSymbols.Clear();
  std::cout << " Symbol List Cleared." << std::endl;
}

} // namespace tf
} // namespace ou
