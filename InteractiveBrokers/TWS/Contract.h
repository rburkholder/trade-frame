#ifndef contract_def
#define contract_def

#include <vector>

/*
    SAME_POS    = open/close leg value is same as combo
    OPEN_POS    = open
    CLOSE_POS   = close
    UNKNOWN_POS = unknown

*/
enum LegOpenClose { SAME_POS, OPEN_POS, CLOSE_POS, UNKNOWN_POS };

struct ComboLeg
{

   ComboLeg()
      :
      conId(0),
	  ratio(0),
	  openClose(0),
	  shortSaleSlot(0)
   {
   }

   long    conId;
   long    ratio;
   CString action; //BUY/SELL/SSHORT

   CString exchange;
   long    openClose; // LegOpenClose enum values

   // for stock legs when doing short sale
   long    shortSaleSlot; // 1 = clearing broker, 2 = third party
   CString designatedLocation;

   bool operator==( const ComboLeg &other) const
   {
      return (conId == other.conId &&
         ratio == other.ratio &&
		 openClose == other.openClose &&
		 shortSaleSlot == other.shortSaleSlot &&
         action.CompareNoCase(other.action) == 0 &&
         exchange.CompareNoCase(other.exchange) == 0 &&
		 designatedLocation.CompareNoCase(other.designatedLocation) == 0);
   }

};

struct Contract
{
   Contract()
      : conId(0)
	  , strike(0)
	  , includeExpired(false)
	  , comboLegs(NULL)
   {
   }

   long    conId;
   CString symbol;
   CString secType;
   CString expiry;
   double  strike;
   CString right;
   CString multiplier;
   CString exchange;
   CString primaryExchange; // pick an actual (ie non-aggregate) exchange that the contract trades on.  DO NOT SET TO SMART.
   CString currency;
   CString localSymbol;
   bool includeExpired;

   // COMBOS
   CString comboLegsDescrip; // received in open order 14 and up for all combos

   typedef std::vector<ComboLeg*> ComboLegList;
   ComboLegList* comboLegs;

public:

	// Helpers
	static void CloneComboLegs(ComboLegList& dst, const ComboLegList& src);
    static void CleanupComboLegs(ComboLegList&);
};

struct ContractDetails
{
   ContractDetails()
      : minTick(0)
	  , priceMagnifier(0)
	  , callable(false)
	  , putable(false)
	  , coupon(0)
	  , convertible(false)
	  , nextOptionPartial(false)
		
   {
   }

   Contract	summary;
   CString	marketName;
   CString	tradingClass;
   double	minTick;
   CString	orderTypes;
   CString	validExchanges;
   long     priceMagnifier;

   // BOND values
   CString cusip;
   CString ratings;
   CString descAppend;
   CString bondType;
   CString couponType;
   bool callable;
   bool putable;
   double coupon;
   bool convertible;
   CString maturity;
   CString issueDate;
   CString nextOptionDate;
   CString nextOptionType;
   bool nextOptionPartial;
   CString notes;
};

inline void
Contract::CloneComboLegs(ComboLegList& dst, const ComboLegList& src)
{
	CleanupComboLegs(dst);

	if (src.empty())
		return;

	dst.reserve(src.size());

	ComboLegList::const_iterator iter = src.begin();
	const ComboLegList::const_iterator iterEnd = src.end();

	for (; iter != iterEnd; ++iter) {
		const ComboLeg* leg = *iter;
		if (!leg)
			continue;
		dst.push_back(new ComboLeg(*leg));
	}
}

inline void
Contract::CleanupComboLegs(ComboLegList& legs)
{
	if (legs.empty())
		return;

	ComboLegList::iterator iter = legs.begin();
	const ComboLegList::iterator iterEnd = legs.end();

	for (; iter != iterEnd; ++iter) {
		delete *iter;
	}

	legs.clear();
}

#endif
