#pragma once

#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

class CBasketTradeSymbolInfo {
public:
  CBasketTradeSymbolInfo( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );
  ~CBasketTradeSymbolInfo( void );
  void CalculateTrade( ptime dtTradeDate, double dblFunds );
  double GetProposedEntryCost() { return m_dblProposedEntryCost; };
  int GetQuantityForEntry() { return m_nQuantityForEntry; };
protected:
  std::string m_sSymbolName;
  std::string m_sPath;
  std::string m_sStrategy;
  ptime m_dtTradeDate;
  double m_dblMaxAllowedFunds;
  double m_dblDayOpenPrice;
  double m_dblPriceForEntry;
  double m_dblAveragePriceOfEntry;
  double m_dblMarketValueAtEntry;
  double m_dblCurrentMarketPrice;
  double m_dblCurrentMarketValue;
  int m_nQuantityForEntry;
  int m_nWorkingQuantity;
  double m_dblAllocatedWorkingFunds;
  double m_dblExitPrice;
  double m_dblProposedEntryCost;
private:
};
