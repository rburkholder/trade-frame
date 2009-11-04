/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

// NewsReaderView.h : interface of the CNewsReaderView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <string>

#include "LibIQFeed/IQFeed.h"
#include "LibIQFeed/IQFeedNewsQuery.h"

class CNewsReaderView : public CDialogImpl<CNewsReaderView>,
                        public CDialogResize<CNewsReaderView>
{
public:
	enum { IDD = IDD_NEWSREADER_FORM };

	BOOL PreTranslateMessage(MSG* pMsg);

//  http://www.codeproject.com/KB/wtl/wtldlgresize.aspx
//    * DLSZ_SIZE_X: Resize the width of the control as the dialog resizes horizontally.
//    * DLSZ_SIZE_Y: Resize the height of the control as the dialog resizes vertically.
//    * DLSZ_MOVE_X: Move the control horizontally as the dialog resizes horizontally.
//    * DLSZ_MOVE_Y: Move the control vertically as the dialog resizes vertically.
//    * DLSZ_REPAINT: Invalidate the control after every move/resize so it repaints every time.

  BEGIN_DLGRESIZE_MAP(CNewsReaderView)
//    DLGRESIZE_CONTROL( IDC_TREESOURCES, DLSZ_SIZE_X )
    DLGRESIZE_CONTROL( IDC_LVHEADLINES, DLSZ_SIZE_X )
    DLGRESIZE_CONTROL( IDC_EDITSTORY, DLSZ_SIZE_X | DLSZ_SIZE_Y )
  END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  //HWND Create(HWND hWndParent, RECT&, LPARAM dwInitParam = NULL);
  HWND Create(HWND hWndParent, LPARAM dwInitParam = NULL);

  CNewsReaderView( void );
  ~CNewsReaderView( void );
protected:

  enum stateStoryRetrieval {
    INACTIVE,
    RETRIEVING
  } m_stateStoryRetrieval;

  enum enumMessages {
    WM_IQFEED_CONNECTED = WM_USER + 1,
    WM_IQFEED_SENDDONE,
    WM_IQFEED_DISCONNECTED,
    WM_IQFEED_ERROR, 

    WM_IQFEED_STORY_LINE,  // one story line
    WM_IQFEED_STORY_DONE,  // no more story lines to process from story request

    WM_IQFEED_NEWS,
    WM_IQFEED_NEWS_DONE  // called from derived class, if we need it
  };

  BEGIN_MSG_MAP_EX(CNewsReaderView)
    MESSAGE_HANDLER( WM_IQFEED_NEWS, OnIQFeedNews )  // goes to external handler
    MESSAGE_HANDLER( WM_IQFEED_NEWS_DONE, OnIQFeedNewsDone )  // message returned from external handler

    MESSAGE_HANDLER( WM_IQFEED_STORY_LINE, OnIQFeedStoryLine )
    MESSAGE_HANDLER( WM_IQFEED_STORY_LINE, OnIQFeedStoryDone )

    NOTIFY_HANDLER( IDC_LVHEADLINES, LVN_GETDISPINFO, OnLVHeadlinesDispInfo )
    NOTIFY_HANDLER( IDC_LVHEADLINES, LVN_ITEMACTIVATE, OnLVHeadlinesItemActivate )
    NOTIFY_HANDLER( IDC_LVHEADLINES, LVN_HOTTRACK, OnLVHeadlinesHotTrack )
    NOTIFY_HANDLER( IDC_LVHEADLINES, NM_CLICK, OnLVHeadlinesClick )
    NOTIFY_HANDLER( IDC_LVHEADLINES, NM_HOVER, OnLVHeadlinesHover )
    NOTIFY_HANDLER( IDC_LVHEADLINES, NM_RCLICK, OnLVHeadlinesRClick )

    MESSAGE_HANDLER( WM_IQFEED_CONNECTED, OnIQFeedConnected )
    MESSAGE_HANDLER( WM_IQFEED_DISCONNECTED, OnIQFeedDisconnected )
    MESSAGE_HANDLER( WM_IQFEED_SENDDONE, OnIQFeedSendDone )
    MESSAGE_HANDLER( WM_IQFEED_ERROR, OnIQFeedError )

//    MSG_WM_MOVE(OnMove)
//    MSG_WM_SIZE(OnSize)  // when enabled, does not allow CDialogResize to do its job
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_DESTROY(OnDestroy)
    CHAIN_MSG_MAP(CDialogResize<CNewsReaderView>)
	END_MSG_MAP()

  CTreeViewCtrl m_treeSources;
  CListViewCtrl m_lvHeadlines;
  CEdit m_edtStory;

  LRESULT OnIQFeedNews( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedNewsDone( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnIQFeedStoryLine( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedStoryDone( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& );

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
  void OnDestroy();
  void OnSize(UINT nType, CSize size);
  void OnMove(CPoint ptPos);

  LRESULT OnLVHeadlinesDispInfo( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesItemActivate( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesHotTrack( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesClick( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesHover( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesRClick( int idCtrl, LPNMHDR, BOOL& );

private:
  typedef CDialogImpl<CNewsReaderView> CThisClass;

  CIQFeed<CNewsReaderView>::structMessageDestinations m_Destinations;
  CIQFeed<CNewsReaderView>* m_pIQFeed;
  CIQFeedNewsQuery<CNewsReaderView>* m_pIQFeedNewsQuery;

  struct structNewsItem {
    std::string Distributor;
    std::string StoryId;
    std::string SymbolList;
    std::string DateTime;
    std::string Headline;
    bool StoryLoaded;
    std::string Story;
    structNewsItem(void): StoryLoaded( false ) {};
  };

  typedef std::vector<structNewsItem> vNewsItems_t;
  vNewsItems_t m_NewsItems;

};
