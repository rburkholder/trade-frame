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
 * File:    FileNotify.cpp
 * Author:  raymond@burkholder.net
 * Project: TradeFrame/MarketTrader
 * Created: 2024/12/15 12:12:40
 * Based on:Apparition
 */

#include <cerrno>
#include <stdexcept>
#include <unistd.h>

#include <sys/inotify.h>

#include <assert.h>

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include "FileNotify.hpp"

namespace {
  const size_t c_size_inotify_event ( sizeof( inotify_event ) );
  const size_t c_size_inotify_buffer ( 1024 * ( c_size_inotify_event + 16 ) );
}

// https://www.linuxjournal.com/article/8478
// https://www.man7.org/linux/man-pages/man7/inotify.7.html

namespace ou {

FileNotify::FileNotify()
: m_fdINotify( 0 )
, m_bActive( false )
{

  m_fdINotify = inotify_init();
  if ( 0 > m_fdINotify ) {
    m_fdINotify = 0;
    throw std::runtime_error( "inotify_init error " + boost::lexical_cast<std::string>( m_fdINotify ) );
  }

  m_bActive = true;

  m_threadINotify = std::thread(
    [this](){

      char bufINotify[ c_size_inotify_buffer ];

      while ( m_bActive ) {

        timeval time;
        time.tv_sec = 1;
        time.tv_usec = 0;

        fd_set rfds;
        FD_ZERO ( &rfds );
        FD_SET ( m_fdINotify, &rfds );

        // TODO: convert to poll or epoll
        int result = select( m_fdINotify + 1, &rfds, NULL, NULL, &time );
        if ( 0 > result ) {
          BOOST_LOG_TRIVIAL(error) << "select error " << result;
        }
        else {
          if ( 0 == result ) {
            //BOOST_LOG_TRIVIAL(info) << "select time out " << result;  // probably not an error, just start loop again
          }
          else {
            if ( FD_ISSET ( m_fdINotify, &rfds ) ) {
              const int length = read( m_fdINotify, bufINotify, c_size_inotify_buffer );
              if ( 0 > length ) {
                if ( EINTR == errno ) {
                  BOOST_LOG_TRIVIAL(info) << "EINTR == errno " << result;  // probably not an error, just start loop again
                }
                else {
                  BOOST_LOG_TRIVIAL(error) << "read length error " << length << "," << errno;
                }
              }
              else {
                if ( 0 == length ) {
                  BOOST_LOG_TRIVIAL(error) << "read length is zero";
                }
                else {
                  int ix {};
                  while ( length > ix ) {

                    inotify_event *event;
                    event = (inotify_event*) &bufINotify[ ix ];

                    EType type = EType::unknown_;

                    if ( 0 < ( IN_CREATE & event->mask ) ) {
                      type = EType::create_;
                    }
                    if ( 0 < ( IN_DELETE & event->mask ) ) {
                      type = EType::delete_;
                    }
                    if ( 0 < ( IN_MODIFY & event->mask ) ) {
                      type = EType::modify_;
                    }
                    if ( 0 < ( IN_DELETE_SELF & event->mask ) ) {
                      type = EType::delete_;
                    }
                    if ( 0 < ( IN_MOVED_FROM & event->mask ) ) {
                      type = EType::move_from_;
                    }
                    if ( 0 < ( IN_MOVED_TO & event->mask ) ) {
                      type = EType::move_to_;
                    }
                    assert( EType::unknown_ != type );

                    std::string sFile;
                    if ( 0 < event->len ) {
                      sFile = std::string( event->name ); // auto finds nulls at end
                      mapWatchNotify_t::iterator iter = m_mapWatchNotify.find( event->wd );
                      assert( m_mapWatchNotify.end() != iter );
                      assert( iter->second );
                      iter->second( type, sFile );
                    }

                    ix += c_size_inotify_event + event->len;
                  }
                }
              }

            }
            else {
              BOOST_LOG_TRIVIAL(warning) << "select has nothing set";
            }
          }
        }
      } // while

    } );

}

FileNotify::~FileNotify() {
  Close();
}

void FileNotify::AddWatch( const std::string& sPath, fNotify_t&& fNotify ) {

  assert( fNotify );

  mapPathWatch_t::iterator iterPathWatch = m_mapPathWatch.find( sPath );
  if ( m_mapPathWatch.end() == iterPathWatch ) {

    {
      auto result = m_mapPathWatch.emplace( sPath, 0 ); // wdFile to filled
      assert( result.second );
      iterPathWatch = result.first;
    }

    int wdFile  = inotify_add_watch(
      m_fdINotify,
      sPath.c_str(),
      IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVE
      );

    if ( 0 > wdFile ) {
      m_mapPathWatch.erase( iterPathWatch );
      throw std::runtime_error( "inotify_add_watch error for " + sPath + ' ' + boost::lexical_cast<std::string>( m_fdINotify ) );
    }

    iterPathWatch->second = wdFile;

    mapWatchNotify_t::iterator iterWatchNotify = m_mapWatchNotify.find( wdFile );
    assert( m_mapWatchNotify.end() == iterWatchNotify );
    {
      auto result = m_mapWatchNotify.emplace( wdFile, std::move( fNotify ) );
      assert( result.second );
    }
  }
  else {
    throw std::runtime_error( "inotify_add_watch error, watch exists " + sPath );
  }
}

void FileNotify::DelWatch( const std::string& sPath ) {
  assert( 0 < sPath.size() );
  mapPathWatch_t::iterator iterPathWatch = m_mapPathWatch.find( sPath );
  assert( m_mapPathWatch.end() != iterPathWatch );
  int wdFile = iterPathWatch->second;
  m_mapPathWatch.erase( iterPathWatch );
  assert( 0 < wdFile );

  int result = inotify_rm_watch ( m_fdINotify, wdFile );
  assert( 0 == result );
  mapWatchNotify_t::iterator iterWatchNotify = m_mapWatchNotify.find( wdFile );
  assert( m_mapWatchNotify.end() != iterWatchNotify );
  iterWatchNotify->second = nullptr;
  m_mapWatchNotify.erase( iterWatchNotify );
}

void FileNotify::Close() {

  m_bActive = false;

  if ( m_threadINotify.joinable() ) {
    m_threadINotify.join();
  }

  assert( m_mapWatchNotify.size() == m_mapPathWatch.size() );

  while ( !m_mapPathWatch.empty() ) {
    const std::string& sPath( m_mapPathWatch.begin()->first );
    DelWatch( sPath );
  }

  int result;
  if ( 0 < m_fdINotify ) {
    result = close( m_fdINotify );
    assert( 0 == result );
    m_fdINotify = 0;
  }

}

} // namespace ou