/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include "Sql.h"

namespace ou {
namespace db {

//
// CSqlQuery
//

template<class F>  // F: Field Defs
class CSqlQuery: public CSql<F> {
public:

  typedef boost::shared_ptr<CSqlQuery<F> > pCSqlQuery_t;

  CSqlQuery( IDatabase& db, const std::string sStatement );
  ~CSqlQuery( void ) {};

protected:

  void ComposeStatement( std::string& sStatement ) { 
    // assumes ComposeStatement is called from within self constructor
    sStatement = *m_psStatement;
  };

private:
  const std::string* m_psStatement;  // one time use only within ComposeStatement
  CSqlQuery( void );  // no default constructor
  CSqlQuery( const CSqlQuery& );  // no default copy constructor
};

template<class F>
CSqlQuery<F>::CSqlQuery( IDatabase& db, const std::string sStatement )
: CSql<F>( db ), m_psStatement( &sStatement )
{
  PrepareStatement();
}

} // db
} // ou
