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

#include <functional>

#include <wx/grid.h>

#include <TFOptions/Option.h>
#include <TFOptions/OptionDelegates.hpp>

#include <TFVuTrading/GridColumnSizer.h>
#include <TFVuTrading/DragDropInstrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define GRID_OPTIONCHAIN_STYLE wxWANTS_CHARS
#define GRID_OPTIONCHAIN_TITLE _("Grid Option Chain")
#define GRID_OPTIONCHAIN_IDNAME ID_GRID_OPTIONCHAIN
#define GRID_OPTIONCHAIN_SIZE wxSize(-1, -1)
#define GRID_OPTIONCHAIN_POSITION wxDefaultPosition

class GridOptionChain_impl;  // Forward Declaration

class GridOptionChain: public wxGrid {
  friend GridOptionChain_impl;
public:

  GridOptionChain();
  GridOptionChain(
    wxWindow* parent, wxWindowID id = GRID_OPTIONCHAIN_IDNAME,
    const wxPoint& pos = GRID_OPTIONCHAIN_POSITION,
    const wxSize& size = GRID_OPTIONCHAIN_SIZE,
    long style = GRID_OPTIONCHAIN_STYLE,
    const wxString& = GRID_OPTIONCHAIN_TITLE );
  virtual ~GridOptionChain();

  bool Create( wxWindow* parent,
    wxWindowID id = GRID_OPTIONCHAIN_IDNAME,
    const wxPoint& pos = GRID_OPTIONCHAIN_POSITION,
    const wxSize& size = GRID_OPTIONCHAIN_SIZE,
    long style = GRID_OPTIONCHAIN_STYLE,
    const wxString& = GRID_OPTIONCHAIN_TITLE );

  void Add( double strike, ou::tf::OptionSide::EOptionSide side, const std::string& sSymbol );

  void MakeRowVisible( double strike );

  void SetSelected( double strike, bool bSelected );

  void Update( double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Quote& );
  void Update( double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Trade& );
  void Update( double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Greek& );

  void Clear(  double strike );

  void Start();
  void Stop();

  void SaveColumnSizes( ou::tf::GridColumnSizer& ) const;
  void SetColumnSizes( ou::tf::GridColumnSizer& );

  void PreDestroy();

  using fOptionDelegates_t = std::function<void( ou::tf::option::Delegates& call, ou::tf::option::Delegates& put)>;
  fOptionDelegates_t m_fOptionDelegates_Attach;
  fOptionDelegates_t m_fOptionDelegates_Detach;

  using fOnRowClicked_t = std::function<void(double, bool bSelected, const ou::tf::option::Delegates& call, const ou::tf::option::Delegates& put )>;
  fOnRowClicked_t m_fOnRowClicked; // called when a row is clicked (on/off)

  using fOnOptionUnderlyingRetrieveComplete_t = DragDropInstrument::fOnOptionUnderlyingRetrieveComplete_t;
  using fOnOptionUnderlyingRetrieveInitiate_t = std::function<void(const std::string&, double, fOnOptionUnderlyingRetrieveComplete_t&&)>; // IQFeed Option Symbol, strike, completion function
  fOnOptionUnderlyingRetrieveInitiate_t m_fOnOptionUnderlyingRetrieveInitiate;  // called when DropTarget wants instrument

protected:

  void Init();
  void CreateControls();

private:
  enum {
    ID_Null=wxID_HIGHEST, ID_GRID_OPTIONCHAIN
  };

  std::unique_ptr<GridOptionChain_impl> m_pimpl;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int file_version);

  void HandleSize( wxSizeEvent& event );
  void OnDestroy( wxWindowDestroyEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};

} // namespace tf
} // namespace ou
