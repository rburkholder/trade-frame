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

#include "HDF5DataManager.h"

#include <iostream>
#include <stdexcept>

//using namespace H5;

//const char CHDF5DataManager::m_H5FileName[] = "TradeFrame.%03d.hdf5";
const char CHDF5DataManager::m_H5FileName[] = "TradeFrame.hdf5";
H5::H5File CHDF5DataManager::m_H5File;
unsigned int CHDF5DataManager::m_RefCount = 0;

CHDF5DataManager HDF5DM;  // statically defined, so at least one instance is always present

CHDF5DataManager::CHDF5DataManager(void) {
  ++m_RefCount;
  if ( 1 == m_RefCount ) {
    //std::cout << "Opening DataManager" << std::endl;

    // Open and prepare HDF5 stuff (HDF5 precompiled utilities cannot handle split files
//    H5::FileAccPropList pl1;
    H5::FileAccPropList pl2;
//    pl2.setFamily( 2048000000, pl1 );
    try {
      try {
        // try for existing file
        m_H5File.openFile( m_H5FileName, H5F_ACC_RDWR, pl2 );
      }
      catch (...) {
        // try to create and open if it doesn't exist
        m_H5File.openFile( m_H5FileName, H5F_ACC_CREAT | H5F_ACC_RDWR, pl2 );
        H5::Group g1( GetH5File()->createGroup( "/bar" ) );
        g1.close();
        H5::Group g2( GetH5File()->createGroup( "/bar/86400" ) );
        g2.close();
        H5::Group g3( GetH5File()->createGroup( "/symbol" ) );
        g3.close();
      }

    }
    catch (...) {
      printf( "problems with HDF5 system\n" );
    }
    std::cout << "DataManager opened" << std::endl;
  }
  else {
    //throw std::exception( "Only one DataManager may be instantiated." );
    // multiple instantiations are allowed
  }
}

CHDF5DataManager::~CHDF5DataManager(void) {
  --m_RefCount;
  if ( 0 == m_RefCount ) {
    m_H5File.close();
  }
}

void CHDF5DataManager::Flush( void ) {
  GetH5File()->flush( H5F_SCOPE_GLOBAL );
}

bool CHDF5DataManager::GroupExists( const std::string &sGroup ) {
  bool bGroupExists = false;
  try {
    H5::Group g = GetH5File()->openGroup( sGroup );
    g.close();
    bGroupExists = true;
  }
  catch ( const H5::Exception /* &e */ ) {
    // group doesn't exist so just ignore
  }
  catch ( ... ) {
    std::cout << "CHDF5DataManager::GroupExists unknown error" << std::endl;
  }
  return bGroupExists;
}

void CHDF5DataManager::AddGroup( const std::string &sGroupPath ) { // needs to have terminating '/'
  //  /symbol, /symbol/G, /symbol/G/O, /symbol/G/O/GOOG
  std::string sSubPath;
  // ensure that appropriate group has been created in the file
  std::string::size_type ixSlash = sGroupPath.find( '/', 1 ); // ignore initial / character
  while ( std::string::npos != ixSlash ) {
    sSubPath = sGroupPath.substr( 0, ++ixSlash );  // use ixSlash as count here
    try {
      try {
        H5::Group g = GetH5File()->openGroup( sSubPath );
        g.close();
      }  // one of these when doesn't exist
      catch ( H5::Exception e ) {  // what is the specific exception here?
        //std::cout << "CDataManager::AddGroup H5::Exception for '" << sGroup << "', " << e.getDetailMsg() << std::endl;
        H5::Group g = GetH5File()->createGroup( sSubPath );
        g.close();
      }
    }
    catch (...) {
      throw std::runtime_error( "CDataManager::AddGroup has creation problems" );
    }
    ixSlash = sGroupPath.find( '/', ixSlash ); // use incremented ixSlash here as new start index
  }
}

void CHDF5DataManager::AddGroupForSymbol( const std::string &sSymbol ) {
  AddGroup( "/symbol/" + sSymbol );
}

herr_t CHDF5DataManager::PrintH5ErrorStackItem( int n, H5E_error_t *err_desc, void *client_data ) {
  // this is a call back from within an exception handler
  std::cout << "H5 Error Level " << n << ": " 
    << err_desc->file_name << "::" 
    << err_desc->func_name << "::"
    << err_desc->desc << std::endl;
  return 1;
}

void CHDF5DataManager::DailyBarPath(const std::string &sSymbol, std::string &sPath) {
  sPath = "/bar/86400/";
  sPath.append( sSymbol.substr( 0, 1 ) );
  sPath.append( "/" );
  sPath.append( sSymbol.substr( sSymbol.length() == 1 ? 0 : 1, 1 ) );
  sPath.append( "/" );
  sPath.append( sSymbol );
}

