// IsoCurrencyXmlToCpp.cpp : Defines the entry point for the console application.
//

// iso 4217
// source of xml file:  http://www.currency-iso.org/dam/downloads/dl_iso_table_a1.xml
// from site:  http://www.currency-iso.org/en/home/tables.html

// some currency file projects:
//  http://code.google.com/p/isomon/
//  http://sourceforge.net/projects/ccy/

// related info:
// wiki:  http://en.wikipedia.org/wiki/ISO_4217
// xe:  http://www.xe.com/

#include "stdafx.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

//#include <boost/property_tree/detail/rapidxml.hpp>  // boosts version is not the full version, use at own peril
//#include <boost/property_tree/detail/>
#include <rapidxml/rapidxml.hpp>
//#include <rapidxml/rapidxml_print.hpp>

#include <boost/lexical_cast.hpp>

// at some point, could re-write to use the Spirit Parser

int _tmain(int argc, _TCHAR* argv[]) {

  char* pc;
  {
    std::stringstream ss;

    std::ifstream file( "C:\\Data\\Projects\\VSC++\\TradeFrame\\lib\\OUCommon\\IsoCurrency.xml" );
    if ( !file.bad() ) {
      ss << file.rdbuf();
    }
    pc = new char[ ss.str().length() + 1 ];
    memcpy( pc, ss.str().c_str(), ss.str().length() + 1 );
  }

  rapidxml::xml_document<> doc;
  doc.parse<0>( pc );

  struct structCurrencyUnit {
    std::string sEntity;
    std::string sCurrency;
    std::string sAlphaCode;
    unsigned int nCode;
    unsigned int nMinorUnit;
    structCurrencyUnit( void ): nCode( 0 ), nMinorUnit( 0 ) {};
  };

  std::ofstream file( "C:\\Data\\Projects\\VSC++\\TradeFrame\\lib\\OUCommon\\IsoCurrency.cpp" );
  if ( file.bad() ) {
    throw std::runtime_error( "bad output file" );
  }

  file << "typedef boost::tuple<std::string, std::string, std::string, unsigned int, unsigned int> tupleCurrencyCodes;" << std::endl;
  file << "typedef std::vector<tupleCurrencyCodes> vCurrencyCodes_t;" << std::endl;
  file << "static vCurrencyCodes_t vCurrencyCodes = tuple_list_of " << std::endl;

  rapidxml::xml_node<> *pNodeRoot = doc.first_node( "ISO_CCY_CODES" );
  rapidxml::xml_node<> *pNode = pNodeRoot->first_node( "ISO_CURRENCY" );
  while ( 0 != pNode ) {
    if ( 0 == strcmp( "ISO_CURRENCY", pNode->name() ) ) {
      structCurrencyUnit cu;
      for ( rapidxml::xml_node<> *pSubNode = pNode->first_node(); 0 != pSubNode; pSubNode = pSubNode->next_sibling() ) {
        if ( 0 == strcmp( "ENTITY", pSubNode->name() ) ) {
          cu.sEntity = pSubNode->value();
          std::string::iterator iter;
          std::string::size_type loc = cu.sEntity.find( '"' );
          while ( std::string::npos != loc ) {
            cu.sEntity.erase( loc, 1 );
            loc = cu.sEntity.find( '"' );
          }
        }
        if ( 0 == strcmp( "CURRENCY", pSubNode->name() ) ) {
          cu.sCurrency = pSubNode->value();
        }
        if ( 0 == strcmp( "ALPHABETIC_CODE", pSubNode->name() ) ) {
          cu.sAlphaCode = pSubNode->value();
        }
        if ( 0 == strcmp( "NUMERIC_CODE", pSubNode->name() ) ) {
          try {
            cu.nCode = boost::lexical_cast<unsigned int>( pSubNode->value() );
          }
          catch (...) {};
        }
        if ( 0 == strcmp( "MINOR_UNIT", pSubNode->name() ) ) {
          try {
            cu.nMinorUnit = boost::lexical_cast<unsigned int>( pSubNode->value() );
          }
          catch( ... ) {};
        }
      }
      if ( 0 < cu.sAlphaCode.length() ) {
        file << "("
          << "\"" << cu.sEntity << "\""
          << "," << "\"" << cu.sCurrency << "\""
          << "," << "\"" << cu.sAlphaCode << "\""
          << "," << cu.nCode
          << "," << cu.nMinorUnit
          << ")" << std::endl;
      }
    }
    
    pNode = pNode->next_sibling();
  }
  file << ";" << std::endl;

  file.close();
	return 0;
}

