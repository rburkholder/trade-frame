#pragma once

#include "TradingEnumerations.h"

class CContract {
public:
  CContract( ContractType::enumContractTypes type = ContractType::Unknown );
  virtual ~CContract(void);
protected:
  ContractType::enumContractTypes m_ContractType;
private:
};
