/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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
 * File:   NotebookOptionChains.h
 * Author: raymond@burkholder.net
 *
 * Created on July 2, 2017, 8:16 PM
 */

#ifndef WINOPTIONCHAINS_H
#define WINOPTIONCHAINS_H

#include <map>
#include <functional>

#include <boost/signals2.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/wx.h>
#include <wx/listbook.h>

#include <TFTrading/TradingEnumerations.h>

#include <TFOptions/Option.h>

#include <TFVuTrading/GridOptionChain.h>

#include <TFBitsNPieces/FirstOrDefaultCombiner.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_OPTIONCHAINS_STYLE wxTAB_TRAVERSAL | wxNB_LEFT
#define SYMBOL_OPTIONCHAINS_TITLE _("Notebook Option Chains")
#define SYMBOL_OPTIONCHAINS_IDNAME ID_NOTEBOOK_OPTIONDETAILS
#define SYMBOL_OPTIONCHAINS_SIZE wxSize(-1, -1)
#define SYMBOL_OPTIONCHAINS_POSITION wxDefaultPosition

class NotebookOptionChains: public wxListbook {
  friend class boost::serialization::access;
public:

  NotebookOptionChains();
  NotebookOptionChains(
    wxWindow* parent, wxWindowID id = SYMBOL_OPTIONCHAINS_IDNAME,
    const wxPoint& pos = SYMBOL_OPTIONCHAINS_POSITION,
    const wxSize& size = SYMBOL_OPTIONCHAINS_SIZE,
    long style = SYMBOL_OPTIONCHAINS_STYLE,
    const wxString& name = SYMBOL_OPTIONCHAINS_TITLE );
  virtual ~NotebookOptionChains();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_OPTIONCHAINS_IDNAME,
    const wxPoint& pos = SYMBOL_OPTIONCHAINS_POSITION,
    const wxSize& size = SYMBOL_OPTIONCHAINS_SIZE,
    long style = SYMBOL_OPTIONCHAINS_STYLE,
    const wxString& name = SYMBOL_OPTIONCHAINS_TITLE  );

  void SetName( const std::string& sName );  // underlying
  void Add( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const std::string& sSymbol );

  using fOnRowClicked_t = std::function<void(boost::gregorian::date, double, bool bSelected, const GridOptionChain::OptionUpdateFunctions& call, const GridOptionChain::OptionUpdateFunctions& put )>;
  fOnRowClicked_t m_fOnRowClicked; // called when a row is control clicked

  using fOnOptionUnderlyingRetrieve_t = std::function<void(const std::string&, boost::gregorian::date, double, GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t )>;
  fOnOptionUnderlyingRetrieve_t m_fOnOptionUnderlyingRetrieve;

  using fOnPageEvent_t = std::function<void(boost::gregorian::date)>;
  fOnPageEvent_t m_fOnPageChanging; // about to depart page
  fOnPageEvent_t m_fOnPageChanged;  // new page in place

  void SetGridOptionChain_ColumnSaver( ou::tf::GridColumnSizer* );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_NOTEBOOK_OPTIONDETAILS
  };

  // put/call at strike
  struct Row {
    int ixRow;
    std::string sCall;
    std::string sPut;
    explicit Row( int ix = 0 ): ixRow( ix ) {}
  };

  // the strike list
  typedef std::map<double, Row> mapStrike_t;

  struct Tab {
    size_t ixTab;
    std::string sDate;
    mapStrike_t mapStrike;
    wxPanel* pPanel;
    GridOptionChain* pWinOptionChain;
    Tab( int ix = 0, const std::string& s = "", wxPanel* pPanel_ = nullptr, ou::tf::GridOptionChain* pGrid = nullptr )
      : ixTab( ix ), sDate( s ), pPanel( pPanel_ ), pWinOptionChain( pGrid ) {}
    Tab( const std::string& s, wxPanel* pPanel_ = nullptr, ou::tf::GridOptionChain* pGrid = nullptr )
      : ixTab{}, sDate( s ), pPanel( pPanel_ ), pWinOptionChain( pGrid ) {}
  };

  using mapOptionExpiry_t = std::map<boost::gregorian::date, Tab>;
  mapOptionExpiry_t m_mapOptionExpiry;

  bool m_bBound;

  std::string m_sName;  // should be underlying so can use to lookup in PanelCharts

  ou::tf::GridColumnSizer* m_pgcsGridOptionChain;

  void OnPageChanged( wxBookCtrlEvent& event );
  void OnPageChanging( wxBookCtrlEvent& event );

  void Init();
  void CreateControls();
  void OnDestroy( wxWindowDestroyEvent& event );

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

BOOST_CLASS_VERSION(ou::tf::NotebookOptionChains, 1)

#endif /* WINOPTIONCHAINS_H */

