#include "StdAfx.h"
#include "assert.h"
#include "Contract.h"

CContract::CContract(ContractType::enumContractTypes type)
: m_ContractType( type )
{
  assert( type < ContractType::_Count );
  assert( type >= ContractType::Unknown );
}

CContract::~CContract(void) {
}
