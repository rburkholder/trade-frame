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

#pragma once

// obtain logging library from 
// http://www.boostpro.com/vault/  //vrc3 is the one to use, svn changes lots and lots of stuff
// http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Boost.Logging
//  http://boost-log.sourceforge.net/libs/log/doc/html/index.html
//  http://boost-log.svn.sourceforge.net/viewvc/boost-log/trunk/

//#include <boost/mpl/identity.hpp>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>

#include <boost/log/sources/basic_logger.hpp>

#include <boost/log/sinks/sink.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

#include <boost/log/utility/empty_deleter.hpp>

#include <boost/log/formatters/ostream.hpp>

//#include <boost/log/sources/threading_models.hpp>
//#include <boost/log/formatters/basic_formatters.hpp>
#include <boost/log/filters/basic_filters.hpp>
//#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/basic_attribute_value.hpp>
//#include <boost/log/filters/attr.hpp>
//#include <boost/log/formatters/attr.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace attrs = boost::log::attributes;

// ./bjam --toolset=msvc --layout=tagged variant=debug,release threading=multi link=static define=BOOST_LOG_USE_CHAR runtime-link=static --with-log  stage
// ..\boost-jam-3.1.17-1-ntx86\bjam.exe --toolset=msvc --layout=tagged variant=debug,rlease threading=multi  link=static  define=BOOST_LOG_USE_CHAR --with-log  stage
// ..\boost-jam-3.1.17-1-ntx86\bjam.exe --toolset=msvc --layout=tagged variant=debug,rlease threading=multi  link=static  runtime-link=static define=BOOST_LOG_USE_CHAR --with-log  stage
// boost-log/boost goes into boost
// boost-log/lib goes in lib

//#define LOG(LG) if (LG.open_record()) LG.strm()
//! The macro writes a record to the log
//#define BOOST_LOG(logger)\
//    if (!(logger).open_record())\
//        ((void)0);\
//    else\
//        (logger).strm()
#define LOG BOOST_LOG(CLog::Log())


class CLog {
public:
  CLog(void);
  ~CLog(void);
  static src::logger &Log( void ) { return lg; };
protected:
  typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
  static boost::shared_ptr<text_sink> m_pSink;
  //static src::logger_mt lg; 
  static src::logger lg; 
private:
  static int m_cntInstances;
};


