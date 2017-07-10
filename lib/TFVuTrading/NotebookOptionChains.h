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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/signals2.hpp>

#include <wx/wx.h>
#include <wx/notebook.h>

#include <TFTrading/TradingEnumerations.h>
#include <TFOptions/Option.h>
#include <TFVuTrading/WinOptionDetails.h>
#include <TFBitsNPieces/FirstOrDefaultCombiner.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_OPTIONCHAINS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_OPTIONCHAINS_TITLE _("Notebook Option Chains")
#define SYMBOL_OPTIONCHAINS_IDNAME ID_NOTEBOOK_OPTIONDETAILS
#define SYMBOL_OPTIONCHAINS_SIZE wxSize(-1, -1)
#define SYMBOL_OPTIONCHAINS_POSITION wxDefaultPosition

class NotebookOptionChains: public wxNotebook {
public:
  
  typedef Watch::pWatch_t pWatch_t;
  typedef boost::signals2::signal<pWatch_t ( const std::string& ), // string is the IQFeed name
                                  ou::tf::FirstOrDefault<pWatch_t> > signalObtainWatch_t;
  typedef signalObtainWatch_t::slot_type slotObtainWatch_t;
  signalObtainWatch_t signalObtainWatch;
  
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
  void Clear();
  void Add( boost::gregorian::date, double strike, ou::tf::OptionSide::enumOptionSide, const std::string& sSymbol );
  
  void Save( boost::archive::text_oarchive& oa);
  void Load( boost::archive::text_iarchive& ia);

protected:
private:
  
  enum { 
    ID_Null=wxID_HIGHEST, ID_OPTIONCHAINS, ID_NOTEBOOK_OPTIONDETAILS
  };
  
  struct Row {
    int ixRow;
    std::string sCall;
    std::string sPut;
    explicit Row( int ix = 0 ): ixRow( ix ) {}
  };
  
  typedef std::map<double, Row> mapStrike_t;
  
  struct Tab {
    size_t ixTab;
    std::string sDate;
    mapStrike_t mapStrike;
    GridOptionDetails* pWinOptionsDetails;
    Tab( int ix = 0, const std::string& s = "", ou::tf::GridOptionDetails* p = nullptr )
      : ixTab( ix ), sDate( s ), pWinOptionsDetails( p ) {}
  };
  
  typedef std::map<boost::gregorian::date, Tab> mapOptionExpiry_t;
  
  mapOptionExpiry_t m_mapOptionExpiry;

  void OnPageChanged( wxBookCtrlEvent& event );
  void OnPageChanging( wxBookCtrlEvent& event );
  
  void Init( void );
  void OnDestroy( wxWindowDestroyEvent& event );
  
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};

} // namespace tf
} // namespace ou

#endif /* WINOPTIONCHAINS_H */

