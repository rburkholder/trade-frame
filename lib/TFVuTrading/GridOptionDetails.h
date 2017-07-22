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

#include <wx/grid.h>

#include <TFOptions/Option.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define GRID_OPTIONDETAILS_STYLE wxWANTS_CHARS
#define GRID_OPTIONDETAILS_TITLE _("Grid Option Details")
#define GRID_OPTIONDETAILS_IDNAME ID_GRID_OPTIONDETAILS
#define GRID_OPTIONDETAILS_SIZE wxSize(-1, -1)
#define GRID_OPTIONDETAILS_POSITION wxDefaultPosition

class GridOptionDetails_impl;  // Forward Declaration

class GridOptionDetails: public wxGrid {
  friend GridOptionDetails_impl;
public:

  GridOptionDetails(void);
  GridOptionDetails( 
    wxWindow* parent, wxWindowID id = GRID_OPTIONDETAILS_IDNAME, 
    const wxPoint& pos = GRID_OPTIONDETAILS_POSITION, 
    const wxSize& size = GRID_OPTIONDETAILS_SIZE, 
    long style = GRID_OPTIONDETAILS_STYLE,
    const wxString& = GRID_OPTIONDETAILS_TITLE );
  ~GridOptionDetails(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = GRID_OPTIONDETAILS_IDNAME, 
    const wxPoint& pos = GRID_OPTIONDETAILS_POSITION, 
    const wxSize& size = GRID_OPTIONDETAILS_SIZE, 
    long style = GRID_OPTIONDETAILS_STYLE,
    const wxString& = GRID_OPTIONDETAILS_TITLE );
  
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
    ID_Null=wxID_HIGHEST, ID_GRID_OPTIONDETAILS
  };

  std::unique_ptr<GridOptionDetails_impl> m_pimpl;
  
  template<class Archive>
  void serialize(Archive & ar, const unsigned int file_version);

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};

} // namespace tf
} // namespace ou
