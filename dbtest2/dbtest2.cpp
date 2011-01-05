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

  session.RegisterTable<CFieldsTable>( "test" );
  session.CreateTables();

  { // after use, delete shared pointers so close works properly, fix IDatabase::Close to use differently.
    ou::db::CSqlInsert<CFields>::pCSqlInsert_t pInsert = session.RegisterInsert<CFields>( "test" );
    ou::db::CSqlUpdate<CFields>::pCSqlUpdate_t pUpdate = session.RegisterUpdate<CFields>( "test" );
    ou::db::CSqlDelete<CFields>::pCSqlDelete_t pDelete = session.RegisterDelete<CFields>( "test" );

    ou::db::CSqlQuery<CFields>::pCSqlQuery_t pQuery = session.RegisterQuery<CFields>( "select * from test;" );
  }

  session.Close();

  return 0;

}

