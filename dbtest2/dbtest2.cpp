// dbtest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <LibSQL/Session.h>
#include <LibSQL/ISqlite3.h>



// test template getting at type without instantiating variable: complains about static call to non static function
// use full specialization or partial specialization



struct CFields {

  template<typename A> // A = Action
  void Fields( A& a ) {
    ou::db::Field( a, "mykey",  m_key,   "INTEGER" );  // type needed to match sqlite rowid
    ou::db::Field( a, "field1", m_field1 );
    ou::db::Field( a, "field2", m_field2 );
    ou::db::Field( a, "field3", m_field3 );
  };

  enum enumfield2 { EOne, ETwo, EThree };

  boost::int64_t m_key;
  std::string m_field1;
  enumfield2 m_field2;
  int m_field3;

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
  session.RegisterFields<CFields>();

  session.CreateTables();

  session.Close();

  return 0;

}

