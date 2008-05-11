#pragma once

#include <map>
#include "DataRow.h"
#include "FastDelegate.h"

using namespace std;

class CDataRows {
public:
  CDataRows(void);
  ~CDataRows(void);

  void SetMultiplier( int multiplier ) { m_Multiplier = multiplier; };
  int GetMultiplier() { return m_Multiplier; };

  CDataRow *operator[]( unsigned int ); // Price * 100 or Price * 1000

  typedef FastDelegate1<CDataRow*> OnRowInsertHandler;
  void SetOnRowInsert( OnRowInsertHandler function ) {
    OnRowInsert = function;
  }
protected:
private:
  int m_Multiplier;
  std::map<unsigned int, CDataRow*> m_mpDataRow; // sparse array
  typedef std::pair <unsigned int, CDataRow*> m_Pair;
  std::map<unsigned int, CDataRow*> ::const_iterator m_Iterator;

  CDataRow *Insert( unsigned int ix );

  OnRowInsertHandler OnRowInsert;

  CRITICAL_SECTION csEntry;
  int nEntry;
};
