#pragma once

//#include "ProviderInterface.h"
#include "TradingEnumerations.h"
#include "Instrument.h"
#include "PersistedOrderId.h"
#include "Execution.h"

#include "Delegate.h"

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <string>

class COrder {
public:
  COrder(  // market 
    CInstrument *instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit or stop
    CInstrument *instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    ptime dtOrderSubmitted = not_a_date_time
    );
  COrder(  // limit and stop
    CInstrument *instrument, // not deleted here, need a smart pointer
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,  
    double dblPrice2,
    ptime dtOrderSubmitted = not_a_date_time
    );
  virtual ~COrder(void);
  void SetOutsideRTH( bool bOutsideRTH ) { m_bOutsideRTH = bOutsideRTH; };
  bool GetOutsideRTH( void ) const { return m_bOutsideRTH; };
  CInstrument *GetInstrument( void ) const { return m_pInstrument; };
  const char *GetOrderSideName( void ) const { return OrderSide::Name[ m_eOrderSide ]; };
  unsigned long GetQuantity( void ) const { return m_nOrderQuantity; };
  OrderType::enumOrderType GetOrderType( void ) const { return m_eOrderType; };
  OrderSide::enumOrderSide GetOrderSide( void ) const { return m_eOrderSide; };
  double GetPrice1( void ) const { return m_dblPrice1; };  // need to validate this on creation
  double GetPrice2( void ) const { return m_dblPrice2; };
  double GetAverageFillPrice( void ) const { return m_dblAverageFillPrice; };
  unsigned long GetOrderId( void ) const { return m_nOrderId; };
  void SetProviderName( const std::string &sName ) { m_sProviderName = sName; };
  const std::string &GetProviderName( void ) const { return m_sProviderName; };
  unsigned long GetNextExecutionId( void ) { return m_nNextExecutionId++; };
  void SetSendingToProvider( void );
  OrderStatus::enumOrderStatus ReportExecution( const CExecution &exec ); // report true when complete
  Delegate<COrder *> OnExecution;
  Delegate<COrder *> OnOrderFilled; // on final fill
  Delegate<COrder *> OnPartialFill; // on intermediate fills only
  void SetCommission( double dblCommission ) { m_dblCommission = dblCommission; };
  void ActOnError( OrderErrors::enumOrderErrors eError );
  unsigned long GetQuanRemaining( void ) { return m_nRemaining; };
  unsigned long GetQuanOrdered( void ) { return m_nOrderQuantity; };
  unsigned long GetQuanFilled( void ) { return m_nFilled; };
  void SetSignalPrice( double dblSignalPrice ) { m_dblSignalPrice = dblSignalPrice; };
  double GetSignalPrice( void ) { return m_dblSignalPrice; };
protected:
  //std::string m_sSymbol;
  std::string m_sProviderName;
  CInstrument *m_pInstrument;
  unsigned long m_nOrderId;
  OrderType::enumOrderType m_eOrderType;
  OrderSide::enumOrderSide m_eOrderSide;
  unsigned long m_nOrderQuantity;
  //unsigned long m_nOrderQuantityRemaining;
  double m_dblPrice1;  // for limit
  double m_dblPrice2;  // for stop
  double m_dblSignalPrice;  // mark at which algorithm requested order

  //CProviderInterface *m_pProvider;  // associated provider
  ptime m_dtOrderSubmitted;
  ptime m_dtOrderCancelled;
  ptime m_dtOrderFilled;

  bool m_bOutsideRTH;

  OrderStatus::enumOrderStatus m_eOrderStatus;
  unsigned long m_nNextExecutionId;

  CPersistedOrderId m_persistedorderid;  // make this static?
  void AssignOrderId( void );

  // statistics and status
  //unsigned long m_nTotalOrdered;
  unsigned long m_nFilled;
  unsigned long m_nRemaining;
  double m_dblCommission;
  double m_dblPriceXQuantity; // used for calculating average price
  double m_dblAverageFillPrice;
  //double m_dblAverageFillPriceWithCommission;
private:
  COrder(void);
};
