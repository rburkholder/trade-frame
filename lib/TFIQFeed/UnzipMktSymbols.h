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

#include <boost/shared_array.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class UnZipMktSymbolsFile {
public:
  UnZipMktSymbolsFile(void): m_nUnZippedFileSize( 0 ) {};
  ~UnZipMktSymbolsFile(void) {};
  typedef boost::shared_array<char> pUnZippedFile_t;
  pUnZippedFile_t UnZip( char* pchSource, size_t nSourceSize );
  size_t UnZippedFileSize( void ) const { return m_nUnZippedFileSize; };
protected:
private:
  size_t m_nUnZippedFileSize;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
