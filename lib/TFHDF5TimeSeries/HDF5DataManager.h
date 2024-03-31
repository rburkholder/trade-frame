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

#include <functional>

#include <hdf5/H5Cpp.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class HDF5DataManager {
public:

  enum enumFileOptionType{ RDWR, RO };

  HDF5DataManager( enumFileOptionType );
  HDF5DataManager( enumFileOptionType, const std::string& sName ); // alternate hdf5 file
  ~HDF5DataManager();

  static const std::string& GetHdf5FileDefault() { return c_sH5FileName; }

  H5::H5File *GetH5File() { return &m_H5File; };
  bool GroupExists( const std::string &sGroup );
  void AddGroup( const std::string &sGroupPath );  // last group needs trailing '/'
  void AddGroupForSymbol( const std::string &sSymbol );
  static herr_t PrintH5ErrorStackItem( int n, H5E_error_t *err_desc, void *client_data );
//  static hsize_t H5ChunkSize( void ) { return 1024; };  // # elements to be shuffled/compressed in one block,  was 64
//  static hsize_t H5ChunkSize( void ) { return 32; };  // # elements to be shuffled/compressed in one block,  was 64
  static void DailyBarPath( const std::string &sSymbol, std::string &sPath );
  void Flush();

  using fCallbackIteratorPath_t = std::function<void(const std::string&)>;
  void IteratePathParts( const std::string& sPath, fCallbackIteratorPath_t&& );

protected:
  static const std::string c_sH5FileName;
//  static unsigned int m_RefCount;
//  static H5::H5File m_H5File;
  H5::H5File m_H5File;
private:
};

} // namespace tf
} // namespace ou
