//============================================================================
// Name        : test.cpp
// Author      : Raymond
// Version     :
// Copyright   : (c) 2010 One Unified
// Description : 
//============================================================================

#include "stdafx.h"

#include <string>

#include <LibTrading/TradingDb.h>
#include <LibTrading/AccountManager.h>

struct print_xml
{
    template <typename T>
    void operator()(T const& x) const
    {
        std::cout
            << '<' << typeid(x).name() << '>'
            << x.constant_value()
            << "</" << typeid(x).name() << '>'
            << std::endl
            ;
    }
};

std::string const sDbFileName = "dbtest.sqlite3";

int _tmain(int argc, _TCHAR* argv[])
{

  CTradingDb db( sDbFileName.c_str() );

  CAccountManager am( db.GetDb() );
  am.CreateDbTables();
  //am.AddAccountAdvisor( "ourpb01", "Raymond Burkholder" );

//	boost::fusion::for_each( m_tplR1, print_xml() );
//  boost::fusion::for_each( m_tplR1, UpdateRecordField() );

//	std::cout << "size: " << sizeof( structR1 ) << ", " << sizeof( m_recR1 ) << ", " << sizeof( m_tplR1 ) << std::endl;
	return 0;
}


