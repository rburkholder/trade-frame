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

struct CFieldsUpdate {
  template<class A>
  void Fields( A& a ){
    ou::db::Field( a, "field1", m_sField );
  };

  std::string m_sField;

};

struct CFieldsDelete {
  template<class A>
  void Fields( A& a ){
    ou::db::Field( a, "field2", m_enumField );
  };

  typedef CFields::enumfield2 enumfield2;
  enumfield2 m_enumField;

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
    pInsert->m_enumField = CFields::EThree;
    pInsert->m_intField = -45;
    pInsert->m_sField = "attempt";

    session.Bind<CFields>( pInsert );
    session.Execute( pInsert );

    pInsert->m_key = 6;
    pInsert->m_intField = 42;
    pInsert->m_enumField = CFields::ETwo;
    pInsert->m_sField = "answer";

    session.Reset( pInsert );
    session.Bind<CFields>( pInsert );
    session.Execute( pInsert );

    ou::db::QueryFields<CFieldsUpdate>::pQueryFields_t pUpdate 
      = session.RegisterUpdate<CFieldsUpdate>( "test" )->Where( "field1 = 'attempt'" );

    pUpdate->m_sField = "good";

    session.Bind<CFieldsUpdate>( pUpdate );
    session.Execute( pUpdate );

    ou::db::QueryFields<CFieldsDelete>::pQueryFields_t pDelete 
      = session.RegisterDelete<CFieldsDelete>( "test" )->Where( "field2 = ?" );;

    pDelete->m_enumField = CFieldsDelete::enumfield2::ETwo;
    session.Bind<CFieldsDelete>( pDelete );
    session.Execute( pDelete );

    pInsert->m_key = 7;
    pInsert->m_intField = 82;
    pInsert->m_enumField = CFields::EOne;
    pInsert->m_sField = "changed";

    session.Reset( pInsert );
    session.Bind<CFields>( pInsert );
    session.Execute( pInsert );

//    ou::db::QueryFields<CFields>::pQueryFields_t pSelect 
//      = session.RegisterQuery<CFields>( se

  }

  session.Close();

  return 0;

}

