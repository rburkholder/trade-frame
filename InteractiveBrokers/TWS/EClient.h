#ifndef eclient_def
#define eclient_def

#include "CommonDefs.h"

struct Contract;
struct Order;
struct ExecutionFilter;
struct ScannerSubscription;

class EClient
{
public:
   virtual ~EClient() {}
   virtual bool eConnect( const char *host, UINT port, int clientId=0) = 0;
   virtual void eDisconnect() = 0;
   virtual int serverVersion() = 0;
   virtual CString TwsConnectionTime() = 0;
   virtual void reqMktData( TickerId id, const Contract &contract,
	   const CString& genericTicks, bool snapshot) = 0;
   virtual void cancelMktData( TickerId id) = 0;
   virtual void placeOrder( OrderId id, const Contract &contract, const Order &order) = 0;
   virtual void cancelOrder( OrderId id) = 0;
   virtual void reqOpenOrders() = 0;
   virtual void reqAccountUpdates(bool subscribe, const CString& acctCode) = 0;
   virtual void reqExecutions(const ExecutionFilter& filter) = 0;
   virtual void reqIds( int numIds) = 0;
   virtual void checkMessages() = 0;
   virtual void reqContractDetails( const Contract &contract) = 0;
   virtual void reqMktDepth( TickerId id, const Contract &contract, int numRows) = 0;
   virtual void cancelMktDepth( TickerId id) = 0;
   virtual void reqNewsBulletins( bool allMsgs) = 0;
   virtual void cancelNewsBulletins() = 0;
   virtual void setServerLogLevel(int level) = 0;
   virtual void reqAutoOpenOrders(bool bAutoBind) = 0;
   virtual void reqAllOpenOrders() = 0;
   virtual void reqManagedAccts() = 0;
   virtual void requestFA(faDataType pFaDataType) = 0;
   virtual void replaceFA(faDataType pFaDataType, const CString& cxml) = 0;
   virtual void reqHistoricalData( TickerId id, const Contract &contract, 
	   const CString &endDateTime, const CString &durationStr, const CString &barSizeSetting,
	   const CString &whatToShow, int useRTH, int formatDate) = 0;
   virtual void exerciseOptions( TickerId id, const Contract &contract,
       int exerciseAction, int exerciseQuantity, const CString &account, int override) = 0;
   virtual void cancelHistoricalData( TickerId tickerId ) = 0;
   virtual void reqRealTimeBars( TickerId id, const Contract &contract, int barSize,
	   const CString &whatToShow, bool useRTH) = 0;
   virtual void cancelRealTimeBars( TickerId tickerId) = 0;
   virtual void cancelScannerSubscription( int tickerId) = 0;
   virtual void reqScannerParameters() = 0;
   virtual void reqScannerSubscription( int tickerId, const ScannerSubscription &subscription) = 0;
   virtual void reqCurrentTime() = 0;
private:
};

#endif
