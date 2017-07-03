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
 * File:   WinOptionChains.h
 * Author: raymond@burkholder.net
 * 
 * Created on July 2, 2017, 8:16 PM
 */

#ifndef WINOPTIONCHAINS_H
#define WINOPTIONCHAINS_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/wx.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_WIN_OPTIONCHAINS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WIN_OPTIONCHAINS_TITLE _("Window Option Chains")
#define SYMBOL_WIN_OPTIONCHAINS_IDNAME ID_WIN_OPTIONCHAINS
#define SYMBOL_WIN_OPTIONCHAINS_SIZE wxSize(-1, -1)
#define SYMBOL_WIN_OPTIONCHAINS_POSITION wxDefaultPosition

class WinOptionChains: public wxWindow {
public:
  WinOptionChains();
  WinOptionChains( 
    wxWindow* parent, wxWindowID id = SYMBOL_WIN_OPTIONCHAINS_IDNAME, 
    const wxPoint& pos = SYMBOL_WIN_OPTIONCHAINS_POSITION, 
    const wxSize& size = SYMBOL_WIN_OPTIONCHAINS_SIZE, 
    long style = SYMBOL_WIN_OPTIONCHAINS_STYLE );
  virtual ~WinOptionChains();

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_WIN_OPTIONCHAINS_IDNAME, 
    const wxPoint& pos = SYMBOL_WIN_OPTIONCHAINS_POSITION, 
    const wxSize& size = SYMBOL_WIN_OPTIONCHAINS_SIZE, 
    long style = SYMBOL_WIN_OPTIONCHAINS_STYLE );
  
  void CreateControls();
  
  void Save( boost::archive::text_oarchive& oa);
  void Load( boost::archive::text_iarchive& ia);

protected:
  void Init( void );
private:
  enum { 
    ID_Null=wxID_HIGHEST, ID_WIN_OPTIONCHAINS, ID_NOTEBOOK_OPTIONDETAILS
  };

  void OnDestroy( wxWindowDestroyEvent& event );
  
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};

} // namespace tf
} // namespace ou

#endif /* WINOPTIONCHAINS_H */

