#include "StdAfx.h"
#include "DataManager.h"
#include <stdexcept>
#include <iostream>

using namespace H5;

// H5FD_FAMILY
/*

    herr_t H5Pset_fapl_family (hid_t fapl_id, hsize_t memb_size,
             hid_t member_properties) 
    herr_t H5Pget_fapl_family (hid_t fapl_id, hsize_t *memb_size,
             hid_t *member_properties) 
*/

const char CDataManager::m_H5FileName[] = "TradeFrame.%03d.hdf5";
H5File CDataManager::m_H5File;
DbEnv CDataManager::m_DbEnv( 0 );
unsigned int CDataManager::m_RefCount = 0;

CDataManager dm;

CDataManager::CDataManager(void) {
  ++m_RefCount;
  if ( 1 == m_RefCount ) {
    m_DbEnv.open( ".", DB_INIT_MPOOL | DB_CREATE | DB_THREAD | DB_INIT_CDB, 0 );
    H5::FileAccPropList pl1;
    H5::FileAccPropList pl2;
    pl2.setFamily( 2048000000, pl1 );
    try {
      try {
        // try for existing file
        m_H5File.openFile( m_H5FileName, H5F_ACC_RDWR, pl2 );
      }
      catch (...) {
        // try to create and open
        m_H5File.openFile( m_H5FileName, H5F_ACC_CREAT | H5F_ACC_RDWR, pl2 );
        //Group g1( dm.GetH5File()->createGroup( "/daily" ) );
        //g1.close();
        //Group g2( dm.GetH5File()->createGroup( "/daily/bar" ) );
        //g2.close();
        Group g3( dm.GetH5File()->createGroup( "/bar" ) );
        g3.close();
        Group g4( dm.GetH5File()->createGroup( "/bar/86400" ) );
        g4.close();
        Group g5( dm.GetH5File()->createGroup( "/symbol" ) );
        g5.close();
      }

    }
    catch (...) {
      printf( "problems with HDF5 system\n" );
    }
  }
  else {
    //throw std::exception( "Only one DataManager may be instantiated." );
    // multiple instantiations are allowed
  }
}

void CDataManager::AddGroupForSymbol( const std::string &sSymbol ) {
  try {
    Group g = dm.GetH5File()->openGroup( "/symbol/" + sSymbol );
    g.close();
  }  // one of these when doesn't exist
  catch ( H5::FileIException e ) {
    std::cout << "H5::FileIException " << e.getDetailMsg() << std::endl;
    Group g = dm.GetH5File()->createGroup( "/symbol/" + sSymbol );
    g.close();
  }
  catch ( H5::GroupIException e ) {
    std::cout << "H5::GroupIException " << e.getDetailMsg() << std::endl;
    Group g = dm.GetH5File()->createGroup( "/symbol/" + sSymbol );
    g.close();
    // assume all is well, or should we do in another try?
  }
}

herr_t CDataManager::PrintH5ErrorStackItem( int n, H5E_error_t *err_desc, void *client_data ) {
  // this is a call back from within an exception handler
  std::cout << "H5 Error Level " << n << ": " 
    << err_desc->file_name << "::" 
    << err_desc->func_name << "::"
    << err_desc->desc << std::endl;
  return 1;
}

CDataManager::~CDataManager(void) {
  --m_RefCount;
  if ( 0 == m_RefCount ) {
    m_H5File.close();
    m_DbEnv.close( 0 );
  }
}

