#pragma once

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

// ./bjam --toolset=msvc variant=debug,release threading=multi link=static define=BOOST_LOG_USE_CHAR --with-log  stage
// boost-log/boost goes into boost
// boost-log/lib goes in lib

#define LOG BOOST_LOG(CLog::Log())

class CLog {
public:
  CLog(void);
  ~CLog(void);
  static src::logger &Log( void ) { return lg; };
protected:
  typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
  static boost::shared_ptr<text_sink> m_pSink;
  static src::logger lg;
private:
  static int m_cntInstances;
};
