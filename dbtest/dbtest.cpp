//============================================================================
// Name        : test.cpp
// Author      : Raymond
// Version     :
// Copyright   : (c) 2010 One Unified
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "stdafx.h"

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


//	boost::fusion::for_each( m_tplR1, print_xml() );
//  boost::fusion::for_each( m_tplR1, UpdateRecordField() );

//	std::cout << "size: " << sizeof( structR1 ) << ", " << sizeof( m_recR1 ) << ", " << sizeof( m_tplR1 ) << std::endl;
	return 0;
}


