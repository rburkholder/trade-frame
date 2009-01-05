#pragma once

#include <string>
//#include "Singleton.h"
#include "ManagerBase.h"
#include "ProviderInterface.h"

class CProviderManager: public ManagerBase<CProviderManager, std::string, CProviderInterface> {
public:
  CProviderManager(void);
  virtual ~CProviderManager(void);
protected:
private:
};
