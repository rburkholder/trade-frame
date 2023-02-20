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
#include <string_view>

#include <iostream>
#include <functional>

#include <boost/json.hpp>

#include <boost/asio/strand.hpp>

#include "one_shot.hpp"
// this needs to be factored out properly
#include "root_certificates.hpp"

#include "Bot.hpp"

namespace json = boost::json;

namespace telegram {

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

// structure Update_Message, part of Update

struct Update_Message {
  uint64_t id;
  Update_Message_From from;
  Update_Message_Chat chat;
  uint64_t date;
  std::string_view svText;
};

Update_Message tag_invoke( json::value_to_tag<Update_Message>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  return Update_Message {
    json::value_to<uint64_t>( obj.at( "message_id" ) ),
    json::value_to<Update_Message_From>( obj.at( "from" ) ),
    json::value_to<Update_Message_Chat>( obj.at( "chat" ) ),
    json::value_to<uint64_t>( obj.at( "date" ) ),
    json::value_to<std::string_view>( obj.at( "text" ) )
  };
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
  std::vector<Update> result;
};

Update_Result tag_invoke( json::value_to_tag<Update_Result>, json::value const& jv ) {
  json::object const& obj = jv.as_object();
  return Update_Result {
    json::value_to<bool>( obj.at( "ok" ) ),
    json::value_to<std::vector<Update> >( obj.at( "result" ) )
  };
}

// class Bot

Bot::Bot( const std::string& sToken )
: m_sToken( sToken )
, m_idChat {}
, m_ssl_context( ssl::context::tlsv12_client )
{
  // This holds the root certificate used for verification
  // NOTE: this needs to be fixed, based upon comments in the include header
  ou::load_root_certificates( m_ssl_context );

  // Verify the remote server's certificate
  m_ssl_context.set_verify_mode( ssl::verify_peer );

  m_pWorkGuard = std::make_unique<work_guard_t>( asio::make_work_guard( m_io ) );
  m_thread = std::move( std::thread( [this](){ m_io.run(); } ) );

  PollUpdates();
}

Bot::~Bot() {
  m_pWorkGuard.reset();
  if ( m_thread.joinable() ) {
    m_thread.join();
  }
}

void Bot::GetMe() {
  if ( m_pWorkGuard ) {
    auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
    request->get(
      "api.telegram.org"
    , "443"
    , m_sToken
    , "getMe"
    , [this]( bool bStatus, const std::string& message ){
        std::cout << message << std::endl;
      }
    );

  }
}

void Bot::PollUpdate( uint64_t offset ) {
  if ( m_pWorkGuard ) {

    json::object UpdateRequest;
    UpdateRequest[ "timeout" ] = 1;
    if ( 0 < offset ) {
      UpdateRequest[ "offset" ] = offset;
    }
    std::string sRequest = json::serialize( UpdateRequest );
    //std::string sRequest( "[\"timeout\":1]" );
    //std::cout << "request='" << sRequest << "'" << std::endl;

    auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
    request->get(
      "api.telegram.org"
    , "443"
    , m_sToken
    , "getUpdates"
    , sRequest
    , [this]( bool bStatus, const std::string& message ){
        if ( bStatus ) {
          //std::cout << "update received: '" << message << "'" << std::endl;
          try {

            json::error_code jec;
            json::value jv = json::parse( message, jec );

            if ( jec.failed() ) {
              std::cerr << "json convert problem: " << jec.what() << std::endl;
            }
            else {
              Update_Result ur( json::value_to<Update_Result>( jv ) );

              if ( ur.bOk ) {
                uint64_t offset {};
                for ( const std::vector<Update>::value_type& vt: ur.result ) {
                  if ( vt.id > offset ) {
                    offset = vt.id;
                    m_idChat = vt.message.chat.id;
                    std::cout
                      << "msg from="
                      << vt.message.from.id
                      << "(" << vt.message.from.svUserName << ")"
                      << ",chat=" << vt.message.chat.id
                      << "(" << vt.message.chat.svUserName << ")"
                      << ",type=" << vt.message.chat.svType
                      << ",text=" << vt.message.svText
                      << std::endl;
                  }
                }

                PollUpdate( ( 0 == offset ) ? 0 : offset + 1 );
              }
            }

          }
          catch( std::invalid_argument ) {
            std::cerr << "PollUpdate invalid argument" << std::endl;
          }
          catch (...) {
            std::cerr << "PollUpdate: unknown issue" << std::endl;
          }

        }
        else {
          std::cerr << "PollUpdate bad status" << std::endl;
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
      std::cout << "telegram send message: no chat id set" << std::endl;
    }
    else {

      json::object UpdateRequest;
      UpdateRequest[ "chat_id" ] = m_idChat;
      UpdateRequest[ "parse_mode" ] = "HTML";
      UpdateRequest[ "text" ] = sMessage;
      std::string sRequest = json::serialize( UpdateRequest );
      //std::cout << "request='" << sRequest << "'" << std::endl;

      auto request = std::make_shared<bot::session::one_shot>( asio::make_strand( m_io ), m_ssl_context );
      request->post(
        "api.telegram.org"
      , "443"
      , m_sToken
      , "sendMessage"
      , sRequest
      , [this]( bool bStatus, const std::string& message ){
          //std::cout << "telegram response: " << message << std::endl;
        }
      );
    }
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