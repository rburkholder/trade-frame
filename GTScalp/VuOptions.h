#pragma once

#include "VuKeyValues.h"
#include "IQFeedOptions.h"
#include "IQFeedProviderSingleton.h"

// CVuIndicies dialog

class CVuOptions : public CVuKeyValues{
	DECLARE_DYNAMIC(CVuOptions)

public:
	CVuOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVuOptions();

// Dialog Data
	enum { IDD = IDD_VUOPTIONS };

protected:

  CIQFeedProviderSingleton m_IQFeedProvider;

  CIQFeedOptions *options;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
  void HandleSymbolListReceived();
  void MonitorSymbolList();

};

