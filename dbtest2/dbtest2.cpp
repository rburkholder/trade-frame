// dbtest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

//#include <OUSQL/Session.h>
//#include <OUSqlite/ISqlite3.h>
#include <OUSqlite/Session.h>

struct CFields {

  template<typename A> // A = Action
  void Fields( A& a ) {
    ou::db::Field( a, "mykey",  m_key );  // type needed to match sqlite rowid
    ou::db::Field( a, "field1", m_sField );
    ou::db::Field( a, "field2", m_enumField );  // find out what the compiler thinks it is
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

//  ou::db::CSession<ou::db::ISqlite3> session;
  ou::db::CSession session;
  session.Open( ":memory:", ou::db::EOpenFlagsAutoCreate );
//  session.Open( "dbtest2.db", ou::db::EOpenFlagsAutoCreate );

  { // after use, delete shared pointers so close works properly, fix IDatabase::Close to use differently.
    
    session.RegisterTable<CFieldsTable>( "test" );
    session.CreateTables();

    CFields fields;
    fields.m_key = 4;
    fields.m_dblField = 3;
    fields.m_enumField = CFields::EThree;
    fields.m_intField = -45;
    fields.m_sField = "attempt";

    session.MapRowDefToTableName<CFields>( "test" );
    ou::db::QueryFields<CFields>::pQueryFields_t pInsert = session.Insert<CFields>( fields );

    fields.m_key = 6;
    fields.m_intField = 42;
    fields.m_enumField = CFields::ETwo;
    fields.m_sField = "answer";

    session.Reset( pInsert );  // could also add ability to pInsert->Reset();
    session.Bind<CFields>( pInsert );  // could also add ability to pInsert->Bind( fields );
    session.Execute( pInsert );  // could also add ability to pInsert->Execute(), or pInsert->Execute( bindingfields );

    CFieldsUpdate update;
    update.m_sField = "good";

    session.MapRowDefToTableName<CFieldsUpdate>( "test" );
    ou::db::QueryFields<CFieldsUpdate>::pQueryFields_t pUpdate 
      = session.Update<CFieldsUpdate>( update ).Where( "field1 = 'attempt'" );

    CFieldsDelete delete_;
    delete_.m_enumField = CFieldsDelete::enumfield2::ETwo;

    session.MapRowDefToTableName<CFieldsDelete>( "test" );
    ou::db::QueryFields<CFieldsDelete>::pQueryFields_t pDelete 
      = session.Delete<CFieldsDelete>( delete_ ).Where( "field2 = ?" );;

    fields.m_key = 7;
    fields.m_intField = 82;
    fields.m_enumField = CFields::EOne;
    fields.m_sField = "changed";

    session.Reset( pInsert );
    session.Bind<CFields>( pInsert );
    session.Execute( pInsert );

    CFields fields2;
    ou::db::QueryFields<ou::db::NoBind>::pQueryFields_t pSelect 
      = session.SQL<ou::db::NoBind>( "select * from test" );
    // this doesn't work properly with 0 records being returned
    do {
      session.Columns<ou::db::NoBind, CFields>( pSelect, fields2 );
      std::cout << fields2.m_key << ", " << fields2.m_sField << ", " << fields2.m_dblField << ", " << fields2.m_intField << std::endl;
    }
    while ( session.Execute( pSelect ) );

  }

  session.Close();

  std::cout << std::endl << "Should be: " << std::endl << "4, good, 3, -45" << std::endl << "7, changed, 3, 82" << std::endl;

  return 0;

}

/*
4, good, 3, -45
7, changed, 3, 82
*/