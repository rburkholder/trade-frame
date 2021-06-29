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
#pragma once

#include <string>
#include <iostream>

#include <boost/function.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include "HDF5DataManager.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// called from IterateCallback (which is called as HDF5 iterates the directory
// this class is called recursively as the group hierarchy is traversed
// class T needs to supply method: Process( sObjectName, sObjectPath );
// example usage in SymbolSelectionFilter

/*
  string sBaseGroup = "/bar/86400/";
  HDF5IterateGroups<CSymbolSelectionFilter> control;
  int result = control.Start( sBaseGroup, this );
  */

// FastDelegate flavoured version

class HDF5IterateGroups {
public:

  typedef FastDelegate2<const std::string&,const std::string&> OnObjectHandler_t;  // objectpath, objectname

  HDF5IterateGroups( void ) {};

  void SetOnHandleObject( OnObjectHandler_t handler ) {
    HandleObject = handler;;
  }
  void SetOnHandleGroup( OnObjectHandler_t handler ) {
    HandleGroup = handler;
  }

  int Start( const std::string& sBaseGroup ) {
    HDF5DataManager dm( HDF5DataManager::RO );
    m_sBaseGroup = sBaseGroup;
    int idx = 0;  // starting location for interrupted queries
    int result = dm.GetH5File()->iterateElems( sBaseGroup, &idx, &HDF5IterateCallback, this );
    return result;
  }

protected:
  std::string m_sBaseGroup;
private:
  OnObjectHandler_t HandleObject;
  OnObjectHandler_t HandleGroup;

  void Process( const std::string &sObjectName ) {
    HDF5DataManager dm( HDF5DataManager::RO );
    std::string sObjectPath;
    if ( '/' == m_sBaseGroup[ m_sBaseGroup.size() - 1 ] ) {
      sObjectPath = m_sBaseGroup + sObjectName;
    }
    else {
      sObjectPath = m_sBaseGroup + '/' + sObjectName;
    }
    H5G_stat_t stats;
    try {
      dm.GetH5File()->getObjinfo( sObjectPath, stats );
      switch ( stats.type ) {
        case H5G_DATASET:
          try {
            if ( NULL != HandleObject ) HandleObject( sObjectPath, sObjectName );
          }
          catch ( std::exception e ) {
            std::cout << "CFilterSelectionIteratorControl::Process Object " << sObjectName << " problem: " << e.what() << std::endl;
          }
          catch (...) {
            std::cout << "CFilterSelectionIteratorControl::Process Object " << sObjectName << " unknown problems" << std::endl;
          }
          break;
        case H5G_GROUP: {
            int idx = 0;  // starting location for interrupted queries
            sObjectPath.append( "/" );
            if ( NULL != HandleGroup ) HandleGroup( sObjectPath, sObjectName );
            HDF5IterateGroups control;  // recursive call
            control.SetOnHandleObject( HandleObject );
            control.SetOnHandleGroup( HandleGroup );
            int result = control.Start( sObjectPath );
          }
          break;
        default:
          break;
      }
    }
    catch ( H5::Exception e ) {
      std::cout << "CFilterSelectionIteratorControl::Process H5::Exception " << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, 0 );
    }
  }

  static herr_t HDF5IterateCallback( hid_t group, const char *name, void *op_data ) {
    HDF5IterateGroups* pControl = ( HDF5IterateGroups *) op_data;
    pControl->Process( name );
  return 0;
  }

};

// boost lambda flavoured version

namespace hdf5 {
/*
IterateGroups Call example:
    #include <boost/phoenix/bind/bind_member_function.hpp>
    namespace args = boost::phoenix::placeholders;
    ou::tf::hdf5::IterateGroups ig(
      "/bar/86400",
      boost::phoenix::bind( &AppScanner::HandleHdf5Group, this, args::arg1, args::arg2 ),
      boost::phoenix::bind( &AppScanner::HandleHdf5Object, this, args::arg1, args::arg2 )
      );
*/
class IterateGroups {
public:
  typedef boost::function<void (const std::string&, const std::string& )> callback_t;
  IterateGroups( const std::string& sBaseGroup, callback_t group, callback_t object )
    : m_dm( HDF5DataManager::RO ), m_g( group ), m_o( object )
  {
    m_sBaseGroup = sBaseGroup;
    int idx = 0;  // starting location for interrupted queries
    int result = m_dm.GetH5File()->iterateElems( sBaseGroup, &idx, &IterateGroups::IterateCallback, this );
  }
  ~IterateGroups( void ) {};
protected:
private:

  std::string m_sBaseGroup;
  HDF5DataManager m_dm;
  callback_t m_g;
  callback_t m_o;

  static herr_t IterateCallback( hid_t group, const char *name, void *op_data ) {
    IterateGroups& ig( *reinterpret_cast<IterateGroups*>( op_data ) );
    std::string sObjectPath;
    std::string sObjectName( name );
    if ( '/' == ig.m_sBaseGroup[ ig.m_sBaseGroup.size() - 1 ] ) {
      sObjectPath = ig.m_sBaseGroup + sObjectName;
    }
    else {
      sObjectPath = ig.m_sBaseGroup + '/' + sObjectName;
    }
    H5G_stat_t stats;
    try {
      ig.m_dm.GetH5File()->getObjinfo( sObjectPath, stats );
      switch ( stats.type ) {
        case H5G_DATASET:
          try {
            ig.m_o( sObjectPath, name );
          }
          catch ( std::exception e ) {
            std::cout << "HDF5::IterateGroups::Process Object " << sObjectPath << "," << sObjectName << " problem: " << e.what() << std::endl;
          }
          catch (...) {
            std::cout << "HDF5::IterateGroups::Process Object " << sObjectName << " unknown problems" << std::endl;
          }
          break;
        case H5G_GROUP: {
            sObjectPath.append( "/" );
            ig.m_g( sObjectPath, sObjectName );
            IterateGroups igRecursive( sObjectPath, ig.m_g, ig.m_o );  // recursive call
          }
          break;
        default:
          break;
      }
    }
    catch ( H5::Exception e ) {
      std::cout << "HDF5::IterateGroups::Process H5::Exception " << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &HDF5DataManager::PrintH5ErrorStackItem, 0 );
    }
    return 0;
  }

};
}

} // namespace tf
} // namespace ou
