/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef eclientsocketbase_h__INCLUDED
#define eclientsocketbase_h__INCLUDED

#include "EClient.h"

#include <memory>
#include <string>
#include <vector>
#include <iosfwd>

class EWrapper;

class TWSAPIDLLEXP EClientSocketBase : public EClient
{
public:

	explicit EClientSocketBase(EWrapper *ptr);
	~EClientSocketBase();

	virtual bool eConnect(const char *host, unsigned int port, int clientId = 0, bool extraAuth = false) = 0;
	virtual void eDisconnect() = 0;

	int clientId() const { return m_clientId; }

	const std::string& optionalCapabilities() const;
	void setOptionalCapabilities(const std::string& optCapts);

	void setUseV100Plus(const std::string& connectOptions);

protected:

	void eConnectBase();
	void eDisconnectBase();

public:

	enum ConnState {
		CS_DISCONNECTED,
		CS_CONNECTING,
		CS_CONNECTED,
		CS_REDIRECT
	};

	// connection state
	ConnState connState() const;
	bool isConnected() const;

	const std::string& host() const { return m_host; }
	unsigned port() const { return m_port; }

protected:

	// access to protected variables
	EWrapper * getWrapper() const;
	void setClientId( int clientId);
	void setExtraAuth( bool extraAuth);
	void setHost( const std::string& host);
	void setPort( unsigned port);

public:

	bool isInBufferEmpty() const;
	bool isOutBufferEmpty() const;

	// override virtual funcs from EClient
	int serverVersion();
	std::string TwsConnectionTime();
	void reqMktData(TickerId id, const Contract& contract,
		const std::string& genericTicks, bool snapshot, const TagValueListSPtr& mktDataOptions);
	void cancelMktData(TickerId id);
	void placeOrder(OrderId id, const Contract& contract, const Order& order);
	void cancelOrder(OrderId id) ;
	void reqOpenOrders();
	void reqAccountUpdates(bool subscribe, const std::string& acctCode);
	void reqExecutions(int reqId, const ExecutionFilter& filter);
	void reqIds(int numIds);
	bool checkMessages();
	void reqContractDetails(int reqId, const Contract& contract);
	void reqMktDepth(TickerId tickerId, const Contract& contract, int numRows, const TagValueListSPtr& mktDepthOptions);
	void cancelMktDepth(TickerId tickerId);
	void reqNewsBulletins(bool allMsgs);
	void cancelNewsBulletins();
	void setServerLogLevel(int level);
	void reqAutoOpenOrders(bool bAutoBind);
	void reqAllOpenOrders();
	void reqManagedAccts();
	void requestFA(faDataType pFaDataType);
	void replaceFA(faDataType pFaDataType, const std::string& cxml);
	void reqHistoricalData( TickerId id, const Contract& contract,
		const std::string& endDateTime, const std::string& durationStr,
		const std::string&  barSizeSetting, const std::string& whatToShow,
		int useRTH, int formatDate, const TagValueListSPtr& chartOptions);
	void exerciseOptions(TickerId tickerId, const Contract& contract,
		int exerciseAction, int exerciseQuantity,
		const std::string& account, int override);
	void cancelHistoricalData(TickerId tickerId );
	void reqRealTimeBars(TickerId id, const Contract& contract, int barSize,
		const std::string& whatToShow, bool useRTH, const TagValueListSPtr& realTimeBarsOptions);
	void cancelRealTimeBars(TickerId tickerId );
	void cancelScannerSubscription(int tickerId);
	void reqScannerParameters();
	void reqScannerSubscription(int tickerId, const ScannerSubscription& subscription, const TagValueListSPtr& scannerSubscriptionOptions);
	void reqCurrentTime();
	void reqFundamentalData(TickerId reqId, const Contract&, const std::string& reportType);
	void cancelFundamentalData(TickerId reqId);
	void calculateImpliedVolatility(TickerId reqId, const Contract& contract, double optionPrice, double underPrice);
	void calculateOptionPrice(TickerId reqId, const Contract& contract, double volatility, double underPrice);
	void cancelCalculateImpliedVolatility(TickerId reqId);
	void cancelCalculateOptionPrice(TickerId reqId);
	void reqGlobalCancel();
	void reqMarketDataType(int marketDataType);
	void reqPositions();
	void cancelPositions();
	void reqAccountSummary( int reqId, const std::string& groupName, const std::string& tags);
	void cancelAccountSummary( int reqId);
	void verifyRequest( const std::string& apiName, const std::string& apiVersion);
	void verifyMessage( const std::string& apiData);
	void verifyAndAuthRequest( const std::string& apiName, const std::string& apiVersion, const std::string& opaqueIsvKey);
	void verifyAndAuthMessage( const std::string& apiData, const std::string& xyzResponse);
	void queryDisplayGroups( int reqId);
	void subscribeToGroupEvents( int reqId, int groupId);
	void updateDisplayGroup( int reqId, const std::string& contractInfo);
	void unsubscribeFromGroupEvents( int reqId);

private:

	virtual int send(const char* buf, size_t sz) = 0;
	virtual int receive(char* buf, size_t sz) = 0;

protected:

	int sendBufferedData();

private:

	void prepareBufferImpl(std::ostream&) const;
	void prepareBuffer(std::ostream&) const;

	void encodeMsgLen(std::string& msg, unsigned offset) const;
	void closeAndSend(std::string msg, unsigned offset = 0);

	int bufferedSend(const char* buf, size_t sz);
	int bufferedSend(const std::string& msg);

	// read and buffer what's available
	int bufferedRead();

	// try to process connection request ack
	int processConnectAckImpl(const char*& ptr, const char* endPtr);
	int processConnectAck(const char*& ptr, const char* endPtr);

	// try to process single msg
	int processMsgImpl(const char*& ptr, const char* endPtr);
	int processMsg(const char*& ptr, const char* endPtr);

	typedef int (EClientSocketBase::*messageHandler)(const char*& ptr, const char* endPtr);
	int processOnePrefixedMsg(const char*& ptr, const char* endPtr, messageHandler);

	void startApi();

	static bool CheckOffset(const char* ptr, const char* endPtr);
	static const char* FindFieldEnd(const char* ptr, const char* endPtr);

	// decoders
	static bool DecodeField(bool&, const char*& ptr, const char* endPtr);
	static bool DecodeField(int&, const char*& ptr, const char* endPtr);
	static bool DecodeField(long&, const char*& ptr, const char* endPtr);
	static bool DecodeField(double&, const char*& ptr, const char* endPtr);
	static bool DecodeField(std::string&, const char*& ptr, const char* endPtr);

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

	bool isConnecting() const;
	void onConnectBase();

private:

	typedef std::vector<char> BytesVec;

private:

	static void CleanupBuffer(BytesVec&, int processed);

private:

	EWrapper *m_pEWrapper;

	BytesVec m_inBuffer;
	BytesVec m_outBuffer;

	std::string m_host;
	int m_port;

	int m_clientId;

	ConnState m_connState;
	bool m_extraAuth;
	int m_serverVersion;
	std::string m_TwsTime;

	std::string m_optionalCapabilities;

	bool m_useV100Plus;
	std::string m_connectOptions;

};

template<> void EClientSocketBase::EncodeField<bool>(std::ostream& os, bool);
template<> void EClientSocketBase::EncodeField<double>(std::ostream& os, double);

#endif
