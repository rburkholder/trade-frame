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

#include "TFIQFeed/IQFeedMsgShim.h"
#include "TFIQFeed/IQFeedNewsQueryMsgShim.h"

#include "ListViewCtrl_Headlines.h"

// some things yet todo:
//  use custom draw to do the headlines
//  hash the articles to determine unique ones
//  speak the headlines?
//  implement cursor and return key so don't need mouse always for getting story content
//  extract and sort symbols
//  implement various sorts and filters

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

  typedef ou::tf::iqfeed::MsgShim<CNewsReaderView>::linebuffer_t linebuffer_t;

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
//    WM_IQFEED_SENDDONE,
//    WM_IQFEED_DISCONNECTED,
//    WM_IQFEED_ERROR, 

    WM_QUERY_CONNECTED,

    WM_IQFEED_STORY_LINE,  // one story line
    WM_IQFEED_STORY_DONE,  // no more story lines to process from story request

    WM_IQFEED_NEWS_CONFIG_DONE,

    WM_IQFEED_NEWS,
    WM_IQFEED_NEWS_DONE  // called from derived class, if we need it
  };

  enum enumActiveControl {
    EACNone, EACTree, EACHeadlines, EACStoryu
  };

  BEGIN_MSG_MAP_EX(CNewsReaderView)
    MESSAGE_HANDLER( WM_IQFEED_NEWS, OnIQFeedNews )  // goes to external handler
    MESSAGE_HANDLER( WM_IQFEED_NEWS_DONE, OnIQFeedNewsDone )  // message returned from external handler

    MESSAGE_HANDLER( WM_IQFEED_STORY_LINE, OnIQFeedStoryLine )
    MESSAGE_HANDLER( WM_IQFEED_STORY_DONE, OnIQFeedStoryDone )

    NOTIFY_HANDLER( IDC_LVHEADLINES, LVN_HOTTRACK, OnLVHotTrackHeadlines )
    NOTIFY_HANDLER( IDC_TREESOURCES, LVN_HOTTRACK, OnLVHotTrackTree )
    NOTIFY_HANDLER( IDC_EDITSTORY, LVN_HOTTRACK, OnLVHotTrackStory )

    NOTIFY_HANDLER( IDC_LVHEADLINES, LVN_GETDISPINFO, OnLVHeadlinesDispInfo )
    NOTIFY_HANDLER( IDC_LVHEADLINES, LVN_ITEMACTIVATE, OnLVHeadlinesItemActivate )
    NOTIFY_HANDLER( IDC_LVHEADLINES, NM_CLICK, OnLVHeadlinesClick )
    NOTIFY_HANDLER( IDC_LVHEADLINES, NM_HOVER, OnLVHeadlinesHover )
    NOTIFY_HANDLER( IDC_LVHEADLINES, NM_RCLICK, OnLVHeadlinesRClick )

    MESSAGE_HANDLER( WM_IQFEED_CONNECTED, OnIQFeedConnected )
    MESSAGE_HANDLER( WM_QUERY_CONNECTED, OnQueryConnected )
//    MESSAGE_HANDLER( WM_IQFEED_DISCONNECTED, OnIQFeedDisconnected )
//    MESSAGE_HANDLER( WM_IQFEED_SENDDONE, OnIQFeedSendDone )
//    MESSAGE_HANDLER( WM_IQFEED_ERROR, OnIQFeedError )

    MESSAGE_HANDLER( WM_IQFEED_NEWS_CONFIG_DONE, OnIQFeedNewsConfigDone )


//    MSG_WM_MOUSEMOVE(OnMouseMove)
//    MSG_WM_MOUSEWHEEL(OnMouseWheel)
//    MSG_WM_MOVE(OnMove)
//    MSG_WM_SIZE(OnSize)  // when enabled, does not allow CDialogResize to do its job
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_DESTROY(OnDestroy)
    CHAIN_MSG_MAP_MEMBER(m_lvHeadlines)
    CHAIN_MSG_MAP(CDialogResize<CNewsReaderView>)
	END_MSG_MAP()

  CTreeViewCtrl m_treeSources;
//  CListViewCtrl m_lvHeadlines;
  CListViewCtrl_Headlines m_lvHeadlines;
  CEdit m_edtStory;

  LRESULT OnIQFeedNews( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedNewsDone( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnIQFeedStoryLine( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnIQFeedStoryDone( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnQueryConnected( UINT, WPARAM, LPARAM, BOOL& );
//  LRESULT OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& );
//  LRESULT OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& );
//  LRESULT OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& );

  LRESULT OnIQFeedNewsConfigDone( UINT, WPARAM, LPARAM, BOOL& );

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
  void OnDestroy();
  void OnSize(UINT nType, CSize size);
  void OnMove(CPoint ptPos);
  void OnMouseMove(UINT nFlags, CPoint point);
  BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

  LRESULT OnLVHotTrackHeadlines( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHotTrackTree( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHotTrackStory( int idCtrl, LPNMHDR, BOOL& );

  LRESULT OnLVHeadlinesDispInfo( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesItemActivate( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesClick( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesHover( int idCtrl, LPNMHDR, BOOL& );
  LRESULT OnLVHeadlinesRClick( int idCtrl, LPNMHDR, BOOL& );

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

  void DisplayStory( vNewsItems_t::size_type ix );

private:
  typedef CDialogImpl<CNewsReaderView> CThisClass;

  ou::tf::iqfeed::MsgShim<CNewsReaderView>::structMessageDestinations m_MsgIdsForIQFeed;
  ou::tf::iqfeed::MsgShim<CNewsReaderView>* m_pIQFeed;

  ou::tf::CIQFeedNewsQueryMsgShim<CNewsReaderView>::structMessageDestinations m_MsgIdsForNewsQuery;
  ou::tf::CIQFeedNewsQueryMsgShim<CNewsReaderView>* m_pIQFeedNewsQuery;

  vNewsItems_t m_NewsItems;

};
