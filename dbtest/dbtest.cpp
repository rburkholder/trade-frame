//============================================================================
// Name        : test.cpp
// Author      : Raymond
// Version     :
// Copyright   : (c) 2010 One Unified
// Description : 
//============================================================================

#include "stdafx.h"

#include <string>
#include <algorithm>
#include <iostream>

//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/spirit/include/phoenix_operator.hpp>

//using namespace boost::phoenix;
//using namespace boost::phoenix::arg_names;

#include <LibSqlite/DbSession.h>


#include <LibTrading/TradingDb.h>
#include <LibTrading/AccountManager.h>


template <typename K, typename F, typename T> // C class with shared ptr, F is function type, K is type of key
void constructClass( 
  const std::string& sErrPrefix, 
  const std::string& sSelect, 
  const K& key,
  sqlite3_stmt** ppStmt,
  boost::shared_ptr<T> ptr,
  F f
  ) {
  std::cout << sErrPrefix << ", " << sSelect << std::endl;
  //*ptr = f( key, ppStmt );
  ptr.
};


std::string const sDbFileName = "dbtest.sqlite3";

class CTestCase {
public:

  enum enumfield2 { EOne, ETwo, EThree };

  template<typename A> // A = Action
  void TableDef( A& a ) {
    Table( a, "test" );
    Key( a, "mykey", m_key );
    Field( a, "field1", m_field1 );
    Field( a, "field2", m_field2 );
    Field( a, "field3", m_field3 );
  };

protected:
private:
  boost::int64_t m_key;
  std::string m_field1;
  enumfield2 m_field2;
  int m_field3;
};

int _tmain(int argc, _TCHAR* argv[]) {

  int i = sizeof( CTestCase::enumfield2 );

  CTestCase tc;
  CDbSession session( sDbFileName.c_str() );
  CStatementCreateTable ct;
  tc.TableDef( ct );
  session.Prepare( ct );

  //std::string s( "test" );
  //std::string::iterator iter;

  //sqlite3_stmt* pStmt;
  //std::string key( "rpb001" );
  CAccountAdvisor::sharedptr_t p;
//  create_class c;
  //p = c( "key", &pStmt );
  //constructClass( "this is an error", "select me from you", key, &pStmt, &p, create_class() );

  //iter = std::find_if( s.begin(), s.end(), arg1 == 'e' );

  CTradingDb db( sDbFileName.c_str() );

  CAccountManager am( db.GetDb() );

  //am.CreateDbTables();
//  CAccountAdvisor aa( db.GetDb(), "ourpb001", "Raymond Burkholder" );

  //std::map<std::string,CAccountAdvisor::pAccountAdvisor_t> map;
  //CAccountAdvisor::sharedptr_t p;

  //sqlite3_stmt* pStmt( NULL );
  std::string key( "ourpb001" );
  //am.LoadObject( "AccountAdvisor", CAccountAdvisor::GetSelect(), key, &pStmt, map, p, bind_key(), create_class() );

  p = am.GetAccountAdvisor( key );


//	boost::fusion::for_each( m_tplR1, print_xml() );
//  boost::fusion::for_each( m_tplR1, UpdateRecordField() );

	return 0;
}

