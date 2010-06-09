#ifndef scanner_def
#define scanner_def

#include <float.h>
#include <limits.h>

#include "IBString.h"

#define UNSET_DOUBLE DBL_MAX
#define UNSET_INTEGER INT_MAX
#define NO_ROW_NUMBER_SPECIFIED -1;

struct ScannerSubscription {
	ScannerSubscription() {
		numberOfRows = NO_ROW_NUMBER_SPECIFIED;
		abovePrice = DBL_MAX;
		belowPrice = DBL_MAX;
		aboveVolume = INT_MAX;
		marketCapAbove = DBL_MAX;
		marketCapBelow = DBL_MAX;
		couponRateAbove = DBL_MAX;
		couponRateBelow = DBL_MAX;
		excludeConvertible = 0;
		averageOptionVolumeAbove = 0;
	}
    int numberOfRows;
    IBString instrument;
    IBString locationCode;
    IBString scanCode;
    double abovePrice;
    double belowPrice;
    int aboveVolume;
    double marketCapAbove;
    double marketCapBelow;
    IBString moodyRatingAbove;
    IBString moodyRatingBelow;
    IBString spRatingAbove;
    IBString spRatingBelow;
    IBString maturityDateAbove;
    IBString maturityDateBelow;
    double couponRateAbove;
    double couponRateBelow;
    int excludeConvertible;
	int averageOptionVolumeAbove;
	IBString scannerSettingPairs;
	IBString stockTypeFilter;
};

#endif
