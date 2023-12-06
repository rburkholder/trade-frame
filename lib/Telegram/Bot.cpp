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

// telegram bot api:  https://core.telegram.org/bots/api#sendmessage
// @BotFather to obtain token: https://core.telegram.org/bots/tutorial

#include <vector>
#include <stdexcept>
#include <functional>
#include <string_view>

#include <boost/json.hpp>
#include <boost/asio/strand.hpp>
#include <boost/log/trivial.hpp>

#include "one_shot.hpp"

#include "root_certificates.hpp" // this needs to be factored out properly

#include "Bot.hpp"

namespace json = boost::json;

namespace telegram {

static const std::string c_sHost( "api.telegram.org" );
static const std::string c_sPort( "443" );

// structure Update_Message_From, part of Update_Message

struct Update_Message_From {
  uint64_t id;
  bool bIsbot;
  std::string_view svFirstName;
  std::string_view svLastName;
  std::string_view svUserName;
  std::string_view svLanguageCode;
};

Update_Message_From tag_invoke( json::value_to_tag<Update_Message_From>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  return Update_Message_From {
    json::value_to<uint64_t>( obj.at( "id" ) ),
    json::value_to<bool>( obj.at( "is_bot" ) ),
    json::value_to<std::string_view>( obj.at( "first_name" ) ),
    json::value_to<std::string_view>( obj.at( "last_name" ) ),
    json::value_to<std::string_view>( obj.at( "username" ) ),
    json::value_to<std::string_view>( obj.at( "language_code" ) )
  };
}

// structure Update_Message_Chat, part of Update_Message

struct Update_Message_Chat {
  uint64_t id;
  std::string_view svFirstName;
  std::string_view svLastName;
  std::string_view svUserName;
  std::string_view svType; // 'private'
};

Update_Message_Chat tag_invoke( json::value_to_tag<Update_Message_Chat>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  return Update_Message_Chat {
    json::value_to<uint64_t>( obj.at( "id" ) ),
    json::value_to<std::string_view>( obj.at( "first_name" ) ),
    json::value_to<std::string_view>( obj.at( "last_name" ) ),
    json::value_to<std::string_view>( obj.at( "username" ) ),
    json::value_to<std::string_view>( obj.at( "type" ) )
  };
}

// structure MessageEntity, part of Update_Message

struct MessageEntity {
  std::string_view svType;
  uint32_t offset;
  uint32_t length;
  std::string_view svUrl; // optional
  //std::string_view svUser; // optional
  //std::string_view svLanguage
};

MessageEntity tag_invoke( json::value_to_tag<MessageEntity>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  return MessageEntity {
    json::value_to<std::string_view>( obj.at( "type" ) ),
    json::value_to<uint32_t>( obj.at( "offset" ) ),
    json::value_to<uint32_t>( obj.at( "length" ) ),
  };
}

// structure Update_Message, part of Update

struct Update_Message {
  uint64_t id;
  Update_Message_From from;
  Update_Message_Chat chat;
  uint64_t date;
  std::string_view svText;
  std::vector<MessageEntity> vMessageEntity;
};

Update_Message tag_invoke( json::value_to_tag<Update_Message>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  Update_Message um {
    json::value_to<uint64_t>( obj.at( "message_id" ) ),
    json::value_to<Update_Message_From>( obj.at( "from" ) ),
    json::value_to<Update_Message_Chat>( obj.at( "chat" ) ),
    json::value_to<uint64_t>( obj.at( "date" ) ),
    json::value_to<std::string_view>( obj.at( "text" ) ),
  };
  if ( obj.contains( "entities" ) ) {
    um.vMessageEntity = json::value_to<std::vector<MessageEntity> >( obj.at( "entities" ) );
  }
  return um;
}

// structure Update, part of Update_Result

struct Update {
  uint64_t id;
  Update_Message message;
};

Update tag_invoke( json::value_to_tag<Update>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  return Update {
    json::value_to<uint64_t>( obj.at( "update_id" ) ),
    json::value_to<Update_Message>( obj.at( "message" ) )
  };
}

// structure Update_Result

struct Update_Result {

  bool bOk;
  std::vector<Update> vResult;

  Update_Result(): bOk( false ) {};
  Update_Result( const std::vector<Update>& vResult_ ): bOk( true ), vResult( vResult_ ) {}
};

Update_Result tag_invoke( json::value_to_tag<Update_Result>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  bool bOk = json::value_to<bool>( obj.at( "ok" ) );
  if ( bOk ) {
    return Update_Result {
      json::value_to<std::vector<Update> >( obj.at( "result" ) )
    };
  }
  else {
    return Update_Result();
  }
}

// class Bot

Bot::Bot( const std::string& sToken )
: m_sToken( sToken )
, m_idChat {}
, m_ssl_context( ssl::context::tlsv12_client )
, m_fCommand( nullptr )
{
  // This holds the root certificate used for verification
  // NOTE: this needs to be fixed, based upon comments in the include header
  ou::load_root_certificates( m_ssl_context );

  // Verify the remote server's certificate
  m_ssl_context.set_verify_mode( ssl::verify_peer );

  m_pWorkGuard = std::make_unique<work_guard_t>( asio::make_work_guard( m_io ) );
  m_thread = std::move( std::thread( [this](){ m_io.run(); } ) );

  PollUpdates();
  SetMyCommands();
}

Bot::~Bot() {
  m_pWorkGuard.reset();
  if ( m_thread.joinable() ) {
    m_thread.join();
  }
}

void Bot::SetCommand( fCommand_t&& f ) {
  m_fCommand = std::move( f );
}

void Bot::GetMe() {
  if ( m_pWorkGuard ) {
    auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
    request->get(
      c_sHost, c_sPort
    , m_sToken
    , "getMe"
    , [this]( bool bStatus, int ec, const std::string& message ){
        BOOST_LOG_TRIVIAL(info) << message;
      }
    );

  }
}

void Bot::PollUpdate( uint64_t offset ) {
  if ( m_pWorkGuard ) {

    json::object UpdateRequest;
    //UpdateRequest[ "timeout" ] = 0; // 0 is default short polling, for testing purposes only https://core.telegram.org/bots/api#sendmessage
    UpdateRequest[ "timeout" ] = 43; // seconds
    if ( 0 < offset ) {
      UpdateRequest[ "offset" ] = offset;
    }
    std::string sRequest = json::serialize( UpdateRequest );
    //BOOST_LOG_TRIVIAL(trace) << "request='" << sRequest << "'";

    auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
    request->get( // https://core.telegram.org/bots/api#getupdates
      c_sHost, c_sPort
    , m_sToken
    , "getUpdates"
    , sRequest
    , [this]( bool bStatus, int ec, const std::string& message ){
        if ( bStatus ) {
          //BOOST_LOG_TRIVIAL(info) << "update received: '" << message << "'";
          try {

            uint64_t offset {};
            json::error_code jec;
            json::value jv = json::parse( message, jec );

            if ( jec.failed() ) {
              BOOST_LOG_TRIVIAL(error) << "json convert problem: " << jec.what();
            }
            else {

              Update_Result ur( json::value_to<Update_Result>( jv ) );
              if ( ur.bOk ) {
                for ( const std::vector<Update>::value_type& vt: ur.vResult ) {
                  if ( vt.id > offset ) {
                    // extract the chat id
                    offset = vt.id;
                    m_idChat = vt.message.chat.id;
                    BOOST_LOG_TRIVIAL(info)
                      << "msg " << vt.id << " from="
                      << vt.message.from.id
                      << "(" << vt.message.from.svUserName << ")"
                      << ",chat=" << vt.message.chat.id
                      << "(" << vt.message.chat.svUserName << ")"
                      << ",type=" << vt.message.chat.svType
                      << ",text=" << vt.message.svText
                      ;

                    // extract any message entities
                    for ( const MessageEntity& me: vt.message.vMessageEntity ) {
                      const std::string s( vt.message.svText.substr( me.offset, me.length ) );
                      if ( "bot_command" == me.svType ) {
                        BOOST_LOG_TRIVIAL(info)
                          << "bot_command=" << s
                          ;
                        try {
                          if ( m_fCommand ) {
                            m_fCommand( s );
                          }
                        }
                        catch(...) {
                          BOOST_LOG_TRIVIAL(error) << "m_fCommand exception";
                        }
                      }
                      else {
                        BOOST_LOG_TRIVIAL(info)
                          << "other entity="
                          << me.svType
                          << "(" << s
                          << ")"
                          //<< ",offset=" << me.offset
                          //<< ",length=" << me.length
                          ;
                      }
                    }
                  }
                  else {
                    BOOST_LOG_TRIVIAL(error) << "Bot::PollUpdate vt.id <= offset," << vt.id << ',' << offset << ',' << message;
                  }
                }
              }
              else {
                // Bot::PollUpdate {"ok":false,"error_code":429,"description":"Too Many Requests: retry after 5","parameters":{"retry_after":5}}
                BOOST_LOG_TRIVIAL(error) << "Bot::PollUpdate bOk false, message: " << message;
                // TODO: track time of last message, how can we be sending too many too fast?
              }

            }
            PollUpdate( ( 0 == offset ) ? 0 : offset + 1 ); // restart, regardless of error, may generate loop (offset is not cleared) if malformed messages?
          }
          catch( const std::invalid_argument& ec ) {
            BOOST_LOG_TRIVIAL(error) << "Bot::PollUpdate std::invalid_argument," << ec.what() << ',' << message;
          }
          catch( const std::exception& ec ) {
            BOOST_LOG_TRIVIAL(error) << "Bot::PollUpdate std::exception," << ec.what() << ',' << message;
          }
          catch(...) {
            BOOST_LOG_TRIVIAL(error) << "Bot::PollUpdate unknown issue," << message;
          }
        }
        else {
          switch ( ec ) {
            case 1: // The socket was closed due to a timeout [as expected]
              PollUpdate( 0 ); // perform another poll
              break;
            default:
              BOOST_LOG_TRIVIAL(error) << "Bot::PollUpdate bad status";
              break;
          }

        }
      }
    );
  }
}

void Bot::PollUpdates() {
  PollUpdate( 0 );
}

void Bot::SendMessage( const std::string& sMessage) {

  if ( m_pWorkGuard ) {
    if ( 0 == m_idChat ) {
      BOOST_LOG_TRIVIAL(warning) << "telegram send message: no chat id set";
    }
    else {

      json::object UpdateRequest;
      UpdateRequest[ "chat_id" ] = m_idChat;
      UpdateRequest[ "parse_mode" ] = "HTML";
      UpdateRequest[ "text" ] = sMessage;
      std::string sRequest = json::serialize( UpdateRequest );
      //BOOST_LOG_TRIVIAL(debug) << "telegram tx: '" << sRequest << "'";
      BOOST_LOG_TRIVIAL(trace) << "telegram tx: " << sRequest;

      auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
      request->post(
        c_sHost, c_sPort
      , m_sToken
      , "sendMessage"
      , sRequest
      , [this]( bool bStatus, int ec, const std::string& message ){
          BOOST_LOG_TRIVIAL(trace) << "telegram rx: " << message;
        }
      );
    }
  }
}

// this isn't actually required, as the update message has anything with '/' decoded as bot_command
void Bot::SetMyCommands() {
  if ( m_pWorkGuard ) {
    json::object BotCommand;
    json::array BotCommandList;

    BotCommand[ "command" ] = "quote";
    BotCommand[ "description" ] = "obtain quote of underlying";
    BotCommandList.emplace_back( BotCommand );

    BotCommand[ "command" ] = "buy";
    BotCommand[ "description" ] = "market buy of underlying";
    BotCommandList.emplace_back( BotCommand );

    BotCommand[ "command" ] = "sell";
    BotCommand[ "description" ] = "market sell of underlying";
    BotCommandList.emplace_back( BotCommand );

    BotCommand[ "command" ] = "position";
    BotCommand[ "description" ] = "statistics of current position";
    BotCommandList.emplace_back( BotCommand );

    json::object BotCommandScope;
    BotCommandScope[ "type" ] = "default";

    json::object Parameters;
    Parameters[ "commands" ] = BotCommandList;
    Parameters[ "scope"] = BotCommandScope;

    std::string sRequest = json::serialize( Parameters );
    //std::cout << "setMyCommands request='" << sRequest << "'" << std::endl;

    auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
    request->post(
      c_sHost, c_sPort
    , m_sToken
    , "setMyCommands"
    , sRequest
    , [this]( bool bStatus, int ec, const std::string& message ){
        //std::cout << "telegram setMyCommands response: " << message << std::endl;
      }
    );
  }
}

} // namespace telegram

/*
'{"ok":true,
  "result":[
    {"update_id":771878720,
      "message":
        {"message_id":2,
          "from":{
              "id":5467345437,
              "is_bot":false,
              "first_name":"Raymond",
              "last_name":"Burkholder",
              "username":"OneUnified",
              "language_code":"en"
              },
          "chat":{
            "id":5467345437,
            "first_name":"Raymond",
            "last_name":"Burkholder",
            "username":"OneUnified",
            "type":"private"
            },
          "date":1676791780,
          "text":"Test"
          }
        }
      ]
    }
'

*/
