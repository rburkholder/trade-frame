/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    Bot.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/Telegram
 * Created: February 17, 2023 18:22:02
 */

#include <string>
#include <thread>

#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <boost/asio/ssl.hpp>

namespace asio = boost::asio; // from <boost/asio/context.hpp>
namespace ssl  = asio::ssl;   // from <boost/asio/ssl.hpp>

namespace telegram {

class Bot {
public:
  Bot( const std::string& sToken );
  ~Bot();

  void SetChatId( uint64_t idChat ) { m_idChat = idChat; };
  uint64_t GetChatId() const { return m_idChat; }

  void GetMe();

  void SendMessage( const std::string& );

  using fCommand_t = std::function<void( const std::string& )>;
  void SetCommand( fCommand_t&& );

protected:
private:

  uint64_t m_idChat;

  const std::string m_sToken;

  std::thread m_thread;

  ssl::context m_ssl_context;
  boost::asio::io_context m_io;

  using work_guard_t = asio::executor_work_guard<boost::asio::io_context::executor_type>;
  using pWorkGuard_t = std::unique_ptr<work_guard_t>;

  pWorkGuard_t m_pWorkGuard;

  fCommand_t m_fCommand;

  void PollUpdate( uint64_t offset );
  void PollUpdates();

  void SetMyCommands();

};

} // namespace telegram
