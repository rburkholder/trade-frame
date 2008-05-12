#pragma once
//#include "k:\data\projects\tradingsoftware\interactivebrokers\tws\ewrapper.h"
//#include "k:\data\projects\tradingsoftware\interactivebrokers\tws\ewrapper.h"
#include "TWS\EClientSocket.h"
#include "ProviderInterface.h"

#include <string>

class CIBTWS : public EWrapper, public CProviderInterface {
public:
  CIBTWS( const string &acctCode, const string &address = "127.0.0.1", UINT port = 7496 );
  ~CIBTWS(void);
  
  //void Start( void );

  // From ProviderInterface:
  virtual void Connect( void );
  virtual void Disconnect( void );

  // From TWS Wrapper:
  virtual void connectionClosed();
  virtual void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute);
  virtual void tickSize( TickerId tickerId, TickType field, int size);
  virtual void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double modelPrice, double pvDividend);
  virtual void tickGeneric(TickerId tickerId, TickType tickType, double value);
  virtual void tickString(TickerId tickerId, TickType tickType, const CString& value);
  virtual void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const CString& formattedBasisPoints,
	   double totalDividends, int holdDays, const CString& futureExpiry, double dividendImpact, double dividendsToExpiry);
   virtual void orderStatus( OrderId orderId, const CString &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const CString& whyHeld);
  virtual void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&);
  virtual void execDetails( OrderId orderId, const Contract& contract, const Execution& execution);
  virtual void error(const int id, const int errorCode, const CString errorString);
  virtual void winError( const CString &str, int lastError);
  virtual void updateNewsBulletin(int msgId, int msgType, const CString& newsMessage, const CString& originExch);
  virtual void currentTime(long time);
  virtual void contractDetails( const ContractDetails& contractDetails);
  virtual void bondContractDetails( const ContractDetails& contractDetails);
  virtual void nextValidId( OrderId orderId);
  virtual void updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const CString& accountName);
  virtual void updateAccountValue(const CString& key, const CString& val,
   const CString& currency, const CString& accountName);
  virtual void updateAccountTime(const CString& timeStamp);
  virtual void updateMktDepth(TickerId id, int position, int operation, int side,
      double price, int size);
  virtual void updateMktDepthL2(TickerId id, int position, CString marketMaker, int operation,
      int side, double price, int size);
  virtual void managedAccounts( const CString& accountsList);
     virtual void receiveFA(faDataType pFaDataType, const CString& cxml);
   virtual void historicalData(TickerId reqId, const CString& date, double open, double high, 
	   double low, double close, int volume, int barCount, double WAP, int hasGaps);
   virtual void scannerParameters(const CString &xml);
   virtual void scannerData(int reqId, int rank, const ContractDetails &contractDetails,
	   const CString &distance, const CString &benchmark, const CString &projection,
	   const CString &legsStr);
   virtual void scannerDataEnd(int reqId);
   virtual void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	   long volume, double wap, int count);

protected:
  std::string m_sAccountCode;
  std::string m_sIPAddress;
  UINT m_nPort;
  TickerId m_curTickerId;
private:
  EClientSocket *pTWS;
};
