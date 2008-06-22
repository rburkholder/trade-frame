#pragma once

#include "HDF5DataManager.h"
using namespace H5;

#include <string>

// called from IterateCallback (which is called as HDF5 iterates the directory
// this class is called recursively as the group hierarchy is traversed
// class T needs to supply method: Process( sObjectName, sObjectPath );
// example usage in SymbolSelectionFilter

/*
  string sBaseGroup = "/bar/86400/";
  HDF5IterateGroups<CSymbolSelectionFilter> control;
  int result = control.Start( sBaseGroup, this );
  */

template<class T> class HDF5IterateGroups {
public:
  HDF5IterateGroups<T>( void ) {};
  int Start( const std::string &sBaseGroup, T *pFilter ) {
    CHDF5DataManager dm;
    m_sBaseGroup = sBaseGroup;
    m_pFilter = pFilter;
    int idx = 0;  // starting location for interrupted queries
    int result = dm.GetH5File()->iterateElems( sBaseGroup, &idx, &HDF5IterateCallback, this );  
    return result;
  }
  void Process( const std::string &sObjectName ) {
    CHDF5DataManager dm;
    std::string sObjectPath;
    sObjectPath = m_sBaseGroup + sObjectName;
    H5G_stat_t stats;
    try {
      dm.GetH5File()->getObjinfo( sObjectPath, stats );
      switch ( stats.type ) {
        case H5G_DATASET: 
          try {
            m_pFilter->Process( sObjectName, sObjectPath );
          }
          catch ( std::exception e ) {
            std::cout << "CFilterSelectionIteratorControl::Process Object " << sObjectName << " problem: " << e.what() << std::endl;
          }
          catch (...) {
            std::cout << "CFilterSelectionIteratorControl::Process Object " << sObjectName << " unknown problems" << std::endl;
          }
          break;
        case H5G_GROUP:
          int idx = 0;  // starting location for interrupted queries
          sObjectPath.append( "/" );
          HDF5IterateGroups control;  // recursive call
          int result = control.Start( sObjectPath, m_pFilter );
          break;
      }
    }
    catch ( H5::Exception e ) {
      cout << "CFilterSelectionIteratorControl::Process H5::Exception " << e.getDetailMsg() << endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, 0 );
    }
  }
protected:
  std::string m_sBaseGroup;
  T *m_pFilter;
private:
  static herr_t HDF5IterateCallback( hid_t group, const char *name, void *op_data );
};

template<class T> herr_t HDF5IterateGroups<T>::HDF5IterateCallback( hid_t group, const char *name, void *op_data ) {
  HDF5IterateGroups<T> *pControl = ( HDF5IterateGroups<T> *) op_data;
  pControl->Process( name );
  return 0;
}

