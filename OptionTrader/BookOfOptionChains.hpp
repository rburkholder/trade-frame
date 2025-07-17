/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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
/*
 * File:    BookOfOptionChains.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 10:26:28
 */

#pragma once

//#include <boost/date_time/gregorian/greg_date.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/treebook.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_BOOKOFOPTIONCHAINS_STYLE wxTAB_TRAVERSAL | wxBK_LEFT
#define SYMBOL_BOOKOFOPTIONCHAINS_TITLE _("Book of Option Chains")
#define SYMBOL_BOOKOFOPTIONCHAINS_IDNAME ID_BOOKOPTIONCHAINS
#define SYMBOL_BOOKOFOPTIONCHAINS_SIZE wxSize(-1, -1)
#define SYMBOL_BOOKOFOPTIONCHAINS_POSITION wxDefaultPosition

class BookOfOptionChains:
  public wxTreebook
{
  friend class boost::serialization::access;
public:

  BookOfOptionChains();
  BookOfOptionChains(
    wxWindow* parent, wxWindowID id = SYMBOL_BOOKOFOPTIONCHAINS_IDNAME,
    const wxPoint& pos = SYMBOL_BOOKOFOPTIONCHAINS_POSITION,
    const wxSize& size = SYMBOL_BOOKOFOPTIONCHAINS_SIZE,
    long style = SYMBOL_BOOKOFOPTIONCHAINS_STYLE,
    const wxString& name = SYMBOL_BOOKOFOPTIONCHAINS_TITLE );
  virtual ~BookOfOptionChains();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_BOOKOFOPTIONCHAINS_IDNAME,
    const wxPoint& pos = SYMBOL_BOOKOFOPTIONCHAINS_POSITION,
    const wxSize& size = SYMBOL_BOOKOFOPTIONCHAINS_SIZE,
    long style = SYMBOL_BOOKOFOPTIONCHAINS_STYLE,
    const wxString& name = SYMBOL_BOOKOFOPTIONCHAINS_TITLE );

  //using fOnPageEvent_t = std::function<void(boost::gregorian::date)>;
  using fOnPageEvent_t = std::function<void()>;
  using fOnNodeEvent_t = std::function<void()>;

  void Set(
    fOnPageEvent_t&& fOnPageChanging // departing
  , fOnPageEvent_t&& fOnPageChanged  // arriving
  , fOnNodeEvent_t&& fOnNodeCollapsed
  , fOnNodeEvent_t&& fOnNodeExpanded
  );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_BOOKOPTIONCHAINS
  };

  bool m_bEventsAreBound;

  fOnPageEvent_t m_fOnPageChanging; // about to depart page
  fOnPageEvent_t m_fOnPageChanged;  // new page in place
  fOnNodeEvent_t m_fOnNodeCollapsed;
  fOnNodeEvent_t m_fOnNodeExpanded;

  void OnPageChanged( wxBookCtrlEvent& );
  void OnPageChanging( wxBookCtrlEvent& );
  void OnNodeCollapsed( wxBookCtrlEvent& );
  void OnNodeExpanded( wxBookCtrlEvent& );

  void Init();
  void CreateControls();
  void OnDestroy( wxWindowDestroyEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  void BindBookEvents();
  void UnbindBookEvents();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::BookOfOptionChains, 1)
