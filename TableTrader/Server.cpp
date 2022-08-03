/*
 * File:      Server.cpp
 * Author:    raymond@burkholder.net
 * copyright: 2018 Raymond Burkholder
 * License:   GPL3
 *
 * Created on September 22, 2018, 12:30 PM
 */

// 2018/10/03 todo: instantiate uuid generator, and external to other modules
//    static boost::uuids::random_generator uuidGenerator;

#include "Server.h"

//Server::Server(
//  const std::string &wtApplicationPath,
//  const std::string &wtConfigurationFile
//)
//: Wt::WServer( wtApplicationPath, wtConfigurationFile)
//{
//}

Server::Server(
  int argc,
  char *argv[],
  const config::Choices& choices,
  const std::string &wtConfigurationFile
)
: Wt::WServer( argc, argv, wtConfigurationFile )
, m_choices( choices )
{
}

Server::~Server() {
}

bool Server::ValidateLogin( const std::string& sUserName, const std::string& sPassWord ) {
  return ( ( sUserName == m_choices.m_sUIUserName ) && ( sPassWord == m_choices.m_sUIPassWord ) );
}

void Server::AddUnderlyingFutures( fAddUnderlyingFutures_t&& f ) {
  for ( const std::string& s: m_choices.m_vUnderlyingFuture ) {
    f( s );
  }
}