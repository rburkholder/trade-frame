#pragma once

#include <string>
#include <map>

//#include "Singleton.h"
#include "ManagerBase.h"
#include "Portfolio.h"

class CPortfolioManager: public ManagerBase<CPortfolioManager, std::string, CPortfolio> {
public:
  CPortfolioManager(void);
  virtual ~CPortfolioManager(void);

  void Add( const std::string &sName );
  void Delete( const std::string &sName );
  CPortfolio *GetPortfolio( const std::string &sName );
protected:
private:
};
