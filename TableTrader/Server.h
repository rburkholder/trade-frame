/*
 * File:      Server.h
 * Author:    raymond@burkholder.net
 * copyright: 2018 Raymond Burkholder
 * License:   GPL3
 *
 * Created on September 22, 2018, 12:30 PM
 */

#ifndef SERVER_H
#define SERVER_H

#include <Wt/WServer.h>

#include "Config.hpp"

class Server: public Wt::WServer {
public:
  //Server(
  //  const std::string &wtApplicationPath=std::string(),
  //  const std::string &wtConfigurationFile=std::string()
  //  );
  Server(
    int argc,
    char *argv[],
    const config::Choices&,
    const std::string &wtConfigurationFile=std::string()
    );
  virtual ~Server();

  bool ValidateLogin( const std::string& sUserName, const std::string& sPassWord );

protected:
private:
  const config::Choices& m_choices;
};

#endif /* SERVER_H */

