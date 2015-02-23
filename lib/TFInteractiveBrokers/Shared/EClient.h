/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef eclient_def
#define eclient_def

#include "CommonDefs.h"
#include "TagValue.h"

struct Contract;
struct Order;
struct ExecutionFilter;
struct ScannerSubscription;

class TWSAPIDLLEXP EClient
{
public:
   virtual ~EClient() {}
   virtual bool eConnect( const char *host, unsigned int port, int clientId = 0, bool extraAuth = 0) = 0;
   virtual void eDisconnect() = 0;
   virtual int serverVersion() = 0;
   virtual std::string TwsConnectionTime() = 0;
   virtual void reqMktData( TickerId id, const Contract& contract,
	   const std::string& genericTicks, bool snapshot, const TagValueListSPtr& mktDataOptions) = 0;
   virtual void cancelMktData( TickerId id) = 0;
   virtual void placeOrder( OrderId id, const Contract& contract, const Order& order) = 0;
   virtual void cancelOrder( OrderId id) = 0;
   virtual void reqOpenOrders() = 0;
   virtual void reqAccountUpdates(bool subscribe, const std::string& acctCode) = 0;
   virtual void reqExecutions(int reqId, const ExecutionFilter& filter) = 0;
   virtual void reqIds( int numIds) = 0;
   virtual bool checkMessages() = 0;
   virtual void reqContractDetails( int reqId, const Contract& contract) = 0;
   virtual void reqMktDepth( TickerId id, const Contract& contract, int numRows, const TagValueListSPtr& mktDepthOptions) = 0;
   virtual void cancelMktDepth( TickerId id) = 0;
   virtual void reqNewsBulletins( bool allMsgs) = 0;
   virtual void cancelNewsBulletins() = 0;
   virtual void setServerLogLevel(int level) = 0;
   virtual void reqAutoOpenOrders(bool bAutoBind) = 0;
   virtual void reqAllOpenOrders() = 0;
   virtual void reqManagedAccts() = 0;
   virtual void requestFA(faDataType pFaDataType) = 0;
   virtual void replaceFA(faDataType pFaDataType, const std::string& cxml) = 0;
   virtual void reqHistoricalData( TickerId id, const Contract& contract,
	   const std::string& endDateTime, const std::string& durationStr, const std::string& barSizeSetting,
	   const std::string& whatToShow, int useRTH, int formatDate, const TagValueListSPtr& chartOptions) = 0;
   virtual void exerciseOptions( TickerId id, const Contract& contract,
       int exerciseAction, int exerciseQuantity, const std::string& account, int override) = 0;
   virtual void cancelHistoricalData( TickerId tickerId ) = 0;
   virtual void reqRealTimeBars( TickerId id, const Contract& contract, int barSize,
	   const std::string& whatToShow, bool useRTH, const TagValueListSPtr& realTimeBarsOptions) = 0;
   virtual void cancelRealTimeBars( TickerId tickerId) = 0;
   virtual void cancelScannerSubscription( int tickerId) = 0;
   virtual void reqScannerParameters() = 0;
   virtual void reqScannerSubscription( int tickerId, const ScannerSubscription& subscription, const TagValueListSPtr& scannerSubscriptionOptions) = 0;
   virtual void reqCurrentTime() = 0;
   virtual void reqFundamentalData( TickerId reqId, const Contract&, const std::string& reportType) = 0;
   virtual void cancelFundamentalData( TickerId reqId) = 0;
   virtual void calculateImpliedVolatility( TickerId reqId, const Contract& contract, double optionPrice, double underPrice) = 0;
   virtual void calculateOptionPrice( TickerId reqId, const Contract& contract, double volatility, double underPrice) = 0;
   virtual void cancelCalculateImpliedVolatility( TickerId reqId) = 0;
   virtual void cancelCalculateOptionPrice( TickerId reqId) = 0;
   virtual void reqGlobalCancel() = 0;
   virtual void reqMarketDataType( int marketDataType) = 0;
   virtual void reqPositions() = 0;
   virtual void cancelPositions() = 0;
   virtual void reqAccountSummary( int reqId, const std::string& groupName, const std::string& tags) = 0;
   virtual void cancelAccountSummary( int reqId) = 0;
   virtual void verifyRequest( const std::string& apiName, const std::string& apiVersion) = 0;
   virtual void verifyMessage( const std::string& apiData) = 0;
   virtual void queryDisplayGroups( int reqId) = 0;
   virtual void subscribeToGroupEvents( int reqId, int groupId) = 0;
   virtual void updateDisplayGroup( int reqId, const std::string& contractInfo) = 0;
   virtual void unsubscribeFromGroupEvents( int reqId) = 0;

private:
};

#endif
