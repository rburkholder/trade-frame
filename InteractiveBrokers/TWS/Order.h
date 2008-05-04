#ifndef order_def
#define order_def

#include <float.h>

#define UNSET_DOUBLE DBL_MAX
#define UNSET_INTEGER INT_MAX

enum Origin { CUSTOMER,
              FIRM,
			  UNKNOWN };

enum AuctionStrategy { AUCTION_UNSET = 0,
                       AUCTION_MATCH = 1,
                       AUCTION_IMPROVEMENT = 2,
					   AUCTION_TRANSPARENT = 3 };

struct Order
{
   Order() {
      orderId         = 0;
      clientId        = 0;
      permId          = 0;
      totalQuantity   = 0;
      lmtPrice        = 0;
      auxPrice        = 0;
      openClose       = "O";
      origin          = CUSTOMER;
      transmit        = true;
      parentId        = 0;
      blockOrder      = false;
      sweepToFill     = false;
      displaySize     = 0;
      triggerMethod   = 0;
      outsideRth      = false;
      hidden          = false;
      discretionaryAmt= 0;
      shortSaleSlot   = 0;
	  ocaType = 0;
	  allOrNone = false;
	  eTradeOnly = true;
	  firmQuoteOnly = true;
	  auctionStrategy = AUCTION_UNSET;
      minQty = UNSET_INTEGER;
      percentOffset = UNSET_DOUBLE;
      nbboPriceCap = UNSET_DOUBLE;
      startingPrice = UNSET_DOUBLE;
      stockRefPrice = UNSET_DOUBLE;
      delta = UNSET_DOUBLE;
      stockRangeLower = UNSET_DOUBLE;
      stockRangeUpper = UNSET_DOUBLE;
	  overridePercentageConstraints = false;

	  // VOLATILITY ORDERS ONLY
	  volatility = UNSET_DOUBLE;
	  volatilityType = UNSET_INTEGER;     // 1=daily, 2=annual
	  deltaNeutralOrderType = "";
	  deltaNeutralAuxPrice = UNSET_DOUBLE;
	  continuousUpdate = false;
	  referencePriceType = UNSET_INTEGER; // 1=Average, 2 = BidOrAsk
	  trailStopPrice = UNSET_DOUBLE;

      // COMBO ORDERS ONLY
      basisPoints = UNSET_DOUBLE;         // EFP orders only
      basisPointsType = UNSET_INTEGER;  // EFP orders only

	  // SCALE ORDERS ONLY
	  scaleNumComponents = UNSET_INTEGER;
	  scaleComponentSize = UNSET_INTEGER;
	  scalePriceIncrement = UNSET_DOUBLE;

	  // What-if
	  whatIf = false;
   }

   // order identifier
   long    orderId;
   long    clientId;
   long    permId;

   // main order fields
   CString action;
   long    totalQuantity;
   CString orderType;
   double  lmtPrice;
   double  auxPrice;

   // extended order fields
   CString tif;           // "Time in Force" - DAY, GTC, etc.
   CString ocaGroup;      // one cancels all group name
   int     ocaType;       // 1 = CANCEL_WITH_BLOCK, 2 = REDUCE_WITH_BLOCK, 3 = REDUCE_NON_BLOCK
   CString orderRef;      // order reference
   bool    transmit;      // if false, order will be created but not transmited
   long    parentId;      // Parent order Id, to associate Auto STP or TRAIL orders with the original order.
   bool    blockOrder;
   bool    sweepToFill;
   int     displaySize;
   int     triggerMethod; // 0=Default, 1=Double_Bid_Ask, 2=Last, 3=Double_Last, 4=Bid_Ask, 7=Last_or_Bid_Ask, 8=Mid-point
   bool    outsideRth;
   bool    hidden;
   CString goodAfterTime;    // Format: 20060505 08:00:00 {time zone}
   CString goodTillDate;     // Format: 20060505 08:00:00 {time zone}
   CString rule80A; // Individual = 'I', Agency = 'A', AgentOtherMember = 'W', IndividualPTIA = 'J', AgencyPTIA = 'U', AgentOtherMemberPTIA = 'M', IndividualPT = 'K', AgencyPT = 'Y', AgentOtherMemberPT = 'N'
   bool    allOrNone;
   int     minQty;
   double  percentOffset; // REL orders only
   bool    overridePercentageConstraints;
   double  trailStopPrice; // TRAILLIMIT orders only

	// financial advisors only
   CString faGroup;
   CString faProfile;
   CString faMethod;
   CString faPercentage;

   // institutional (ie non-cleared) only
   CString openClose; // O=Open, C=Close
   Origin  origin;    // 0=Customer, 1=Firm
   int     shortSaleSlot; // 1 if you hold the shares, 2 if they will be delivered from elsewhere.  Only for Action="SSHORT
   CString designatedLocation; // set when slot=2 only.

   // SMART routing only
   double   discretionaryAmt; // SMART routing only
   bool     eTradeOnly;    // SMART routing only
   bool     firmQuoteOnly; // SMART routing only
   double   nbboPriceCap; // SMART routing only

   // BOX exchange orders only
   int      auctionStrategy; // AUCTION_MATCH, AUCTION_IMPROVEMENT, AUCTION_TRANSPARENT
   double   startingPrice; 
   double   stockRefPrice;
   double   delta;

   // pegged to stock and VOL orders only
   double   stockRangeLower;
   double   stockRangeUpper;

   // VOLATILITY ORDERS ONLY
   double   volatility;
   int      volatilityType;     // 1=daily, 2=annual
   CString  deltaNeutralOrderType;
   double   deltaNeutralAuxPrice;
   bool     continuousUpdate;
   int      referencePriceType; // 1=Average, 2 = BidOrAsk

   // COMBO ORDERS ONLY
   double   basisPoints;      // EFP orders only
   int      basisPointsType;  // EFP orders only

   // SCALE ORDERS ONLY
   int    scaleNumComponents;
   int    scaleComponentSize;
   double scalePriceIncrement;

   // Clearing info
   CString account; // IB account
   CString settlingFirm;
   CString clearingAccount; // True beneficiary of the order
   CString clearingIntent; // "" (Default), "IB", "Away", "PTA" (PostTrade)

   // What-if
   bool   whatIf;
};

#endif