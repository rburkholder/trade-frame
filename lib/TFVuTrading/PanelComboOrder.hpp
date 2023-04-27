/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    PanelComboOrder.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: April 10, 2023 13:10:29
 */

#pragma once

#include <map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>
#include <wx/timer.h>

// amalgamation of NotebookOptionChains, GridOptionChain

class wxGrid;
class wxButton;
class wxListbook;
class wxGridEvent;
class wxToggleButton;
class wxListbookEvent;

#include <TFVuTrading/GridOptionChain.hpp>
#include <TFVuTrading/GridOptionComboOrder.hpp>

#include "InterfaceBookOptionChain.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

class GridOptionChain;

#define SYMBOL_PANELCOMBOORDER_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELCOMBOORDER_TITLE _("PanelComboOrder")
#define SYMBOL_PANELCOMBOORDER_IDNAME ID_PANELCOMBOORDER
#define SYMBOL_PANELCOMBOORDER_SIZE wxDefaultSize
#define SYMBOL_PANELCOMBOORDER_POSITION wxDefaultPosition

class PanelComboOrder:
  public wxPanel
, public InterfaceBookOptionChain
{
  friend class boost::serialization::access;
public:

  PanelComboOrder();
  PanelComboOrder(
    wxWindow* parent, wxWindowID id = ID_PANELCOMBOORDER,
    const wxPoint& pos = SYMBOL_PANELCOMBOORDER_POSITION,
    const wxSize& size = SYMBOL_PANELCOMBOORDER_SIZE,
    long style = SYMBOL_PANELCOMBOORDER_STYLE,
    const wxString& name = SYMBOL_PANELCOMBOORDER_TITLE );
  virtual ~PanelComboOrder();

  bool Create( wxWindow* parent,
    wxWindowID id = ID_PANELCOMBOORDER,
    const wxPoint& pos = SYMBOL_PANELCOMBOORDER_POSITION,
    const wxSize& size = SYMBOL_PANELCOMBOORDER_SIZE,
    long style = SYMBOL_PANELCOMBOORDER_STYLE,
    const wxString& name = SYMBOL_PANELCOMBOORDER_TITLE  );

  virtual void Set(
    fOnPageEvent_t&& fOnPageChanging // departed
  , fOnPageEvent_t&& fOnPageChanged  // arrival
  );

  virtual void Set( // NOTE: needs to be set prior to use of Add()
    fOptionDelegates_t&& fOptionDelegates_Attach
  , fOptionDelegates_t&& fOptionDelegates_Detach
  );

  virtual void MakeRowVisible( boost::gregorian::date, double strike );

  // slow mechanism of updating grid entries
  virtual void Update( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Quote& );
  virtual void Update( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Trade& );
  virtual void Update( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Greek& );

  virtual void Clear( boost::gregorian::date, double strike );

  void Add( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const std::string& sSymbol );

  using fOnRowClicked_t = std::function<void(boost::gregorian::date, double, bool bSelected, const ou::tf::option::Delegates& call, const ou::tf::option::Delegates& put )>;
  fOnRowClicked_t m_fOnRowClicked; // called when a row is control clicked

  using fOnOptionUnderlyingRetrieve_t = std::function<void(const std::string&, boost::gregorian::date, double, GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t )>;
  fOnOptionUnderlyingRetrieve_t m_fOnOptionUnderlyingRetrieve;

  void SetGridOptionChain_ColumnSaver( ou::tf::GridColumnSizer* );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANELCOMBOORDER
  , ID_BOOK_OptionChains
  , ID_GRID_OptionChain
  , ID_GRID_ComboOrder
  , ID_BTN_UpdateGreeks
  , ID_BTN_ClearOrder
  , ID_BTN_PlaceOrder
  };

  wxListbook* m_pBookOptionChains;

  wxToggleButton* m_btnUpgdateGreeks;
  wxButton* m_btnClearOrder;
  wxButton* m_btnPlaceOrder;

  wxTimer m_timerGuiRefresh;
  void HandleGuiRefresh( wxTimerEvent& event );

  void StartRefresh();
  void StopRefresh();

  fOnPageEvent_t m_fOnPageChanging; // about to depart page
  fOnPageEvent_t m_fOnPageChanged;  // new page in place

  fOptionDelegates_t m_fOptionDelegates_Attach;
  fOptionDelegates_t m_fOptionDelegates_Detach;

  // put/call at strike
  struct Row {
    int ixRow;
    std::string sCall;
    std::string sPut;
    explicit Row( int ix = 0 ): ixRow( ix ) {}
  };

  // the strike list
  using mapStrike_t = std::map<double, Row>;

  struct Tab {
    size_t ixTab;
    std::string sDate;
    mapStrike_t mapStrike;
    wxPanel* pPanel;
    GridOptionChain* pGridOptionChain;
    Tab( int ix = 0, const std::string& s = "", wxPanel* pPanel_ = nullptr, ou::tf::GridOptionChain* pGrid = nullptr )
      : ixTab( ix ), sDate( s ), pPanel( pPanel_ ), pGridOptionChain( pGrid ) {}
    Tab( const std::string& s, wxPanel* pPanel_ = nullptr, ou::tf::GridOptionChain* pGrid = nullptr )
      : ixTab{}, sDate( s ), pPanel( pPanel_ ), pGridOptionChain( pGrid ) {}
  };

  using mapOptionExpiry_t = std::map<boost::gregorian::date, Tab>;
  mapOptionExpiry_t m_mapOptionExpiry;

  GridOptionChain* m_pGridOptionChain_Current;
  GridOptionComboOrder* m_pGridOptionComboOrder;
  ou::tf::GridColumnSizer* m_pgcsGridOptionChain;

  void Init();
  void CreateControls();

    void OnDestroy_Book( wxWindowDestroyEvent& event );
    void OnDestroy_Panel( wxWindowDestroyEvent& event );

    /// wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED event handler for ID_BOOK_OptionChains
    void OnBOOKOptionChainsPageChanged( wxListbookEvent& event );

    /// wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING event handler for ID_BOOK_OptionChains
    void OnBOOKOptionChainsPageChanging( wxListbookEvent& event );

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_ComboOrder
    void OnCellLeftClick( wxGridEvent& event );

    /// wxEVT_GRID_CELL_RIGHT_CLICK event handler for ID_GRID_ComboOrder
    void OnCellRightClick( wxGridEvent& event );

    /// wxEVT_MOTION event handler for ID_GRID_ComboOrder
    void OnMotion( wxMouseEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_BTN_UpdateGreeks
    void OnBTNUpdateGreeksClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_ClearOrder
    void OnBTNClearOrderClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BTN_PlaceOrder
    void OnBTNPlaceOrderClick( wxCommandEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  void BindEvents();
  void UnbindEvents();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemResources>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemResources>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou
