/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

/*
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 09, 2024 19:58:27
 */

#include <fstream>
#include <exception>

#include <boost/phoenix/core.hpp>

#include <boost/fusion/include/vector.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_BaseCurrency(    "base_currency" );
  static const std::string sChoice_CurrencyTopUp(   "base_currency_top_up" );
  static const std::string sChoice_PairSetting(     "pair_setting" );
  static const std::string sChoice_sExchange(       "exchange" );
  static const std::string sChoice_sHdf5File(       "hdf5_file" );
  static const std::string sChoice_sHdf5SimSet(     "hdf5_sim_set" );
  static const std::string sOption_IbInstance(      "ib_instance" );
  static const std::string sChoice_MaxLifeTime(     "max_life_time" );
  static const std::string sChoice_PipProfit(       "pip_profit" );
  static const std::string sChoice_PipStopLoss(     "pip_stop_loss" );
  static const std::string sChoice_PipTrailingStop( "pip_trailing_stop" );
  static const std::string sChoice_BarSeconds(      "bar_seconds" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, bool bRequired, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
    }
    else {
      if ( bRequired ) {
        BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
        bOk = false;
      }
    }
  return bOk;
  }
}

// https://www.boost.org/doc/libs/master/libs/spirit/example/qi/parse_date.cpp
// define custom transformation
namespace boost { namespace spirit { namespace traits {
  template<>
  struct transform_attribute<
      boost::posix_time::time_duration, fusion::vector<unsigned short, unsigned short, unsigned short>, qi::domain>
  {
      typedef fusion::vector<unsigned short, unsigned short, unsigned short> time_parts;

      typedef time_parts type;

      static time_parts pre( boost::posix_time::time_duration ) {
        return time_parts();
      }

      static void post( boost::posix_time::time_duration& td, time_parts const& v )
      {
        td = boost::posix_time::time_duration(
              fusion::at_c<0>(v)
            , fusion::at_c<1>(v)
            , fusion::at_c<2>(v))
            ;
      }

      static void fail( boost::posix_time::time_duration& ) {}
  };
}}}

//std::ostream& operator<<( std::ostream& stream, config::Choices::vPairSettings_t& ps ) { return stream; }

BOOST_FUSION_ADAPT_STRUCT(
  config::Choices::PairSettings,
  (std::string, m_sName)
  (boost::posix_time::time_duration, m_tdStartTime)
  (boost::posix_time::time_duration, m_tdStopTime)
  (std::string, m_sTimeZone)
  (std::uint32_t, m_nTradingAmount)
)

namespace config {

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct ParserPairSettings: qi::grammar<Iterator, Choices::PairSettings()> {
  ParserPairSettings(): ParserPairSettings::base_type( ruleStart ) {

    ruleName %= +( qi::char_ - qi::char_( ',' ) );
    ruleValue %= qi::ushort_;
    ruleTime %= ruleValue >> qi::lit( ':' )
             >> ruleValue >> qi::lit( ':' )
             >> ruleValue
             ;
    ruleTradingAmount %= qi::uint_;
    ruleTimeZone %= +qi::char_("A-Za-z0-9_/") // https://github.com/boost-vault/date_time/blob/master/date_time_zonespec.csv
                 ;
    ruleStart %= ruleName >> qi::lit( ',' )
              >> ruleTime >> qi::lit( ',' )
              >> ruleTime >> qi::lit( ',' )
              >> ruleTimeZone >> qi::lit( ',' )
              >> ruleTradingAmount
              ;

  }

  typedef boost::fusion::vector<unsigned short, unsigned short, unsigned short> time_parts;

  qi::rule<Iterator, std::string()> ruleName;
  qi::rule<Iterator, unsigned short()> ruleValue;
  qi::rule<Iterator, time_parts()> ruleTime;
  qi::rule<Iterator, std::string()> ruleTimeZone;
  qi::rule<Iterator, std::uint32_t()> ruleTradingAmount;
  qi::rule<Iterator, Choices::PairSettings()> ruleStart;
};

void Choices::PairSettings::Parse( const std::string& s ) {
  static ParserPairSettings<std::string::const_iterator> parser;
  bool b = parse( s.begin(), s.end(), parser, *this );
  assert( b );
}

bool Load( const std::string& sFileName, Choices& choices ) {

  bool bOk( true );

  try {

    po::options_description config( "currency trader config" );
    config.add_options()

      ( sChoice_BaseCurrency.c_str(), po::value<std::string>( &choices.m_sBaseCurrency )->default_value( "usd" ), "base currency" )
      ( sChoice_CurrencyTopUp.c_str(), po::value<double>( &choices.m_dblBaseCurrencyTopUp )->default_value( 1000000.00 ), "base currency top up" )

      ( sChoice_PairSetting.c_str(), po::value<Choices::vPairSettings_t>( &choices.m_vPairSettings ), "pair settings <name,start<hh:mm::ss>,stop<hh:mm:ss>,tz>" )

      ( sChoice_sExchange.c_str(), po::value<std::string>( &choices.m_sExchange ), "exchange name" )

      ( sChoice_sHdf5File.c_str(), po::value<std::string>( &choices.m_sHdf5File )->default_value( "TradeFrame.hdf5" ), "hdf5 path/file" )
      ( sChoice_sHdf5SimSet.c_str(), po::value<std::string>( &choices.m_sHdf5SimSet )->default_value( "" ), "hdf5 simulation set" ) // simulate when supplied

      ( sOption_IbInstance.c_str(), po::value<int>( &choices.m_nIbInstance )->default_value( 1 ), "IB instance" )

      ( sChoice_MaxLifeTime.c_str(),   po::value<std::string>( &choices.m_sMaxTradeLifeTime ), "max life time HH:mm:ss" )

      ( sChoice_PipProfit.c_str(), po::value<unsigned int>( &choices.m_nPipProfit ), "pip profit taking" )
      ( sChoice_PipStopLoss.c_str(), po::value<unsigned int>( &choices.m_nPipStopLoss ), "pip stop loss" )
      ( sChoice_PipTrailingStop.c_str(), po::value<unsigned int>( &choices.m_nPipTrailingStop )->default_value( 0 ), "pip trailing stop" )

      ( sChoice_BarSeconds.c_str(), po::value<unsigned int>( &choices.m_nBarSeconds )->default_value( 15 * 60 ), "trading bar width (seconds)" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "currency trader config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      //bOk &= parse<Choices::vPairSettings_t>( sFileName, vm, sChoice_PairSetting, false, choices.m_vPairSettings );
      // move back into the parse with appropriate ostream
      if ( 0 < vm.count( sChoice_PairSetting ) ) {
        choices.m_vPairSettings = std::move( vm[sChoice_PairSetting].as<Choices::vPairSettings_t>() );
        for ( Choices::vPairSettings_t::value_type& vt: choices.m_vPairSettings ) {
          BOOST_LOG_TRIVIAL(info)
            << sChoice_PairSetting << " = "
                   << vt.m_sName
            << ',' << vt.m_tdStartTime
            << ',' << vt.m_tdStopTime
            << ',' << vt.m_sTimeZone
            << ',' << vt.m_nTradingAmount
            ;
        }
      }
      else {
        BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << sChoice_PairSetting << "='";
        bOk = false;
      }

      bOk &= parse<typeof choices.m_sBaseCurrency>( sFileName, vm, sChoice_BaseCurrency, true, choices.m_sBaseCurrency );
      bOk &= parse<typeof choices.m_dblBaseCurrencyTopUp>( sFileName, vm, sChoice_CurrencyTopUp, true, choices.m_dblBaseCurrencyTopUp );

      bOk &= parse<typeof choices.m_sExchange>( sFileName, vm, sChoice_sExchange, true, choices.m_sExchange );

      bOk &= parse<typeof choices.m_sHdf5File>( sFileName, vm, sChoice_sHdf5File, true, choices.m_sHdf5File );
      bOk &= parse<typeof choices.m_sHdf5SimSet>( sFileName, vm, sChoice_sHdf5SimSet, true, choices.m_sHdf5SimSet );

      bOk &= parse<typeof choices.m_nIbInstance>( sFileName, vm, sOption_IbInstance, true, choices.m_nIbInstance );

      bOk &= parse<typeof choices.m_sMaxTradeLifeTime>( sFileName, vm, sChoice_MaxLifeTime, true, choices.m_sMaxTradeLifeTime );
      choices.m_tdMaxTradeLifeTime = boost::posix_time::duration_from_string( choices.m_sMaxTradeLifeTime );

      bOk &= parse<typeof choices.m_nPipProfit>( sFileName, vm, sChoice_PipProfit, true, choices.m_nPipProfit );
      bOk &= parse<typeof choices.m_nPipStopLoss>( sFileName, vm, sChoice_PipStopLoss, true, choices.m_nPipStopLoss );
      bOk &= parse<typeof choices.m_nPipTrailingStop>( sFileName, vm, sChoice_PipTrailingStop, true, choices.m_nPipTrailingStop );

      bOk &= parse<typeof choices.m_nBarSeconds>( sFileName, vm, sChoice_BarSeconds, true, choices.m_nBarSeconds );
    }

  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " config parse error: " << e.what();
    bOk = false;
  }
  catch(...) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " config unknown error";
    bOk = false;
  }

  return bOk;

}

} // namespace config