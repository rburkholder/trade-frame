#pragma once

#include "VuKeyValues.h"



class CVuSymbolsOfInterest :  public CVuKeyValues {
	DECLARE_DYNAMIC(CVuSymbolsOfInterest)

public:
  CVuSymbolsOfInterest(CWnd* pParent = NULL);
  virtual ~CVuSymbolsOfInterest();

// Dialog Data
	enum { IDD = IDD_VUSYMBOLSOFINTEREST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
