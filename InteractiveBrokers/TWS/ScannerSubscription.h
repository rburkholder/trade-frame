#ifndef scanner_def
#define scanner_def

#include <float.h>

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
    CString instrument;
    CString locationCode;
    CString scanCode;
    double abovePrice;
    double belowPrice;
    int aboveVolume;
    double marketCapAbove;
    double marketCapBelow;
    CString moodyRatingAbove;
    CString moodyRatingBelow;
    CString spRatingAbove;
    CString spRatingBelow;
    CString maturityDateAbove;
    CString maturityDateBelow;
    double couponRateAbove;
    double couponRateBelow;
    int excludeConvertible;
	int averageOptionVolumeAbove;
	CString scannerSettingPairs;
	CString stockTypeFilter;
};

#endif
