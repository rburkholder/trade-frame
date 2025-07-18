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
 * File:    InstrumentViews.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 10:26:28
 */

#pragma once

//#include <boost/date_time/gregorian/greg_date.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>

#define SYMBOL_INSTRUMENTVIEWS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_INSTRUMENTVIEWS_TITLE _("Book of Option Chains")
#define SYMBOL_INSTRUMENTVIEWS_IDNAME ID_BOOKOPTIONCHAINS
#define SYMBOL_INSTRUMENTVIEWS_SIZE wxDefaultSize
#define SYMBOL_INSTRUMENTVIEWS_POSITION wxDefaultPosition

class wxTreeCtrl;
class wxTreeEvent;

namespace ou { // One Unified
namespace tf { // TradeFrame

class TreeItem;

class InstrumentViews
: public wxPanel
{
  friend class boost::serialization::access;
public:

  InstrumentViews();
  InstrumentViews(
    wxWindow* parent, wxWindowID id = SYMBOL_INSTRUMENTVIEWS_IDNAME,
    const wxPoint& pos = SYMBOL_INSTRUMENTVIEWS_POSITION,
    const wxSize& size = SYMBOL_INSTRUMENTVIEWS_SIZE,
    long style = SYMBOL_INSTRUMENTVIEWS_STYLE,
    const wxString& name = SYMBOL_INSTRUMENTVIEWS_TITLE );
  virtual ~InstrumentViews();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_INSTRUMENTVIEWS_IDNAME,
    const wxPoint& pos = SYMBOL_INSTRUMENTVIEWS_POSITION,
    const wxSize& size = SYMBOL_INSTRUMENTVIEWS_SIZE,
    long style = SYMBOL_INSTRUMENTVIEWS_STYLE,
    const wxString& name = SYMBOL_INSTRUMENTVIEWS_TITLE );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_BOOKOPTIONCHAINS
  , ID_TREECTRL
  };

  wxTreeCtrl* m_pTreeCtrl;
  TreeItem* m_pRootTreeItem; // // root of custom tree items

  //using fOnPageEvent_t = std::function<void(boost::gregorian::date)>;
  using fOnPageEvent_t = std::function<void()>;
  using fOnNodeEvent_t = std::function<void()>;

  void Set(
    fOnPageEvent_t&& fOnPageChanging // departing
  , fOnPageEvent_t&& fOnPageChanged  // arriving
  , fOnNodeEvent_t&& fOnNodeCollapsed
  , fOnNodeEvent_t&& fOnNodeExpanded
  );

  fOnPageEvent_t m_fOnPageChanging; // about to depart page
  fOnPageEvent_t m_fOnPageChanged;  // new page in place
  fOnNodeEvent_t m_fOnNodeCollapsed;
  fOnNodeEvent_t m_fOnNodeExpanded;

  void Init();
  void CreateControls();
  void HandleTreeEventItemGetToolTip( wxTreeEvent& );
  void OnDestroy( wxWindowDestroyEvent& event );

  void AddSymbol();

  wxBitmap GetBitmapResource( const wxString& name );
  static bool ShowToolTips() { return true; };

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

BOOST_CLASS_VERSION(ou::tf::InstrumentViews, 1)
