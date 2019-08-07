/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/*
 * File:    ReadSymbolFile.h
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 6, 2019, 1:43 PM
 */

#ifndef READSYMBOLFILE_H
#define READSYMBOLFILE_H

class ReadSymbolFile {
public:

  using vSymbol_t = std::vector<std::string>;

  ReadSymbolFile( vSymbol_t& );
  virtual ~ReadSymbolFile( );
private:

};

#endif /* READSYMBOLFILE_H */

