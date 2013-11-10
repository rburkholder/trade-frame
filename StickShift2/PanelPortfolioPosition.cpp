/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "PanelPortfolioPosition.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelPortfolioPosition::PanelPortfolioPosition(void) {
  Init();
}

PanelPortfolioPosition::PanelPortfolioPosition( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelPortfolioPosition::~PanelPortfolioPosition(void) {
}

void PanelPortfolioPosition::Init() {

    m_sizerMain = NULL;
    m_sizerPortfolio = NULL;
    m_lblIdPortfolio = NULL;
    m_lblCurrency = NULL;
    m_lblDescription = NULL;
    m_gridPortfolioStats = NULL;
    m_txtUnRealizedPL = NULL;
    m_txtCommission = NULL;
    m_txtRealizedPL = NULL;
    m_txtTotal = NULL;
    m_gridPositions = NULL;

    m_menuGridLabelPositionPopUp = NULL;
    m_menuGridCellPositionPopUp = NULL;

}

void PanelPortfolioPosition::SetPortfolio( pPortfolio_t pPortfolio ) {
  m_pPortfolio = pPortfolio;
  m_lblIdPortfolio->SetLabelText( pPortfolio->GetRow().idPortfolio );
  m_lblCurrency->SetLabelText( pPortfolio->GetRow().sCurrency );
  m_lblDescription->SetLabelText( pPortfolio->GetRow().sDescription );
  if ( ou::tf::Portfolio::Master == pPortfolio->GetRow().ePortfolioType ) {
    //m_gridPositions->Hide();
    //m_sizerMain->Detach( m_gridPositions );
    //m_sizerMain->Remove( m_gridPositions );
//    delete m_gridPositions;
//    m_gridPositions = 0;
    //this->GetParent()->RemoveChild( m_gridPositions );
    m_gridPositions->Enable( false );
//    m_sizerMain->Layout();
    //m_gridPositions->Destroy();
  }
}

bool PanelPortfolioPosition::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())     {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

void PanelPortfolioPosition::CreateControls() {    

    PanelPortfolioPosition* itemPanel1 = this;

    m_sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(m_sizerMain);

    m_sizerPortfolio = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(m_sizerPortfolio, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_sizerPortfolio->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxALL, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    m_lblIdPortfolio = new wxStaticText( itemPanel1, ID_LblIdPortfolio, _("portfolio"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_lblIdPortfolio, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblCurrency = new wxStaticText( itemPanel1, ID_LblCurrency, _("currency"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_lblCurrency, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblDescription = new wxStaticText( itemPanel1, ID_LblDescription, _("description"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer4->Add(m_lblDescription, 1, wxALIGN_LEFT|wxALL, 5);

    m_gridPortfolioStats = new wxFlexGridSizer(2, 4, 0, 0);
    m_sizerPortfolio->Add(m_gridPortfolioStats, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, ID_LblUnrealizedPL, _("UnRealized PL:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText10, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtUnRealizedPL = new wxTextCtrl( itemPanel1, ID_TxtUnRealizedPL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(m_txtUnRealizedPL, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText12 = new wxStaticText( itemPanel1, ID_LblCommission, _("Commission:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtCommission = new wxTextCtrl( itemPanel1, ID_TxtCommission, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(m_txtCommission, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText14 = new wxStaticText( itemPanel1, ID_LblRealizedPL, _("Realized PL:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText14, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtRealizedPL = new wxTextCtrl( itemPanel1, ID_TxtRealizedPL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(m_txtRealizedPL, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText16 = new wxStaticText( itemPanel1, ID_LblTotal, _("Total:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText16, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtTotal = new wxTextCtrl( itemPanel1, ID_TxtTotal, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(m_txtTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_gridPortfolioStats->AddGrowableCol(1);

    m_gridPositions = new wxGrid( itemPanel1, ID_GridPositions, wxDefaultPosition, wxSize(-1, -1), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    m_gridPositions->SetDefaultColSize(75);
    m_gridPositions->SetDefaultRowSize(22);
    m_gridPositions->SetColLabelSize(22);
    m_gridPositions->SetRowLabelSize(0);
    m_gridPositions->CreateGrid(0, 7, wxGrid::wxGridSelectCells);

    m_gridPositions->SetColLabelValue( 0, "Position" );
    m_gridPositions->SetColLabelValue( 1, "Quan" );
    m_gridPositions->SetColLabelValue( 2, "ConsValue" );
    m_gridPositions->SetColLabelValue( 3, "MktValue" );
    m_gridPositions->SetColLabelValue( 4, "UnRealPL" );
    m_gridPositions->SetColLabelValue( 5, "RealPL" );
    m_gridPositions->SetColLabelValue( 6, "Comm." );

    m_sizerMain->Add(m_gridPositions, 1, wxALIGN_LEFT|wxALL, 5);

  Bind( wxEVT_CLOSE_WINDOW, &PanelPortfolioPosition::OnClose, this );  // start close of windows and controls

  m_menuGridLabelPositionPopUp = new wxMenu;
  m_menuGridLabelPositionPopUp->Append( ID_MenuAddPosition, "Add Position" );
  m_menuGridLabelPositionPopUp->Append( ID_MenuAddPosition, "Add Portfolio" );
  m_menuGridLabelPositionPopUp->Append( ID_MenuClosePortfolio, "Close Portfolio" );

  m_menuGridCellPositionPopUp = new wxMenu;
  m_menuGridCellPositionPopUp->Append( ID_MenuAddPosition, "Add Position" );
  m_menuGridCellPositionPopUp->Append( ID_MenuClosePosition, "Close Position" );
  m_menuGridCellPositionPopUp->Append( ID_MenuAddPosition, "Add Portfolio" );
  m_menuGridCellPositionPopUp->Append( ID_MenuClosePortfolio, "Close Portfolio" );
  
  Bind( wxEVT_GRID_LABEL_RIGHT_CLICK, &PanelPortfolioPosition::OnRightClickGridLabel, this ); // add in object for each row, column, cell?
  Bind( wxEVT_GRID_CELL_RIGHT_CLICK, &PanelPortfolioPosition::OnRightClickGridCell, this ); // add in object for each row, column, cell?
  Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelPortfolioPosition::OnPositionPopUpAddPosition, this, ID_MenuAddPosition, -1, 0 );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelPortfolioPosition::OnPositionPopUpClosePosition, this, ID_MenuClosePosition, -1, 0 );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelPortfolioPosition::OnPositionPopUpAddPortfolio, this, ID_MenuAddPortfolio, -1, 0 );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &PanelPortfolioPosition::OnPositionPopUpClosePortfolio, this, ID_MenuClosePortfolio, -1, 0 );

}

void PanelPortfolioPosition::OnRightClickGridLabel( wxGridEvent& event ) {
  this->PopupMenu( m_menuGridLabelPositionPopUp );
}

void PanelPortfolioPosition::OnRightClickGridCell( wxGridEvent& event ) {
  this->PopupMenu( m_menuGridCellPositionPopUp );
}

void PanelPortfolioPosition::OnPositionPopUpAddPosition( wxCommandEvent& event ) {
  std::cout << "add position" << std::endl;
}

void PanelPortfolioPosition::OnPositionPopUpClosePosition( wxCommandEvent& event ) {
  std::cout << "close position"  << std::endl;
}

void PanelPortfolioPosition::OnPositionPopUpAddPortfolio( wxCommandEvent& event ) {
  std::cout << "add portfoio" << std::endl;
}

void PanelPortfolioPosition::OnPositionPopUpClosePortfolio( wxCommandEvent& event ) {
  std::cout << "close portfoio" << std::endl;
}

void PanelPortfolioPosition::OnClose( wxCloseEvent& event ) {

  if ( 0 != m_menuGridLabelPositionPopUp ) {
    delete m_menuGridLabelPositionPopUp;
  }

  if ( 0 != m_menuGridCellPositionPopUp ) {
    delete m_menuGridCellPositionPopUp;
  }

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();

}


wxBitmap PanelPortfolioPosition::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelPortfolioPosition::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
