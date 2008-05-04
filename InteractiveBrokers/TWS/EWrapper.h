#ifndef ewrapper_def
#define ewrapper_def

#include "CommonDefs.h"

enum TickType { BID_SIZE, BID, ASK, ASK_SIZE, LAST, LAST_SIZE,
				HIGH, LOW, VOLUME, CLOSE,
				BID_OPTION_COMPUTATION, 
				ASK_OPTION_COMPUTATION, 
				LAST_OPTION_COMPUTATION,
				MODEL_OPTION,
				OPEN,
				LOW_13_WEEK,
				HIGH_13_WEEK,
				LOW_26_WEEK,
				HIGH_26_WEEK,
				LOW_52_WEEK,
				HIGH_52_WEEK,
				AVG_VOLUME,
				OPEN_INTEREST,
				OPTION_HISTORICAL_VOL,
				OPTION_IMPLIED_VOL,
				OPTION_BID_EXCH,
				OPTION_ASK_EXCH,
				OPTION_CALL_OPEN_INTEREST,
				OPTION_PUT_OPEN_INTEREST,
				OPTION_CALL_VOLUME,
				OPTION_PUT_VOLUME,
				INDEX_FUTURE_PREMIUM,
				BID_EXCH,
				ASK_EXCH,
				AUCTION_VOLUME,
				AUCTION_PRICE,
				AUCTION_IMBALANCE,
				MARK_PRICE,
				BID_EFP_COMPUTATION,
				ASK_EFP_COMPUTATION,
				LAST_EFP_COMPUTATION,
				OPEN_EFP_COMPUTATION,
				HIGH_EFP_COMPUTATION,
				LOW_EFP_COMPUTATION,
				CLOSE_EFP_COMPUTATION,
				LAST_TIMESTAMP,
				SHORTABLE,
				NOT_SET };

inline bool isPrice( TickType tickType) {
	return tickType == BID || tickType == ASK || tickType == LAST;
}

struct Contract;
struct ContractDetails;
struct Order;
struct OrderState;
struct Execution;

class EWrapper
{
public:
   virtual void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute) = 0;
   virtual void tickSize( TickerId tickerId, TickType field, int size) = 0;
   virtual void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double modelPrice, double pvDividend) = 0;
   virtual void tickGeneric(TickerId tickerId, TickType tickType, double value) = 0;
   virtual void tickString(TickerId tickerId, TickType tickType, const CString& value) = 0;
   virtual void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const CString& formattedBasisPoints,
	   double totalDividends, int holdDays, const CString& futureExpiry, double dividendImpact, double dividendsToExpiry) = 0;
   virtual void orderStatus( OrderId orderId, const CString &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const CString& whyHeld) = 0;
   virtual void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&) = 0;
   virtual void winError( const CString &str, int lastError) = 0;
   virtual void connectionClosed() = 0;
   virtual void updateAccountValue(const CString& key, const CString& val,
   const CString& currency, const CString& accountName) = 0;
   virtual void updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const CString& accountName) = 0;
   virtual void updateAccountTime(const CString& timeStamp) = 0;
   virtual void nextValidId( OrderId orderId) = 0;
   virtual void contractDetails( const ContractDetails& contractDetails) = 0;
   virtual void bondContractDetails( const ContractDetails& contractDetails) = 0;
   virtual void execDetails( OrderId orderId, const Contract& contract, const Execution& execution) =0;
   virtual void error(const int id, const int errorCode, const CString errorString) = 0;
   virtual void updateMktDepth(TickerId id, int position, int operation, int side,
      double price, int size) = 0;
   virtual void updateMktDepthL2(TickerId id, int position, CString marketMaker, int operation,
      int side, double price, int size) = 0;
   virtual void updateNewsBulletin(int msgId, int msgType, const CString& newsMessage, const CString& originExch) = 0;
   virtual void managedAccounts( const CString& accountsList) = 0;
   virtual void receiveFA(faDataType pFaDataType, const CString& cxml) = 0;
   virtual void historicalData(TickerId reqId, const CString& date, double open, double high, 
	   double low, double close, int volume, int barCount, double WAP, int hasGaps) = 0;
   virtual void scannerParameters(const CString &xml) = 0;
   virtual void scannerData(int reqId, int rank, const ContractDetails &contractDetails,
	   const CString &distance, const CString &benchmark, const CString &projection,
	   const CString &legsStr) = 0;
   virtual void scannerDataEnd(int reqId) = 0;
   virtual void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	   long volume, double wap, int count) = 0;
   virtual void currentTime(long time) = 0;
};


#endif
