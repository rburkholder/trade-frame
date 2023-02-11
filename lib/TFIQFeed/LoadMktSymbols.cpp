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

namespace detail {
  // shared between debug and release
  const std::string sFileNameMarketSymbolsText( "../mktsymbols_v2.txt" );
  const std::string sFileNameMarketSymbolsBinary( "../symbols.ser" );
}

typedef MarketSymbol::TableRowDef trd_t;

void LoadMktSymbols( InMemoryMktSymbolList& symbols, MktSymbolLoadType::Enum e, bool bSaveTextToDisk, const std::string& sName ) {
  // valid combinations:
  // bDownload            t t f
  // bLoadTextFromDisk    f f t
  // bSaveTextToDisk      f t f
  if (
    (   MktSymbolLoadType::Download == e ) ||
    ( ( MktSymbolLoadType::LoadTextFromDisk == e ) && !bSaveTextToDisk )
    ) {
  }
  else {
    throw std::runtime_error( "illegal option combination" );
  }

  symbols.Clear();

  ValidateMktSymbolLine validator;
  validator.SetOnProcessLine( MakeDelegate( &symbols, &InMemoryMktSymbolList::InsertParsedStructure ) );

  switch ( e ) {
  case MktSymbolLoadType::Download:
    try {

      CurlGetMktSymbols cgms;

      UnZipMktSymbolsFile uzmsf;
      UnZipMktSymbolsFile::pUnZippedFile_t pUnZippedFile = uzmsf.UnZip( cgms.Buffer(), cgms.Size() );

      if ( bSaveTextToDisk ) {
        std::ofstream file;
        //char* name = "mktsymbols_v2.txt";
        std::cout << "Writing Symbol File " << std::endl;
        file.open( sName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary );
        if ( file.bad() ) {
          throw std::runtime_error( "can't open output file" );
        }
        else {
          file.write( pUnZippedFile.get(), uzmsf.UnZippedFileSize() );
          file.close();
        }
      }

      std::cout << "Processing Contents" << std::endl;
      const char* pBegin = pUnZippedFile.get();
      const char* pEnd = pBegin + uzmsf.UnZippedFileSize();
      validator.ParseHeaderLine( pBegin, pEnd );
      while ( pBegin != pEnd ) {
        validator.Parse( pBegin, pEnd );
      }
    }
    catch( ... ) {
      std::cout << "Some Sort of failure in Download" << std::endl;
    }
    break;
  case MktSymbolLoadType::LoadTextFromDisk:
    typedef ParseMktSymbolDiskFile diskfile_t;

    try {

      diskfile_t diskfile;
      diskfile.SetOnProcessLine( MakeDelegate( &validator, &ValidateMktSymbolLine::Parse<diskfile_t::iterator_t> ) );

      diskfile.Run( detail::sFileNameMarketSymbolsText );
    }
    catch (...) {
      std::cout << "Some sort of failure on disk read" << std::endl;
    }
    break;
  }

  validator.SetOnProcessHasOption( MakeDelegate( &symbols, &InMemoryMktSymbolList::HandleSymbolHasOption ) );
  validator.SetOnUpdateOptionUnderlying( MakeDelegate( &symbols, &InMemoryMktSymbolList::HandleUpdateOptionUnderlying ) );
  validator.PostProcess();
  validator.Summary();

}

} // namespace iqfeed
} // namespace tf
} // namespace ou
