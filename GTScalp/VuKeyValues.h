#pragma once

//#include "GTScalp.h"
#include "Resource.h"
#include "CtlKeyValues.h"
#include "IQFeedSymbol.h"
#include "RowKeyValues.h"
#include "IQFeedProviderSingleton.h"

#include <map>
#include <string>
using namespace std;

// CKeyValues dialog

// Base class in existance mostly for handling the ToolTips
//  also provides mechanism for getting symbols into the CListCtrl

class CVuKeyValues : public CDialog {
	DECLARE_DYNAMIC(CVuKeyValues)

public:
	CVuKeyValues(CWnd* pParent =NULL);   // standard constructor
	virtual ~CVuKeyValues();

// Dialog Data
	enum { IDD = IDD_VUKEYVALUES };

  // Sorting:
  // http://support.microsoft.com/kb/250614/en-us

  CRowKeyValues *AppendSymbol( const string &name );
  void WatchSymbol( const string &Symbol, CRowKeyValues *row );
  void UnWatchSymbol( const string &Symbol, CRowKeyValues *row );

protected:

  afx_msg BOOL OnInitDialog();
  CCtlKeyValues kv1;

  BOOL memberFxn( UINT id, NMHDR * pTTTStruct, LRESULT * pResult ) { return FALSE; };
  BOOL  fn( UINT id, NMHDR *nm, LRESULT * result ) { return FALSE; };
  afx_msg void fn2( NMHDR * nm, LRESULT * pResult );
  afx_msg void fn3( NMHDR * nm, LRESULT * pResult );
  afx_msg void fn4( NMHDR * nm, LRESULT * pResult );

  afx_msg void OnLButtonUp( UINT, CPoint );
  afx_msg void OnLButtonDown( UINT, CPoint );

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  afx_msg void OnSize( UINT, int, int );
  bool bDialogReady;

  CIQFeedProviderSingleton m_IQFeedProvider;

public:
  afx_msg void OnHdnDividerdblclickKeyvalues(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnHdnItemchangedKeyvalues(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnHdnEndtrackKeyvalues(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMCustomdrawKeyvalues(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnLvnItemchangedKeyvalues(NMHDR *pNMHDR, LRESULT *pResult);
};
