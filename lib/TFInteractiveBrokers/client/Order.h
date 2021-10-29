/* Copyright (C) 2019 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef TWS_API_CLIENT_ORDER_H
#define TWS_API_CLIENT_ORDER_H

#include "TagValue.h"
#include "OrderCondition.h"
#include "SoftDollarTier.h"
#include "Decimal.h"
#include <float.h>
#include <limits.h>

#define UNSET_DOUBLE DBL_MAX
#define UNSET_INTEGER INT_MAX
#define UNSET_LONG LLONG_MAX
#define UNSET_DECIMAL ULLONG_MAX

enum Origin { CUSTOMER,
              FIRM,
              UNKNOWN };

enum AuctionStrategy { AUCTION_UNSET = 0,
                       AUCTION_MATCH = 1,
                       AUCTION_IMPROVEMENT = 2,
                       AUCTION_TRANSPARENT = 3 };

enum UsePriceMmgtAlgo { DONT_USE = 0,
                        USE,
                        DEFAULT = UNSET_INTEGER };

struct OrderComboLeg
{
	OrderComboLeg()
	{
		price = UNSET_DOUBLE;
	}

	double price;

	bool operator==( const OrderComboLeg& other) const
	{
		return (price == other.price);
	}
};

typedef std::shared_ptr<OrderComboLeg> OrderComboLegSPtr;

struct Order
{
	// order identifier
	long     orderId = 0;
	long     clientId = 0;
	int      permId = 0;

	// main order fields
	std::string action;
	Decimal  totalQuantity = UNSET_DECIMAL;
	std::string orderType;
	double   lmtPrice = UNSET_DOUBLE;
	double   auxPrice = UNSET_DOUBLE;

	// extended order fields
	std::string tif;           // "Time in Force" - DAY, GTC, etc.
	std::string activeStartTime = "";	// for GTC orders
	std::string activeStopTime = "";	// for GTC orders
	std::string ocaGroup;      // one cancels all group name
	int      ocaType = 0;       // 1 = CANCEL_WITH_BLOCK, 2 = REDUCE_WITH_BLOCK, 3 = REDUCE_NON_BLOCK
	std::string orderRef;      // order reference
	bool     transmit = true;      // if false, order will be created but not transmited
	long     parentId = 0;      // Parent order Id, to associate Auto STP or TRAIL orders with the original order.
	bool     blockOrder = false;
	bool     sweepToFill = false;
	int      displaySize = 0;
	int      triggerMethod = 0; // 0=Default, 1=Double_Bid_Ask, 2=Last, 3=Double_Last, 4=Bid_Ask, 7=Last_or_Bid_Ask, 8=Mid-point
	bool     outsideRth = false;
	bool     hidden = false;
	std::string goodAfterTime;    // Format: 20060505 08:00:00 {time zone}
	std::string goodTillDate;     // Format: 20060505 08:00:00 {time zone}
	std::string rule80A; // Individual = 'I', Agency = 'A', AgentOtherMember = 'W', IndividualPTIA = 'J', AgencyPTIA = 'U', AgentOtherMemberPTIA = 'M', IndividualPT = 'K', AgencyPT = 'Y', AgentOtherMemberPT = 'N'
	bool     allOrNone = false;
	int      minQty = UNSET_INTEGER;
	double   percentOffset = UNSET_DOUBLE; // REL orders only
	bool     overridePercentageConstraints = false;
	double   trailStopPrice = UNSET_DOUBLE; // TRAILLIMIT orders only
	double   trailingPercent = UNSET_DOUBLE;

	// financial advisors only
	std::string faGroup;
	std::string faProfile;
	std::string faMethod;
	std::string faPercentage;

	// institutional (ie non-cleared) only
	std::string openClose = ""; // O=Open, C=Close
	Origin   origin = CUSTOMER;    // 0=Customer, 1=Firm
	int      shortSaleSlot = 0; // 1 if you hold the shares, 2 if they will be delivered from elsewhere.  Only for Action="SSHORT
	std::string designatedLocation; // set when slot=2 only.
	int      exemptCode = -1;

	// SMART routing only
	double   discretionaryAmt = 0;
	bool     optOutSmartRouting = false;

	// BOX exchange orders only
	int      auctionStrategy = AUCTION_UNSET; // AUCTION_MATCH, AUCTION_IMPROVEMENT, AUCTION_TRANSPARENT
	double   startingPrice = UNSET_DOUBLE;
	double   stockRefPrice = UNSET_DOUBLE;
	double   delta = UNSET_DOUBLE;

	// pegged to stock and VOL orders only
	double   stockRangeLower = UNSET_DOUBLE;
	double   stockRangeUpper = UNSET_DOUBLE;

	bool     randomizeSize = false;
	bool     randomizePrice = false;

	// VOLATILITY ORDERS ONLY
	double   volatility = UNSET_DOUBLE;
	int      volatilityType = UNSET_INTEGER;     // 1=daily, 2=annual
	std::string deltaNeutralOrderType = "";
	double   deltaNeutralAuxPrice = UNSET_DOUBLE;
	long     deltaNeutralConId = 0;
	std::string deltaNeutralSettlingFirm = "";
	std::string deltaNeutralClearingAccount = "";
	std::string deltaNeutralClearingIntent = "";
	std::string deltaNeutralOpenClose = "";
	bool     deltaNeutralShortSale = false;
	int      deltaNeutralShortSaleSlot = 0;
	std::string deltaNeutralDesignatedLocation = "";
	bool     continuousUpdate = false;
	int      referencePriceType = UNSET_INTEGER; // 1=Average, 2 = BidOrAsk

	// COMBO ORDERS ONLY
	double   basisPoints = UNSET_DOUBLE;      // EFP orders only
	int      basisPointsType = UNSET_INTEGER;  // EFP orders only

	// SCALE ORDERS ONLY
	int      scaleInitLevelSize = UNSET_INTEGER;
	int      scaleSubsLevelSize = UNSET_INTEGER;
	double   scalePriceIncrement = UNSET_DOUBLE;
	double   scalePriceAdjustValue = UNSET_DOUBLE;
	int      scalePriceAdjustInterval = UNSET_INTEGER;
	double   scaleProfitOffset = UNSET_DOUBLE;
	bool     scaleAutoReset = false;
	int      scaleInitPosition = UNSET_INTEGER;
	int      scaleInitFillQty = UNSET_INTEGER;
	bool     scaleRandomPercent = false;
	std::string scaleTable = "";

	// HEDGE ORDERS
	std::string hedgeType;  // 'D' - delta, 'B' - beta, 'F' - FX, 'P' - pair
	std::string hedgeParam; // 'beta=X' value for beta hedge, 'ratio=Y' for pair hedge

	// Clearing info
	std::string account; // IB account
	std::string settlingFirm;
	std::string clearingAccount; // True beneficiary of the order
	std::string clearingIntent; // "" (Default), "IB", "Away", "PTA" (PostTrade)

	// ALGO ORDERS ONLY
	std::string algoStrategy;

	TagValueListSPtr algoParams;
	TagValueListSPtr smartComboRoutingParams;

	std::string algoId;

	// What-if
	bool     whatIf = false;

	// Not Held
	bool     notHeld = false;
	bool     solicited = false;

	// models
	std::string modelCode;

	// order combo legs
	typedef std::vector<OrderComboLegSPtr> OrderComboLegList;
	typedef std::shared_ptr<OrderComboLegList> OrderComboLegListSPtr;

	OrderComboLegListSPtr orderComboLegs;

	TagValueListSPtr orderMiscOptions;

	//VER PEG2BENCH fields:
	int referenceContractId = UNSET_INTEGER;
	double peggedChangeAmount = UNSET_DOUBLE;
	bool isPeggedChangeAmountDecrease = false;
	double referenceChangeAmount = UNSET_DOUBLE;
	std::string referenceExchangeId;
	std::string adjustedOrderType;
	double triggerPrice = UNSET_DOUBLE;
	double adjustedStopPrice = UNSET_DOUBLE;
	double adjustedStopLimitPrice = UNSET_DOUBLE;
	double adjustedTrailingAmount = UNSET_DOUBLE;
	int adjustableTrailingUnit = UNSET_INTEGER;
	double lmtPriceOffset = UNSET_DOUBLE;

	std::vector<std::shared_ptr<OrderCondition>> conditions;
	bool conditionsCancelOrder = false;
	bool conditionsIgnoreRth = false;

	// ext operator
	std::string extOperator = "";

	SoftDollarTier softDollarTier = SoftDollarTier("", "", "");

	// native cash quantity
	double cashQty = UNSET_DOUBLE;

	std::string mifid2DecisionMaker = "";
	std::string mifid2DecisionAlgo = "";
	std::string mifid2ExecutionTrader = "";
	std::string mifid2ExecutionAlgo = "";

	// don't use auto price for hedge
	bool dontUseAutoPriceForHedge = false;

	bool isOmsContainer = false;

	bool discretionaryUpToLimitPrice = false;

	std::string autoCancelDate = "";
	Decimal filledQuantity = UNSET_DECIMAL;
	int refFuturesConId = UNSET_INTEGER;
	bool autoCancelParent = false;
	std::string shareholder = "";
	bool imbalanceOnly = false;
	bool routeMarketableToBbo = false;
	long long parentPermId = UNSET_LONG;

	UsePriceMmgtAlgo usePriceMgmtAlgo = UsePriceMmgtAlgo::DEFAULT;
	int duration = UNSET_INTEGER;
	int postToAts = UNSET_INTEGER;

public:

	// Helpers
	static void CloneOrderComboLegs(OrderComboLegListSPtr& dst, const OrderComboLegListSPtr& src);
};

inline void
Order::CloneOrderComboLegs(OrderComboLegListSPtr& dst, const OrderComboLegListSPtr& src)
{
	if (!src.get())
		return;

	dst->reserve(src->size());

	OrderComboLegList::const_iterator iter = src->begin();
	const OrderComboLegList::const_iterator iterEnd = src->end();

	for (; iter != iterEnd; ++iter) {
		const OrderComboLeg* leg = iter->get();
		if (!leg)
			continue;
		dst->push_back(OrderComboLegSPtr(new OrderComboLeg(*leg)));
	}
}

#endif
