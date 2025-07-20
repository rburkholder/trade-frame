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
#include <memory>
#include <unordered_map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>

#include <TFTrading/Instrument.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Option.h>
#include <TFOptions/Engine.h>

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
class ComposeInstrument;

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

  using pComposeInstrument_t = std::shared_ptr<ComposeInstrument>;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using fBuildWatch_t = std::function<pWatch_t( pInstrument_t )>;
  using fBuildOption_t = std::function<pOption_t( pInstrument_t )>;

  using pOptionEngine_t = std::shared_ptr<ou::tf::option::Engine>;

  void Set(
    pComposeInstrument_t&
  , fBuildWatch_t&&
  , fBuildOption_t&&
  , pOptionEngine_t&
  );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_BOOKOPTIONCHAINS
  , ID_TREECTRL
  };

  wxTreeCtrl* m_pTreeCtrl;
  TreeItem* m_pRootTreeItem; // // root of custom tree items

  using setInstrumentName_t = std::set<std::string>;
  setInstrumentName_t m_setInstrumentName;

  pComposeInstrument_t m_pComposeInstrument;  // IQFeed only, or IQFeed & IB

  fBuildWatch_t m_fBuildWatch;
  fBuildOption_t m_fBuildOption;

  pOptionEngine_t m_pOptionEngine;

  struct Instance: public ou::tf::option::chain::OptionName {
    pInstrument_t pInstrument; // resident in all Options
    pOption_t pOption;  // note, includes Watch, just-in-time Watch/Option construction
  };

  using chain_t = ou::tf::option::Chain<Instance>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;

  struct Instrument {

    ou::tf::TreeItem* pti;
    pInstrument_t pInstrument;
    OptionChainView* pChainView;
    mapChains_t mapChains;

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
  void HandleTreeEventItemExpanded( wxTreeEvent& );
  void OnDestroy( wxWindowDestroyEvent& event );

  void SizeTreeCtrl();

  void DialogSymbol();
  void AddSymbol( const std::string& );
  void BuildView( pInstrument_t& );
  void BuildOptionChains( mapInstrument_t::iterator );
  void PresentOptionChains( mapInstrument_t::iterator );

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
