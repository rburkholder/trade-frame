#include "StdAfx.h"
#include "HDF5DataManager.h"

#include <iostream>
#include <stdexcept>

using namespace H5;

const char CHDF5DataManager::m_H5FileName[] = "TradeFrame.%03d.hdf5";
H5File CHDF5DataManager::m_H5File;
unsigned int CHDF5DataManager::m_RefCount = 0;

CHDF5DataManager HDF5DM;

CHDF5DataManager::CHDF5DataManager(void) {
  ++m_RefCount;
  if ( 1 == m_RefCount ) {
    //std::cout << "Opening DataManager" << std::endl;

    // Open and prepare HDF5 stuff
    H5::FileAccPropList pl1;
    H5::FileAccPropList pl2;
    pl2.setFamily( 2048000000, pl1 );
    try {
      try {
        // try for existing file
        m_H5File.openFile( m_H5FileName, H5F_ACC_RDWR, pl2 );
      }
      catch (...) {
        // try to create and open if it doesn't exist
        m_H5File.openFile( m_H5FileName, H5F_ACC_CREAT | H5F_ACC_RDWR, pl2 );
        Group g1( GetH5File()->createGroup( "/bar" ) );
        g1.close();
        Group g2( GetH5File()->createGroup( "/bar/86400" ) );
        g2.close();
        Group g3( GetH5File()->createGroup( "/symbol" ) );
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

void CHDF5DataManager::AddGroup( const std::string &sGroup ) {
  // caller can supply stepped groups to create group structure:
  //  /symbol, /symbol/G, /symbol/G/O, /symbol/G/O/GOOG
  try {
    try {
      Group g = GetH5File()->openGroup( sGroup );
      g.close();
    }  // one of these when doesn't exist
    catch ( H5::Exception e ) {
      //std::cout << "CDataManager::AddGroup H5::Exception for '" << sGroup << "', " << e.getDetailMsg() << std::endl;
      Group g = GetH5File()->createGroup( sGroup );
      g.close();
    }
  }
  catch (...) {
    throw std::runtime_error( "CDataManager::AddGroup has creation problems" );
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

