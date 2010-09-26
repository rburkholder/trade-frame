//============================================================================
// Name        : test.cpp
// Author      : Raymond
// Version     :
// Copyright   : (c) 2010 One Unified
// Description : 
//============================================================================

#include "stdafx.h"

#include <string>

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



int _tmain(int argc, _TCHAR* argv[])
{

  std::string sDbName = "dbtest.db4";

  CAccountManager am( sDbName );

//	boost::fusion::for_each( m_tplR1, print_xml() );
//  boost::fusion::for_each( m_tplR1, UpdateRecordField() );

//	std::cout << "size: " << sizeof( structR1 ) << ", " << sizeof( m_recR1 ) << ", " << sizeof( m_tplR1 ) << std::endl;
	return 0;
}


