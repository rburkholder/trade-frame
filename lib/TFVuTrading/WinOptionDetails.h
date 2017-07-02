/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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
// Started 2014/09/10

#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <TFOptions/Option.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_WIN_OPTIONDETAILS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WIN_OPTIONDETAILS_TITLE _("Panel Option Details")
#define SYMBOL_WIN_OPTIONDETAILS_IDNAME ID_WIN_OPTIONDETAILS
#define SYMBOL_WIN_OPTIONDETAILS_SIZE wxSize(-1, -1)
#define SYMBOL_WIN_OPTIONDETAILS_POSITION wxDefaultPosition

class WinOptionDetails_impl;  // Forward Declaration

class WinOptionDetails: public wxWindow {
  friend WinOptionDetails_impl;
public:

  WinOptionDetails(void);
  WinOptionDetails( 
    wxWindow* parent, wxWindowID id = SYMBOL_WIN_OPTIONDETAILS_IDNAME, 
    const wxPoint& pos = SYMBOL_WIN_OPTIONDETAILS_POSITION, 
    const wxSize& size = SYMBOL_WIN_OPTIONDETAILS_SIZE, 
    long style = SYMBOL_WIN_OPTIONDETAILS_STYLE );
  ~WinOptionDetails(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_WIN_OPTIONDETAILS_IDNAME, 
    const wxPoint& pos = SYMBOL_WIN_OPTIONDETAILS_POSITION, 
    const wxSize& size = SYMBOL_WIN_OPTIONDETAILS_SIZE, 
    long style = SYMBOL_WIN_OPTIONDETAILS_STYLE );
  
  void UpdateCallGreeks( double strike, ou::tf::Greek& );
  void UpdateCallQuote( double strike, ou::tf::Quote& );
  void UpdateCallTrade( double strike, ou::tf::Trade& );  
  void UpdatePutGreeks( double strike, ou::tf::Greek& );
  void UpdatePutQuote( double strike, ou::tf::Quote& );
  void UpdatePutTrade( double strike, ou::tf::Trade& );  

protected:

  void Init( void );

private:
  enum { 
    ID_Null=wxID_HIGHEST, ID_WIN_OPTIONDETAILS, ID_GRID_OPTIONDETAILS
  };

  std::unique_ptr<WinOptionDetails_impl> m_pimpl;
  
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<class Archive>
  void serialize(Archive & ar, const unsigned int file_version);
};

} // namespace tf
} // namespace ou