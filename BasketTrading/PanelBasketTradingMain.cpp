#include "StdAfx.h"
#include "PanelBasketTradingMain.h"


PanelBasketTradingMain::PanelBasketTradingMain(void) {
  Init();
}

PanelBasketTradingMain::PanelBasketTradingMain(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelBasketTradingMain::~PanelBasketTradingMain(void) {
}

bool PanelBasketTradingMain::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  wxPanel::Create( parent, id, pos, size, style );
  CreateControls();
  if (GetSizer())
  {
      GetSizer()->SetSizeHints(this);
  }
//  Centre();
  return true;
}

void PanelBasketTradingMain::Init( void ) {
    m_btnStart = NULL;
    m_btnExitPosition = NULL;
    m_btnStop = NULL;
}

void PanelBasketTradingMain::CreateControls() {    
////@begin PanelBasketTradingMain content construction
    PanelBasketTradingMain* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnStart = new wxButton( itemPanel1, ID_BtnStart, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    //m_btnStart->Enable(false);
    itemBoxSizer3->Add(m_btnStart, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnExitPosition = new wxButton( itemPanel1, ID_BtnExitPositions, _("Exit Position"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnExitPosition->Enable(false);
    itemBoxSizer5->Add(m_btnExitPosition, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnStop = new wxButton( itemPanel1, ID_BtnStop, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnStop->Enable(false);
    itemBoxSizer7->Add(m_btnStop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end PanelBasketTradingMain content construction

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelBasketTradingMain::OnBtnStart, this, ID_BtnStart );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelBasketTradingMain::OnBtnExitPositions, this, ID_BtnExitPositions );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelBasketTradingMain::OnBtnStop, this, ID_BtnStop );

}

void PanelBasketTradingMain::OnBtnStart( wxCommandEvent& event ) {
  m_btnStart->Enable( false );
  m_btnExitPosition->Enable( true );
  m_btnStop->Enable( true );
  if ( 0 != m_OnBtnStart ) m_OnBtnStart();
}

void PanelBasketTradingMain::OnBtnExitPositions( wxCommandEvent& event ) {
  m_btnExitPosition->Enable( false );
  m_btnStop->Enable( true );
  if ( 0 != m_OnBtnExitPositions ) m_OnBtnExitPositions();
}

void PanelBasketTradingMain::OnBtnStop( wxCommandEvent& event ) {
  m_btnExitPosition->Enable( false );
  m_btnStop->Enable( false );
  if ( 0 != m_OnBtnStop ) m_OnBtnStop();
}

