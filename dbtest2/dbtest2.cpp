// dbtest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <LibSQL/Session.h>

// test template getting at type without instatiating variable
// use full specialization or partial specialization



class CTestTable {
public:

  template<typename A> // A = Action
  void Fields( A& a ) {
    ou::db::Key(    a, "mykey", m_key );
    ou::db::Field(  a, "field1", m_field1 );
    ou::db::Field(  a, "field2", m_field2 );
    ou::db::Field(  a, "field3", m_field3 );
  };

  enum enumfield2 { EOne, ETwo, EThree };

  boost::int64_t m_key;
  std::string m_field1;
  enumfield2 m_field2;
  int m_field3;

protected:
private:
};

int _tmain(int argc, _TCHAR* argv[]) {

  ou::db::CSession session( "dbtest2.db", ou::db::CSession::EFlagsAutoCreate );

  //CTestTable table;

  //ou::db::CTableDef<CTestTable> td;

  session.RegisterTableDef<CTestTable>( "test" );

  session.CreateTables();

	return 0;

}

