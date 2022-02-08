#pragma once

#include "VuKeyValues.h"

// CVuIndicies dialog

class CVuIndicies : public CVuKeyValues{
	DECLARE_DYNAMIC(CVuIndicies)

public:
	CVuIndicies(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVuIndicies();

// Dialog Data
	enum { IDD = IDD_VUINDICIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
