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

#include "GridOptionChain.h"
#include "GridOptionChain_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionChain::GridOptionChain(): wxGrid() {
  Init();
}

GridOptionChain::GridOptionChain(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle )
: wxGrid(  )
{
  Init();
  Create(parent, id, pos, size, style, sTitle );
}

GridOptionChain::~GridOptionChain() {
}

void GridOptionChain::Init() {
}

bool GridOptionChain::Create(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

  wxGrid::Create(parent, id, pos, size, style, name );
  m_pimpl = std::make_unique<GridOptionChain_impl>( *this);

  CreateControls();

  return true;
}

void GridOptionChain::CreateControls() {

  //Bind( wxEVT_CLOSE_WINDOW, &GridOptionDetails::OnClose, this );  // not called for child windows
  Bind( wxEVT_DESTROY, &GridOptionChain::OnDestroy, this );

  //Bind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
  //Bind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );

  //Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
  //Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );
  //Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

  //Bind( EVENT_DRAW_CHART, &WinChartView::HandleGuiDrawChart, this );

  // this GuiRefresh initialization should come after all else
  //m_timerGuiRefresh.SetOwner( this );
  //Bind( wxEVT_TIMER, &GridOptionDetails::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  //m_timerGuiRefresh.Start( 250 );

  m_pimpl->CreateControls();

}

void GridOptionChain::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Quote& quote ) {
  m_pimpl->Update( strike, side, quote );
}

void GridOptionChain::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Trade& trade ) {
  m_pimpl->Update( strike, side, trade );
}

void GridOptionChain::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Greek& greek ) {
  m_pimpl->Update( strike, side, greek );
}

void GridOptionChain::Clear(  double strike ) {}

void GridOptionChain::SaveColumnSizes( ou::tf::GridColumnSizer& gcs ) const {
  gcs.SaveColumnSizes( *this );
}

void GridOptionChain::SetColumnSizes( ou::tf::GridColumnSizer& gcs ) {
 gcs.SetColumnSizes( *this );
}

void GridOptionChain::Start() { m_pimpl->Start(); }
void GridOptionChain::Stop() { m_pimpl->Stop(); }

void GridOptionChain::Add( double strike, ou::tf::OptionSide::EOptionSide side, const std::string& sSymbol ) {
  m_pimpl->Add( strike, side, sSymbol );
}

void GridOptionChain::MakeRowVisible( double strike ) {
  m_pimpl->MakeRowVisible( strike );
}

void GridOptionChain::SetSelected( double strike, bool bSelected) {
  m_pimpl->SetSelected( strike, bSelected );
}

template void GridOptionChain::serialize<boost::archive::text_iarchive>(
    boost::archive::text_iarchive & ar,
    const unsigned int file_version
);

template void GridOptionChain::serialize<boost::archive::text_oarchive>(
    boost::archive::text_oarchive & ar,
    const unsigned int file_version
);

void GridOptionChain::HandleSize( wxSizeEvent& event ) {
}

void GridOptionChain::PreDestroy() {
  m_pimpl->StopWatch();
}

void GridOptionChain::OnDestroy( wxWindowDestroyEvent& event ) {

  //m_timerGuiRefresh.Stop();
  //Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  //m_pimpl->StopWatch();
  m_pimpl->DestroyControls();
  m_pimpl.reset();

  Unbind( wxEVT_DESTROY, &GridOptionChain::OnDestroy, this );

  //Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
  //Unbind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );

  //Unbind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
  //Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
  //Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );
  //Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

  event.Skip();  // auto followed by Destroy();
}

} // namespace tf
} // namespace ou
