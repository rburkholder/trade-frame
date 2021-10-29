/* Copyright (C) 2019 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef TWS_API_CLIENT_EORDERDECODER_H
#define TWS_API_CLIENT_EORDERDECODER_H

#include "platformspecific.h"
#include "Contract.h"
#include "Order.h"
#include "OrderState.h"
#include "EDecoder.h"

class EOrderDecoder
{
public:
	EOrderDecoder(Contract *contract, Order *order, OrderState *orderState, int version, int serverVersion);

public:
	bool decodeOrderId(const char*& ptr, const char* endPtr);
	bool decodeContract(const char*& ptr, const char* endPtr);
	bool decodeAction(const char*& ptr, const char* endPtr);
	bool decodeTotalQuantity(const char*& ptr, const char* endPtr);
	bool decodeOrderType(const char*& ptr, const char* endPtr);
	bool decodeLmtPrice(const char*& ptr, const char* endPtr);
	bool decodeAuxPrice(const char*& ptr, const char* endPtr);
	bool decodeTIF(const char*& ptr, const char* endPtr);
	bool decodeOcaGroup(const char*& ptr, const char* endPtr);
	bool decodeAccount(const char*& ptr, const char* endPtr);
	bool decodeOpenClose(const char*& ptr, const char* endPtr);
	bool decodeOrigin(const char*& ptr, const char* endPtr);
	bool decodeOrderRef(const char*& ptr, const char* endPtr);
	bool decodeClientId(const char*& ptr, const char* endPtr);
	bool decodePermId(const char*& ptr, const char* endPtr);
	bool decodeOutsideRth(const char*& ptr, const char* endPtr);
	bool decodeHidden(const char*& ptr, const char* endPtr);
	bool decodeDiscretionaryAmount(const char*& ptr, const char* endPtr);
	bool decodeGoodAfterTime(const char*& ptr, const char* endPtr);
	bool skipSharesAllocation(const char*& ptr, const char* endPtr);
	bool decodeFAParams(const char*& ptr, const char* endPtr);
	bool decodeModelCode(const char*& ptr, const char* endPtr);
	bool decodeGoodTillDate(const char*& ptr, const char* endPtr);
	bool decodeRule80A(const char*& ptr, const char* endPtr);
	bool decodePercentOffset(const char*& ptr, const char* endPtr);
	bool decodeSettlingFirm(const char*& ptr, const char* endPtr);
	bool decodeShortSaleParams(const char*& ptr, const char* endPtr);
	bool decodeAuctionStrategy(const char*& ptr, const char* endPtr);
	bool decodeBoxOrderParams(const char*& ptr, const char* endPtr);
	bool decodePegToStkOrVolOrderParams(const char*& ptr, const char* endPtr);
	bool decodeDisplaySize(const char*& ptr, const char* endPtr);
	bool decodeBlockOrder(const char*& ptr, const char* endPtr);
	bool decodeSweepToFill(const char*& ptr, const char* endPtr);
	bool decodeAllOrNone(const char*& ptr, const char* endPtr);
	bool decodeMinQty(const char*& ptr, const char* endPtr);
	bool decodeOcaType(const char*& ptr, const char* endPtr);
	bool skipETradeOnly(const char*& ptr, const char* endPtr);
	bool skipFirmQuoteOnly(const char*& ptr, const char* endPtr);
	bool skipNbboPriceCap(const char*& ptr, const char* endPtr);
	bool decodeParentId(const char*& ptr, const char* endPtr);
	bool decodeTriggerMethod(const char*& ptr, const char* endPtr);
	bool decodeVolOrderParams(const char*& ptr, const char* endPtr, bool decodeOpenOrderAttribs);
	bool decodeTrailParams(const char*& ptr, const char* endPtr);
	bool decodeBasisPoints(const char*& ptr, const char* endPtr);
	bool decodeComboLegs(const char*& ptr, const char* endPtr);
	bool decodeSmartComboRoutingParams(const char*& ptr, const char* endPtr);
	bool decodeScaleOrderParams(const char*& ptr, const char* endPtr);
	bool decodeHedgeParams(const char*& ptr, const char* endPtr);
	bool decodeOptOutSmartRouting(const char*& ptr, const char* endPtr);
	bool decodeClearingParams(const char*& ptr, const char* endPtr);
	bool decodeNotHeld(const char*& ptr, const char* endPtr);
	bool decodeDeltaNeutral(const char*& ptr, const char* endPtr);
	bool decodeAlgoParams(const char*& ptr, const char* endPtr);
	bool decodeSolicited(const char*& ptr, const char* endPtr);
	bool decodeWhatIfInfoAndCommission(const char*& ptr, const char* endPtr);
	bool decodeOrderStatus(const char*& ptr, const char* endPtr);
	bool decodeVolRandomizeFlags(const char*& ptr, const char* endPtr);
	bool decodePegBenchParams(const char*& ptr, const char* endPtr);
	bool decodeConditions(const char*& ptr, const char* endPtr);
	bool decodeAdjustedOrderParams(const char*& ptr, const char* endPtr);
	bool decodeStopPriceAndLmtPriceOffset(const char*& ptr, const char* endPtr);
	bool decodeSoftDollarTier(const char*& ptr, const char* endPtr);
	bool decodeCashQty(const char*& ptr, const char* endPtr);
	bool decodeDontUseAutoPriceForHedge(const char*& ptr, const char* endPtr);
	bool decodeIsOmsContainer(const char*& ptr, const char* endPtr);
	bool decodeDiscretionaryUpToLimitPrice(const char*& ptr, const char* endPtr);
	bool decodeAutoCancelDate(const char*& ptr, const char* endPtr);
	bool decodeFilledQuantity(const char*& ptr, const char* endPtr);
	bool decodeRefFuturesConId(const char*& ptr, const char* endPtr);
	bool decodeAutoCancelParent(const char*& ptr, const char* endPtr);
	bool decodeAutoCancelParent(const char*& ptr, const char* endPtr, int minVersionAutoCancelParent);
	bool decodeShareholder(const char*& ptr, const char* endPtr);
	bool decodeImbalanceOnly(const char*& ptr, const char* endPtr);
	bool decodeRouteMarketableToBbo(const char*& ptr, const char* endPtr);
	bool decodeParentPermId(const char*& ptr, const char* endPtr);
	bool decodeCompletedTime(const char*& ptr, const char* endPtr);
	bool decodeCompletedStatus(const char*& ptr, const char* endPtr);
	bool decodeUsePriceMgmtAlgo(const char*& ptr, const char* endPtr);
	bool decodeDuration(const char*& ptr, const char* endPtr);
	bool decodePostToAts(const char*& ptr, const char* endPtr);

private:
	Contract* m_contract;
	Order* m_order;
	OrderState* m_orderState;
	int m_version;
	int m_serverVersion;
};

#endif