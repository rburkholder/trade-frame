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

#include <set>
#include <unordered_map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>

#include <TFIQFeed/Provider.h>

#include <TFTrading/ComposeInstrument.hpp>

#define SYMBOL_INSTRUMENTVIEWS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_INSTRUMENTVIEWS_TITLE _("Instrument Views")
#define SYMBOL_INSTRUMENTVIEWS_IDNAME ID_BOOKOPTIONCHAINS
#define SYMBOL_INSTRUMENTVIEWS_SIZE wxDefaultSize
#define SYMBOL_INSTRUMENTVIEWS_POSITION wxDefaultPosition

class wxTreeCtrl;
class wxTreeEvent;
class OptionChainView;

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

  void Set( ou::tf::iqfeed::Provider::pProvider_t& );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_BOOKOPTIONCHAINS
  , ID_TREECTRL
  };

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  ou::tf::iqfeed::Provider::pProvider_t m_piqf;

  wxTreeCtrl* m_pTreeCtrl;
  TreeItem* m_pRootTreeItem; // // root of custom tree items

  using setInstrumentName_t = std::set<std::string>;
  setInstrumentName_t m_setInstrumentName;

  using pComposeInstrument_t = std::unique_ptr<ou::tf::ComposeInstrument>;
  pComposeInstrument_t m_pComposeInstrument;

  struct Instrument {

    ou::tf::TreeItem* pti;
    pInstrument_t pInstrument;
    OptionChainView* pChainView;

    Instrument()
    : pti( nullptr ), pChainView( nullptr )
    {}
    Instrument( Instrument&& rhs ) {}
    ~Instrument() {}
  };

  using mapInstrument_t = std::unordered_map<std::string, Instrument>;
  mapInstrument_t m_mapInstrument;

  OptionChainView* m_pcurOptionChainView;

  void Init();
  void CreateControls();
  void HandleTreeEventItemGetToolTip( wxTreeEvent& );
  void OnDestroy( wxWindowDestroyEvent& event );

  void DialogSymbol();
  void AddSymbol( const std::string& );
  void BuildView( pInstrument_t& );

  wxBitmap GetBitmapResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & m_mapInstrument.size();
    for ( const mapInstrument_t::value_type& vt: m_mapInstrument ) {
      ar & vt.first;
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    size_t nNames;
    ar & nNames;
    std::string sName;
    while ( 0 != nNames ) {
      ar & sName;
      m_setInstrumentName.insert( sName ); // process once iqfeed connected
      --nNames;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::InstrumentViews, 1)
