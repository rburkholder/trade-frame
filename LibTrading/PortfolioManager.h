#pragma once

#include <string>
#include <map>

#include "Portfolio.h"

class CPortfolioManager {
public:
  CPortfolioManager(void);
  ~CPortfolioManager(void);

  void Add( const std::string &sName );
  void Delete( const std::string &sName );
  CPortfolio *GetPortfolio( const std::string &sName );
protected:
private:
};
