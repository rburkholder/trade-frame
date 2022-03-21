/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <cassert>
#include <stdexcept>

#include <boost/fusion/include/std_pair.hpp>

#include <boost/spirit/include/qi.hpp>

#include <OUCommon/TimeSource.h>

#include "Instrument.h"

using mapExchangeRule_t = ou::tf::Instrument::mapExchangeRule_t;

namespace qi = boost::spirit::qi;

// http://boost-spirit.com/home/articles/qi-example/parsing-a-list-of-key-value-pairs-using-spirit-qi/

template<typename Iterator>
struct ParseMarketRules: qi::grammar<Iterator, mapExchangeRule_t()> {

  ParseMarketRules(): ParseMarketRules::base_type( start ) {

    ruleExchange = ( +( qi::char_ - qi::char_(",=") ) );
    ruleMarketRuleId = qi::int_;
    ruleExchangeRule = ruleExchange >> qi::lit('=') >> ruleMarketRuleId;
    start = ruleExchangeRule >> *( qi::lit(",") >> ruleExchangeRule );

  }

  qi::rule<Iterator, std::string()> ruleExchange;
  qi::rule<Iterator, int()> ruleMarketRuleId;
  qi::rule<Iterator, std::pair<std::string,int>()> ruleExchangeRule;
  //qi::rule<Iterator, mapExchangeRule_t::value_type()> ruleExchangeRule;
  qi::rule<Iterator, mapExchangeRule_t()> start;

};

namespace ou { // One Unified
namespace tf { // TradeFrame

static const boost::posix_time::ptime dtDefault( boost::posix_time::not_a_date_time );

std::string Instrument::BuildDate( boost::gregorian::date date ) {
  auto ymd = date.year_month_day();
  return BuildDate( ymd.year, ymd.month.as_number(), ymd.day.as_number() );
}

std::string Instrument::BuildDate( uint16_t year, uint16_t month, uint16_t day ) {
  return
    boost::lexical_cast<std::string>( year )
    + ( ( 9 < month ) ? "" : "0" ) + boost::lexical_cast<std::string>( month )
    + ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day )
    ;
}

std::string Instrument::BuildGenericOptionName( const std::string& sUnderlying, uint16_t year, uint16_t month, uint16_t day, OptionSide::enumOptionSide side, double strike ) { // month is 1 based
  std::string sGenericName;
  std::string::size_type pos = sUnderlying.find_first_of( "-" );
  if ( std::string::npos == pos ) {
    sGenericName = sUnderlying;
  }
  else {
    sGenericName = sUnderlying.substr( 0, pos );
  }
  //std::string sGenericName( sUnderlying );
  sGenericName += "-" + BuildDate( year, month, day );
  sGenericName += "-";
  sGenericName += side;
  sGenericName += "-" + boost::lexical_cast<std::string>( strike );
  return sGenericName;
}

std::string Instrument::BuildGenericFutureName( const std::string& sUnderlying, uint16_t year, uint16_t month, uint16_t day ) { // month is 1 based
 std::string sGenericName( sUnderlying );
  sGenericName += "-" + boost::lexical_cast<std::string>( year )
    + ( ( 9 < month ) ? "" : "0" ) + boost::lexical_cast<std::string>( month )
    + ( ( 0 == day ) ? "" : ( ( ( 9 < day ) ? "" : "0" ) + boost::lexical_cast<std::string>( day ) ) )
    ;
  return sGenericName;
}

std::string Instrument::BuildGenericOptionName( const std::string& sUnderlying, boost::gregorian::date date, OptionSide::enumOptionSide side, double strike ) {
  std::string sGenericName;
  std::string::size_type pos = sUnderlying.find_first_of( "-" );
  if ( std::string::npos == pos ) {
    sGenericName = sUnderlying;
  }
  else {
    sGenericName = sUnderlying.substr( 0, pos );
  }
  //std::string sGenericName( sUnderlying );
  sGenericName += "-" + BuildDate( date );
  sGenericName += "-";
  sGenericName += side;
  sGenericName += "-" + boost::lexical_cast<std::string>( strike );
  return sGenericName;
}

std::string Instrument::BuildGenericFutureName( const std::string& sUnderlying, boost::gregorian::date date ) {
 std::string sGenericName( sUnderlying );
  sGenericName += "-" + BuildDate( date );
  return sGenericName;
}

Instrument::Instrument( const TableRowDef& row )
  : m_row( row ),
//  m_eUnderlyingStatus( EUnderlyingNotSettable ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),
  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  switch ( row.eType ) {
    case InstrumentType::Stock:
      break;
    case InstrumentType::Index:
      break;
    case InstrumentType::Option:
      break;
    case InstrumentType::FuturesOption:
      break;
    case InstrumentType::Future:
      break;
    default:
      assert( false );
  }
}

// just enough to obtain fundamentals
Instrument::Instrument( idInstrument_cref idInstrument )
: m_row( idInstrument ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{}

// equity / generic creation
Instrument::Instrument(
  idInstrument_cref idInstrument,
  InstrumentType::EInstrumentType eType,
  const idExchange_t &idExchange
)
: m_row( idInstrument, eType, idExchange ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{}

 // future
Instrument::Instrument(
  idInstrument_cref idInstrument,
  InstrumentType::EInstrumentType eType,
  const idExchange_t& idExchange,
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day )
: m_row( idInstrument, eType, idExchange, year, month, day ),
//  m_eUnderlyingStatus( EUnderlyingNotSettable ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option yymm // TODO: get this removed
Instrument::Instrument(
  idInstrument_cref idInstrument,
  InstrumentType::EInstrumentType eType,
  const idExchange_t& idExchange,
  boost::uint16_t year, boost::uint16_t month,
//  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide eOptionSide,
  double dblStrike )
  : m_row( idInstrument, eType, idExchange,
//  pUnderlying->GetInstrumentName(),
  year, month, eOptionSide, dblStrike ),
//  m_pUnderlying( pUnderlying ),
//  m_eUnderlyingStatus( EUnderlyingSet ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  //assert( 0 < m_sExchange.size() );
//  assert( 0 != pUnderlying.get() );
//  assert( "" != pUnderlying->GetInstrumentName() );
  //m_eUnderlyingStatus = EUnderlyingSet;
  //m_eUnderlyingStatus = EUnderlyingNotSettable;  // not sure which to use
}

 // option yymmdd
Instrument::Instrument(
  idInstrument_cref idInstrument,
  InstrumentType::EInstrumentType eType,
  const idExchange_t& idExchange,
  boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
//  pInstrument_t pUnderlying,
  OptionSide::enumOptionSide eOptionSide,
  double dblStrike )
  : m_row( idInstrument, eType, idExchange,
//  pUnderlying->GetInstrumentName(),
  year, month, day, eOptionSide, dblStrike ),
//  m_pUnderlying( pUnderlying ),
//  m_eUnderlyingStatus( EUnderlyingSet ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  //assert( 0 < m_sExchange.size() );
//  assert( 0 != pUnderlying.get() );
//  assert( "" != pUnderlying->GetInstrumentName() );
  //m_eUnderlyingStatus = EUnderlyingSet;
  //m_eUnderlyingStatus = EUnderlyingNotSettable;  // not sure which to use
}

// currency
Instrument::Instrument(
  const idInstrument_t& idInstrument,
//                       const idInstrument_t& idCounterInstrument,
  InstrumentType::EInstrumentType eType,
  const idExchange_t& idExchange,
  Currency::enumCurrency base,
  Currency::enumCurrency counter
  )
  : m_row( idInstrument,
//  idCounterInstrument,
  eType, idExchange, base, counter ),
//  m_pUnderlying( pUnderlying ),
//  m_eUnderlyingStatus( EUnderlyingNotSettable ),
  m_dtrTimeLiquid( dtDefault, dtDefault ),  m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{

}

Instrument::Instrument( const Instrument& instrument )
:
  m_row( instrument.m_row ),
  m_dtrTimeLiquid( dtDefault, dtDefault ), m_dtrTimeTrading( dtDefault, dtDefault ),
  m_dateCommonCalc( boost::gregorian::not_a_date_time )
{
  mapAlternateNames_t::const_iterator iter = instrument.m_mapAlternateNames.begin();
  while ( instrument.m_mapAlternateNames.end() != iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_t::value_type( iter->first, iter->second ) );
  }
}

Instrument::~Instrument() {
}

void Instrument::SetAlternateName( eidProvider_t id, idInstrument_cref name ) {
  mapAlternateNames_t::iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() == iter ) {
    m_mapAlternateNames.insert( mapAlternateNames_t::value_type( id, name ) );
//    OnAlternateNameAdded( AlternateNameChangeInfo_t( id, m_row.idInstrument, name ) );  // 2012/02/05  this creates a loop when loading alt names in instrument manager
  }
  else {
    if ( iter->second != name ) {
      idInstrument_t old = iter->second;
      iter->second.assign( name );
      OnAlternateNameChanged( AlternateNameChangeInfo_t( id, old, name ) );
    }
  }
}

Instrument::idInstrument_cref Instrument::GetInstrumentName( eidProvider_t id ) const {
  mapAlternateNames_t::const_iterator iter = m_mapAlternateNames.find( id );
  if ( m_mapAlternateNames.end() != iter ) {
    //throw std::runtime_error( "Instrument::GetAlternateName no alternate name" );
    return iter->second;
  }
  return m_row.idInstrument;
}

bool Instrument::operator==( const Instrument& rhs ) const {
  return (
    ( m_row.idInstrument == rhs.m_row.idInstrument )
    && ( m_row.idExchange == rhs.m_row.idExchange )
    && ( m_row.eType == rhs.m_row.eType )
//    && ( m_row.idUnderlying == rhs.m_row.idUnderlying )
    && ( m_row.dblStrike == rhs.m_row.dblStrike )
    && ( m_row.eOptionSide == rhs.m_row.eOptionSide )
    );
}

// use 16:00 est as time of expiry, as that is when they cease trading (for OPRA equities)
// 18:30 deals with after hours trading and settlements on the underlying.  the options cease trading at 16:00.

boost::posix_time::ptime Instrument::GetExpiryUtc( void ) const {
  // may require further time refinements with other option types
  // may require a table for handling other markets and time zones
  boost::gregorian::date dateExpiry( m_row.nYear, m_row.nMonth, m_row.nDay );
  switch ( m_row.eType ) {
    case InstrumentType::Option:  // for equities options
      return ou::TimeSource::Instance().
              ConvertRegionalToUtc( dateExpiry, boost::posix_time::time_duration( 16, 0, 0 ), "America/New_York", true );
      break;
    case InstrumentType::FuturesOption:  // for metals options
      return ou::TimeSource::Instance().
              ConvertRegionalToUtc( dateExpiry, boost::posix_time::time_duration( 13, 30, 0 ), "America/New_York", true );
      break;
  }
  return ou::TimeSource::Instance().
          ConvertRegionalToUtc( dateExpiry, boost::posix_time::time_duration( 16, 0, 0 ), "America/New_York", true );
}

double Instrument::NormalizeOrderPrice( double price ) const {
  return NormalizeOrderPrice( price, m_row.dblMinTick );
}

double Instrument::NormalizeOrderPrice( double price, double interval ) {
  assert( 0.0 <= price );
  assert( 0.0 < interval );
  const double round = interval / 2.0;
  const double multiple_rough = price / interval;
  const double multiple_floor = std::floor( multiple_rough );
  const double lower = multiple_floor * interval;
  double dblResult = ( price < ( lower + round ) ) ? lower : ( lower + interval );
  std::cout
    << "Instrument::NormalizeOrderPrice: "
    << "interval=" << interval << ","
    << "price=" << price << ","
    << "result=" << dblResult
    << std::endl;
  return dblResult;
}

int Instrument::GetExchangeRule() {

  if ( 0 == m_mapExchangeRule.size() ) {

    using const_string_iter = std::string::const_iterator;
    ParseMarketRules<const_string_iter> grammarParseMarketRules;

    const_string_iter bgn( m_row.sExchangeRules.begin() );
    const_string_iter end( m_row.sExchangeRules.end() );

    bool bOk = parse( bgn, end, grammarParseMarketRules, m_mapExchangeRule );
    assert( bOk );

  }

  mapExchangeRule_t::const_iterator iter = m_mapExchangeRule.find( m_row.idExchange );
  assert( m_mapExchangeRule.end() != iter );
  return iter->second;

}

} // namespace tf
} // namespace ou
