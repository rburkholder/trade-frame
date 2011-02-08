#ifndef eclientsocketbase_h__INCLUDED
#define eclientsocketbase_h__INCLUDED

#include "EClient.h"

#include <memory>
#include <string>
#include <vector>

class EWrapper;

class EClientSocketBase : public EClient
{
public:

	explicit EClientSocketBase(EWrapper *ptr);
	~EClientSocketBase();

	virtual bool eConnect(const char *host, unsigned int port, int clientId=0) = 0;
	virtual void eDisconnect() = 0;

protected:

	void eConnectBase();
	void eDisconnectBase();

public:

	// connection state
	bool isConnected() const;

protected:

	// access to protected variables
	EWrapper * getWrapper() const;
	void setClientId( int clientId);

public:

	bool isInBufferEmpty() const;
	bool isOutBufferEmpty() const;

	// override virtual funcs from EClient
	int serverVersion();
	IBString TwsConnectionTime();
	void reqMktData(TickerId id, const Contract &contract,
		const IBString &genericTicks, bool snapshot);
	void cancelMktData(TickerId id);
	void placeOrder(OrderId id, const Contract &contract, const Order &order);
	void cancelOrder(OrderId id) ;
	void reqOpenOrders();
	void reqAccountUpdates(bool subscribe, const IBString& acctCode);
	void reqExecutions(int reqId, const ExecutionFilter& filter);
	void reqIds(int numIds);
	bool checkMessages();
	void reqContractDetails(int reqId, const Contract &contract);
	void reqMktDepth(TickerId tickerId, const Contract &contract, int numRows);
	void cancelMktDepth(TickerId tickerId);
	void reqNewsBulletins(bool allMsgs);
	void cancelNewsBulletins();
	void setServerLogLevel(int level);
	void reqAutoOpenOrders(bool bAutoBind);
	void reqAllOpenOrders();
	void reqManagedAccts();
	void requestFA(faDataType pFaDataType);
	void replaceFA(faDataType pFaDataType, const IBString& cxml);
	void reqHistoricalData( TickerId id, const Contract &contract,
		const IBString &endDateTime, const IBString &durationStr,
		const IBString & barSizeSetting, const IBString &whatToShow,
		int useRTH, int formatDate);
	void exerciseOptions(TickerId tickerId, const Contract &contract,
		int exerciseAction, int exerciseQuantity,
		const IBString &account, int override);
	void cancelHistoricalData(TickerId tickerId );
	void reqRealTimeBars(TickerId id, const Contract &contract, int barSize,
		const IBString &whatToShow, bool useRTH);
	void cancelRealTimeBars(TickerId tickerId );
	void cancelScannerSubscription(int tickerId);
	void reqScannerParameters();
	void reqScannerSubscription(int tickerId, const ScannerSubscription &subscription);
	void reqCurrentTime();
	void reqFundamentalData(TickerId reqId, const Contract&, const IBString& reportType);
	void cancelFundamentalData(TickerId reqId);
	void calculateImpliedVolatility(TickerId reqId, const Contract &contract, double optionPrice, double underPrice);

private:

	virtual int send(const char* buf, size_t sz) = 0;
	virtual int receive(char* buf, size_t sz) = 0;

protected:

	int sendBufferedData();

private:

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
	static bool DecodeField(IBString&, const char*& ptr, const char* endPtr);

	static bool DecodeFieldMax(int&, const char*& ptr, const char* endPtr);
	static bool DecodeFieldMax(long&, const char*& ptr, const char* endPtr);
	static bool DecodeFieldMax(double&, const char*& ptr, const char* endPtr);

	// encoders
	template<class T> static void EncodeField(std::ostream&, T);

	// "max" encoders
	static void EncodeFieldMax(std::ostream& os, int);
	static void EncodeFieldMax(std::ostream& os, double);

	// socket state
	virtual bool isSocketOK() const = 0;

protected:

	void onConnectBase();

private:

	typedef std::vector<char> BytesVec;

private:

	static void CleanupBuffer(BytesVec&, int processed);

private:

	EWrapper *m_pEWrapper;

	BytesVec m_inBuffer;
	BytesVec m_outBuffer;

	int m_clientId;

	bool m_connected;
	int m_serverVersion;
	IBString m_TwsTime;

};

template<> void EClientSocketBase::EncodeField<bool>(std::ostream& os, bool);
template<> void EClientSocketBase::EncodeField<double>(std::ostream& os, double);

#endif
