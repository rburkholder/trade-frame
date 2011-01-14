// dbtest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <LibSQL/Session.h>
#include <LibSqlite/ISqlite3.h>



// test template getting at type without instantiating variable: complains about static call to non static function
// use full specialization or partial specialization



struct CFields {

  template<typename A> // A = Action
  void Fields( A& a ) {
    ou::db::Field( a, "mykey",  m_key,   "INTEGER" );  // type needed to match sqlite rowid
    ou::db::Field( a, "field1", m_sField );
    ou::db::Field( a, "field2", m_enumField );
    ou::db::Field( a, "field3", m_intField );
    ou::db::Field( a, "field4", m_dblField );
  };

  enum enumfield2 { EOne, ETwo, EThree };

  boost::int64_t m_key;
  std::string m_sField;
  enumfield2 m_enumField;
  int m_intField;
  double m_dblField;

};

struct CFieldsTable: public CFields {

  template<typename A>
  void Fields( A& a ) {
    CFields::Fields( a );
    ou::db::Key( a, "mykey" );
  };

};

int _tmain(int argc, _TCHAR* argv[]) {

  ou::db::CSession<ou::db::ISqlite3> session;
  session.Open( "dbtest2.db", ou::db::EOpenFlagsAutoCreate );


  { // after use, delete shared pointers so close works properly, fix IDatabase::Close to use differently.
    session.RegisterTable<CFieldsTable>( "test" );
    session.CreateTables();

    ou::db::QueryFields<CFields>::pQueryFields_t pInsert = session.RegisterInsert<CFields>( "test" );

    pInsert->m_key = 4;
    pInsert->m_dblField = 3;
    pInsert->m_enumField = pInsert->EThree;
    pInsert->m_intField = -45;
    pInsert->m_sField = "attempt";

    session.Bind<CFields>( pInsert );
    session.Execute( pInsert );

    ou::db::QueryFields<CFields>::pQueryFields_t pUpdate = session.RegisterUpdate<CFields>( "test" );
    ou::db::QueryFields<CFields>::pQueryFields_t pDelete = session.RegisterDelete<CFields>( "test" );

    ou::db::QueryFields<CFields>::pQueryFields_t pQuery = session.RegisterQuery<CFields>( "select * from test;" );
  }

  session.Close();

  return 0;

}

