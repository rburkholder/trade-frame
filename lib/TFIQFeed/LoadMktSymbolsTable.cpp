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

#include "StdAfx.h"

#include <boost/scope_exit.hpp>

#include <ioapi.h>
#include <ioapi_mem.h>
#include <unzip.h>

#include "CurlGetMktSymbols.h"
#include "LoadMktSymbolsTable.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

LoadMktSymbolsTable::LoadMktSymbolsTable(void) {
}

LoadMktSymbolsTable::~LoadMktSymbolsTable(void) {
}

void LoadMktSymbolsTable::Load( void ) {

  CurlGetMktSymbols cgms;

  char* sZipFile( "**inmem**" );
  char* sSourceName( "mktsymbols_v2.txt" );

  unzFile uf=NULL;
  int err=UNZ_OK;
  unz_file_info64 file_info;
  char filename_inzip[256];
  ourmemory_t om;
  om.base = 0;
  om.cur_offset = om.limit = om.size = 0;

  om.base = cgms.Buffer();
  om.size = cgms.Size();

  char* pchUnzippedFileContent;
  pchUnzippedFileContent = 0;

  BOOST_SCOPE_EXIT( &pchUnzippedFileContent ) {
    if ( 0 != pchUnzippedFileContent ) {
      delete[] pchUnzippedFileContent;
      pchUnzippedFileContent = 0;
    }
  } BOOST_SCOPE_EXIT_END

  zlib_filefunc64_def ffunc;

  fill_memory_filefunc64( &ffunc, &om );

  uf = unzOpen2_64(sZipFile, &ffunc);
  if ( 0 == uf ) 
    throw std::runtime_error( "open" );

  err = unzLocateFile( uf, sSourceName, 0 );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "locate" );

  err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "get info" );

  err = unzOpenCurrentFile( uf );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "open current" );

  pchUnzippedFileContent = new char[ file_info.uncompressed_size ];
  if ( 0 == pchUnzippedFileContent ) {
    throw  std::runtime_error( "UnzippedFileContent" );
  }

  int cnt = unzReadCurrentFile(uf,pchUnzippedFileContent,file_info.uncompressed_size);
  if ( file_info.uncompressed_size != cnt ) 
    throw  std::runtime_error( "read" );

  err = unzCloseCurrentFile( uf );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "close current" );

  err = unzClose( uf );
  if ( UNZ_OK != err ) 
    throw  std::runtime_error( "close" );

}

} // namespace iqfeed
} // namespace tf
} // namespace ou
