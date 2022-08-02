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

Server::Server(
  const std::string &wtApplicationPath,
  const std::string &wtConfigurationFile
)
: Wt::WServer( wtApplicationPath, wtConfigurationFile)
{
}

Server::Server(
  int argc,
  char *argv[],
  const std::string &wtConfigurationFile
)
: Wt::WServer( argc, argv, wtConfigurationFile )
{
}

Server::~Server() {
}

