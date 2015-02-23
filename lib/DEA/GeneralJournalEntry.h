/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// started 2013-05-05

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <OUSQL/Functions.h>

#include <OUAuth/KeyTypes.h>
#include "KeyTypes.h"

namespace ou { // One Unified
namespace dea { // double entry accounting

// 20130524 need some validation checks when initializing with or without the key

class GeneralJournalEntry {
public:

  typedef ou::auth::keytypes::idUser_t idUser_t;
  typedef keytypes::idTransaction_t idTransaction_t;
  typedef keytypes::idReference_t idReference_t;
  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::money_t money_t;

  typedef boost::shared_ptr<GeneralJournalEntry> pGLEntry_t;

  struct TableRowDefNoKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "referenceid", idReference );  // ties related entries together
      ou::db::Field( a, "accountid", idAccount );
      ou::db::Field( a, "userid", idUser );
      ou::db::Field( a, "debit", mnyDebit );
      ou::db::Field( a, "credit", mnyCredit );
      ou::db::Field( a, "code", sCode );
      ou::db::Field( a, "created", dtCreated );
      ou::db::Field( a, "description", sDescription );
    }

    idReference_t idReference;
    idAccount_t idAccount;
    idUser_t idUser;
    money_t mnyDebit;
    money_t mnyCredit;
    std::string sCode;
    ptime dtCreated;
    std::string sDescription;

    TableRowDefNoKey( void ):  idReference( 0 ), idUser( 0 ), mnyDebit( 0 ), mnyCredit( 0 ), 
      dtCreated( boost::date_time::special_values::not_a_date_time ) {};
    TableRowDefNoKey( const TableRowDefNoKey& row ): 
      idReference( row.idReference ), idAccount( row.idAccount), idUser( row.idUser ),
      sCode( row.sCode ), dtCreated( row.dtCreated ),
      mnyDebit( row.mnyDebit ), mnyCredit( row.mnyCredit ), 
      sDescription( row.sDescription ) {};
    TableRowDefNoKey( const idReference_t& idReference_, const idAccount_t& idAccount_, const idUser_t& idUser_, 
      money_t mnyDebit_, money_t mnyCredit_, const std::string& sCode_, const std::string&  sDescription_ ):
        idReference( idReference_ ), idAccount( idAccount_ ), idUser( idUser_ ), 
          mnyDebit( mnyDebit_ ), mnyCredit( mnyCredit_ ), sCode( sCode_ ), sDescription( sDescription_ ) {};

  };

  struct TableRowDef: TableRowDefNoKey {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "transactionid", idTransaction );  // unique key
    }

    idTransaction_t idTransaction;

    TableRowDef( void ): idTransaction( 0 ), TableRowDefNoKey() {};
    TableRowDef( const TableRowDef& row ): 
      idTransaction( row.idTransaction ), TableRowDefNoKey( static_cast<TableRowDefNoKey>( row ) ) {};
    TableRowDef( const idTransaction_t& idTransaction_, const TableRowDefNoKey& row )
      : idTransaction( idTransaction_ ), TableRowDefNoKey( row ) {};
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "transactionid" );
      ou::db::Constraint( a, "accountid", "chartofaccounts", "accountid" );
    }
  };

  GeneralJournalEntry( const TableRowDef& row ): m_row( row ) {};
  GeneralJournalEntry( const idTransaction_t& idTransaction, const TableRowDefNoKey& row ): m_row( TableRowDef( idTransaction, row ) ) {};
  ~GeneralJournalEntry(void);

  void SetCreationTime( const ptime& dt ) { m_row.dtCreated = dt; };
  void SetKey( const idTransaction_t& idTransaction ) { m_row.idTransaction = idTransaction; };

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:
  TableRowDef m_row;
};

} // namespace dea
} // namespace ou
