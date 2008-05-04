#ifndef eclientsocket_def
#define eclientsocket_def

#ifndef DLLEXP
#define DLLEXP
#endif

#include "EWrapper.h"
#include "EClient.h"

#include <string>
#include <vector>
#include <memory>

class EWrapper;
class CSocket;
class CSocketFile;
class MySocket;

class DLLEXP EClientSocket : public EClient
{
friend class MySocket;
public:
   EClientSocket( EWrapper *ptr);
   ~EClientSocket();

   // override virtual funcs from EClient

   bool eConnect( const char *host, UINT port, int clientId=0);
   void eDisconnect();
   int serverVersion();
   CString TwsConnectionTime();
   void reqMktData( TickerId id, const Contract &contract,
	   const CString &genericTicks, bool snapshot);
   void cancelMktData( TickerId id);
   void placeOrder( OrderId id, const Contract &contract, const Order &order);
   void cancelOrder( OrderId id) ;
   void reqOpenOrders();
   void reqAccountUpdates(bool subscribe, const CString& acctCode);
   void reqExecutions(const ExecutionFilter& filter);
   void reqIds( int numIds);
   void checkMessages();
   void reqContractDetails( const Contract &contract);
   void reqMktDepth( TickerId tickerId, const Contract &contract, int numRows);
   void cancelMktDepth( TickerId tickerId);
   void reqNewsBulletins(bool allMsgs);
   void cancelNewsBulletins();
   void setServerLogLevel(int level);
   void reqAutoOpenOrders(bool bAutoBind);
   void reqAllOpenOrders();
   void reqManagedAccts();
   void requestFA(faDataType pFaDataType);
   void replaceFA(faDataType pFaDataType, const CString& cxml);
   void reqHistoricalData( TickerId id, const Contract &contract, 
	   const CString &endDateTime, const CString &durationStr,
	   const CString & barSizeSetting, const CString &whatToShow,
	   int useRTH, int formatDate);
   void exerciseOptions( TickerId tickerId, const Contract &contract,
                         int exerciseAction, int exerciseQuantity,
                         const CString &account, int override);
   void cancelHistoricalData( TickerId tickerId );
   void reqRealTimeBars( TickerId id, const Contract &contract, int barSize,
	   const CString &whatToShow, bool useRTH);
   void cancelRealTimeBars( TickerId tickerId );
   void cancelScannerSubscription( int tickerId);
   void reqScannerParameters();
   void reqScannerSubscription( int tickerId, const ScannerSubscription &subscription);
   void reqCurrentTime();

private:

	int send(const char* buf, size_t sz);
	int sendBufferedData();
	int bufferedSend(const char* buf, size_t sz);
	int bufferedSend(const std::string& msg);

	// read and buffer what's available
	int bufferedRead();

	// try to process connection request ack
	int processConnectAck(const char*& ptr, const char* endPtr);

	// try to process single msg
	int processMsg(const char*& ptr, const char* endPtr);

	static bool CheckOffset(const char* ptr, const char* endPtr);
	static const char* FindFieldEnd(const char* ptr, const char* endPtr);

	// decoders
	static bool DecodeField(bool&, const char*& ptr, const char* endPtr);
	static bool DecodeField(int&, const char*& ptr, const char* endPtr);
	static bool DecodeField(long&, const char*& ptr, const char* endPtr);
	static bool DecodeField(double&, const char*& ptr, const char* endPtr);
	static bool DecodeField(CString&, const char*& ptr, const char* endPtr);

	static bool DecodeFieldMax(int&, const char*& ptr, const char* endPtr);
	static bool DecodeFieldMax(long&, const char*& ptr, const char* endPtr);
	static bool DecodeFieldMax(double&, const char*& ptr, const char* endPtr);

	// encoders
	template<class T>
	static void EncodeField(std::ostream&, T);

	template<>
	static void EncodeField<bool>(std::ostream& os, bool);

	// "max" encoders
	static void EncodeFieldMax(std::ostream& os, int);
	static void EncodeFieldMax(std::ostream& os, double);

	// callback from socket
	void onConnect(int i);
	void onReceive(int i);
	void onSend(int i);
	void onClose(int i);

	// helper
	bool handleSocketError(int i);

private:

	typedef std::vector<char> BytesVec;

private:

	static void CleanupBuffer(BytesVec&, int processed);

private:

	EWrapper *m_pEWrapper;
	std::auto_ptr<MySocket> m_pSocket;

	BytesVec m_inBuffer;
	BytesVec m_outBuffer;

	int m_clientId;

	bool m_connected;
	int m_serverVersion;
	CString m_TwsTime;
};

#endif
