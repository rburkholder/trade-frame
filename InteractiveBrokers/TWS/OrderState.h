#ifndef ORDER_STATE_H__INCLUDED
#define ORDER_STATE_H__INCLUDED

#include <atlstr.h>
#include "Order.h"

struct OrderState {

	explicit OrderState()
		:
		commission(UNSET_DOUBLE),
		minCommission(UNSET_DOUBLE),
		maxCommission(UNSET_DOUBLE)
	{}

	CString status;

	CString initMargin;
	CString maintMargin;
	CString equityWithLoan;

	double  commission;
	double  minCommission;
	double  maxCommission;
	CString commissionCurrency;
	
	CString warningText;
};

#endif