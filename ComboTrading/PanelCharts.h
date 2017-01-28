/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started December 6, 2015, 1:26 PM

#pragma once

#include <map>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/event.h>

#include <TFBitsNPieces/TreeOps.h>
#include <TFVuTrading/DialogPickSymbol.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/Watch.h>

#include "TreeItem.h"
#include "ChartInteractive.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_CHARTS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_CHARTS_TITLE _("Panel Chart Data")
#define SYMBOL_PANEL_CHARTS_IDNAME ID_PANEL_CHARTS
#define SYMBOL_PANEL_CHARTS_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_CHARTS_POSITION wxDefaultPosition

class PanelCharts: public wxPanel {
public:
  
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::Watch::pWatch_t pWatch_t;
  
  PanelCharts( void );
  PanelCharts( wxWindow* parent, wxWindowID id = SYMBOL_PANEL_CHARTS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_CHARTS_POSITION, 
    const wxSize& size = SYMBOL_PANEL_CHARTS_SIZE, 
    long style = SYMBOL_PANEL_CHARTS_STYLE );
  virtual ~PanelCharts();

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_CHARTS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_CHARTS_POSITION, 
    const wxSize& size = SYMBOL_PANEL_CHARTS_SIZE, 
    long style = SYMBOL_PANEL_CHARTS_STYLE );
  
  typedef boost::signals2::signal<void(const std::string&, std::string&)> signalLookUpDescription_t;
  typedef signalLookUpDescription_t::slot_type slotLookUpDescription_t;
  signalLookUpDescription_t signalLookUpDescription;
  
  struct ValuesForBuildInstrument {
    const std::string& sKey;
    const std::string& sIQF;
    const std::string& sIB;
    pInstrument_t& pInstrument;
    boost::uint16_t day;
    ValuesForBuildInstrument( const std::string& sKey_, const std::string& sIQF_, const std::string& sIB_, pInstrument_t& pInstrument_, boost::uint16_t day_) 
      : sKey( sKey_ ), sIQF( sIQF_ ), sIB( sIB_ ), pInstrument( pInstrument_ ), day( day_ ) 
      {}
  };
  
  typedef boost::signals2::signal<void(ValuesForBuildInstrument&)> signalBuildInstrument_t;
  typedef signalBuildInstrument_t::slot_type slotBuildInstrument_t;
  signalBuildInstrument_t signalBuildInstrument;
  
  typedef boost::signals2::signal<void(pInstrument_t)> signalRegisterInstrument_t;
  typedef signalRegisterInstrument_t::slot_type slotRegisterInstrument_t;
  signalRegisterInstrument_t signalRegisterInstrument;
  
  typedef boost::signals2::signal<pInstrument_t(const std::string&), ou::tf::FirstOrDefault<pInstrument_t> > signalLoadInstrument_t;
  typedef signalLoadInstrument_t::slot_type slotLoadInstrument_t;
  signalLoadInstrument_t signalLoadInstrument;

  void InstrumentUpdated( pInstrument_t ); // typically:  the ib contract has arrived
  
  // providers may change, so what happens to providers already registered with an instrument?
  typedef ou::tf::ProviderManager::pProvider_t pProvider_t;
  void SetProviders( pProvider_t pData1Provider, pProvider_t pData2Provider, pProvider_t pExecutionProvider );
  
  //void StartWatch( void );
  //void StopWatch( void );

  void Save( boost::archive::text_oarchive& oa);
  void Load( boost::archive::text_iarchive& ia);
  
protected: 
  
  void Init();
  void CreateControls();
  
private:
  
  enum { 
    ID_Null=wxID_HIGHEST, ID_PANEL_CHARTS, 
    MIRoot, MIGroup, MIInstrument, MIPortfolio, MIPosition
  };
  
  typedef std::map<ou::tf::Instrument::idInstrument_t,pWatch_t> mapInstrumentWatch_t;
  mapInstrumentWatch_t m_mapInstrumentWatch;
  
  pProvider_t m_pData1Provider;
  pProvider_t m_pData2Provider;
  pProvider_t m_pExecutionProvider;
  
  ou::tf::TreeItemResources m_baseResources;
  Resources m_resources;
  ou::tf::TreeOps* m_pTreeOps;
  
  ou::tf::DialogPickSymbol* m_pDialogPickSymbol;
  DialogPickSymbol::DataExchange m_de;
  pInstrument_t m_pDialogPickSymbolCreatedInstrument;
  
  //ChartInteractive* m_pwinDetail;
  wxWindow* m_pwinDetail;
  
  void HandleTreeOpsChanging( wxTreeItemId id );
  
  void HandleLookUpDescription( const std::string&, std::string& );
  
  pWatch_t HandleNewInstrumentRequest( const Resources::ENewInstrumentLock );
  void HandleComposeComposite( ou::tf::DialogPickSymbol::DataExchange* );
  
  pWatch_t HandleLoadInstrument( const std::string& );
  pWatch_t LoadInstrument( pInstrument_t );
  
  void BuildInstrument( const DialogPickSymbol::DataExchange& pde, pInstrument_t& pInstrument );

  void OnClose( wxCloseEvent& event );
  
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};

} // namespace tf
} // namespace ou
