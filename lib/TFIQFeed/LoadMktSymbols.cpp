/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// Started 2012/10/14

#include "StdAfx.h"

#include <stdexcept>

#include <fstream>

#include "CurlGetMktSymbols.h"
#include "UnzipMktSymbols.h"
#include "ParseMktSymbolDiskFile.h"
#include "ValidateMktSymbolLine.h"

#include "LoadMktSymbols.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

LoadMktSymbols::LoadMktSymbols(void) {
}

LoadMktSymbols::~LoadMktSymbols(void) {
}

void LoadMktSymbols::Load( bool bDownload, bool bLoadFromDisk, bool bSaveToDisk ) {
  // valid combinations:
  // bDownload      t t f
  // bLoadFromDisk  f f t
  // bSaveToDisk    f t f
  if (
    (  bDownload && !bLoadFromDisk && !bSaveToDisk ) ||
    (  bDownload && !bLoadFromDisk &&  bSaveToDisk ) ||
    ( !bDownload &&  bLoadFromDisk && !bSaveToDisk ) 
    ) {
  }
  else {
    throw std::runtime_error( "illegal option combination" );
  }

  if ( bDownload ) {
    try {

      CurlGetMktSymbols cgms;
      
      UnZipMktSymbolsFile uzmsf;
      UnZipMktSymbolsFile::pUnZippedFile_t pUnZippedFile = uzmsf.UnZip( cgms.Buffer(), cgms.Size() );

      if ( bSaveToDisk ) {
        std::ofstream file;
        char* name = "mktsymbols_v2.txt";
        std::cout << "Writing Symbol File " << std::endl;
        file.open( name, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary );
        if ( file.bad() ) {
          throw std::runtime_error( "can't open output file" );
        }
        else {
          file.write( pUnZippedFile.get(), uzmsf.UnZippedFileSize() );
          file.close();
        }
      }

      std::cout << "Processing Contents" << std::endl;
      ValidateMktSymbolLine validator;
      const char* pBegin = pUnZippedFile.get();
      const char* pEnd = pBegin + uzmsf.UnZippedFileSize();
      validator.SetOnProcessLine( MakeDelegate( &m_symbols, &symbols_t::HandleParsedStructure ) );
      validator.ParseHeaderLine( pBegin, pEnd );
      while ( pBegin != pEnd ) {
        validator.Parse( pBegin, pEnd );
      } 
      validator.SetOnProcessHasOption( MakeDelegate( &m_symbols, &symbols_t::HandleSymbolHasOption ) );
      validator.SetOnUpdateOptionUnderlying( MakeDelegate( &m_symbols, &symbols_t::HandleUpdateOptionUnderlying ) );
      validator.PostProcess();
      validator.Summary();

    }
    catch( ... ) {
      std::cout << "Some Sort of failure in Download" << std::endl;
    }
  }

  if ( bLoadFromDisk ) {

    typedef ParseMktSymbolDiskFile diskfile_t;

    try {

      diskfile_t diskfile;
      ValidateMktSymbolLine validator;
      diskfile.SetOnProcessLine( MakeDelegate( &validator, &ValidateMktSymbolLine::Parse<diskfile_t::iterator_t> ) );
      validator.SetOnProcessLine( MakeDelegate( &m_symbols, &symbols_t::HandleParsedStructure ) );

      diskfile.Run();

      validator.SetOnProcessHasOption( MakeDelegate( &m_symbols, &symbols_t::HandleSymbolHasOption ) );
      validator.SetOnUpdateOptionUnderlying( MakeDelegate( &m_symbols, &symbols_t::HandleUpdateOptionUnderlying ) );
      validator.PostProcess();
      validator.Summary();
    }
    catch (...) {
      std::cout << "Some sort of failure on disk read" << std::endl;
    }

  }

}

} // namespace iqfeed
} // namespace tf
} // namespace ou
