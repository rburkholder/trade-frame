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

//#include "stdafx.h"

#include <functional>

#include <wx/textctrl.h>
#include <wx/menu.h>

#include "PanelPortfolioPosition_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelPortfolioPosition_impl::PanelPortfolioPosition_impl( PanelPortfolioPosition& ppp ): m_ppp( ppp ) {

  m_bDialogActive = false;

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

    //m_pdialogInstrumentSelect = 0;
    m_pdialogSimpleOneLineOrder = 0;

    m_nRowRightClick = -1;

}

PanelPortfolioPosition_impl::~PanelPortfolioPosition_impl( void ) {
}

void PanelPortfolioPosition_impl::CreateControls() {    

    PanelPortfolioPosition* itemPanel1 = &m_ppp;

    m_sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(m_sizerMain);

    m_sizerPortfolio = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(m_sizerPortfolio, 0, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_sizerPortfolio->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxLEFT|wxRIGHT|wxBOTTOM, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 0);

    m_lblIdPortfolio = new wxStaticText( itemPanel1, m_ppp.ID_LblIdPortfolio, _("portfolio"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_lblIdPortfolio, 0, wxALIGN_LEFT|wxALL, 5);

    m_lblCurrency = new wxStaticText( itemPanel1, m_ppp.ID_LblCurrency, _("currency"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_lblCurrency, 0, wxALIGN_LEFT|wxALL, 5);

    m_gridPortfolioStats = new wxFlexGridSizer(2, 4, 0, 0);
    m_sizerPortfolio->Add(m_gridPortfolioStats, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel1, m_ppp.ID_LblUnrealizedPL, _("UnRealized PL:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtUnRealizedPL = new wxTextCtrl( itemPanel1, m_ppp.ID_TxtUnRealizedPL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtUnRealizedPL, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel1, m_ppp.ID_LblCommission, _("Commission:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtCommission = new wxTextCtrl( itemPanel1, m_ppp.ID_TxtCommission, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtCommission, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel1, m_ppp.ID_LblRealizedPL, _("Realized PL:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtRealizedPL = new wxTextCtrl( itemPanel1, m_ppp.ID_TxtRealizedPL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtRealizedPL, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel1, m_ppp.ID_LblTotal, _("Total:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtTotal = new wxTextCtrl( itemPanel1, m_ppp.ID_TxtTotal, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_gridPortfolioStats->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(itemBoxSizer17, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_lblDescription = new wxStaticText( itemPanel1, m_ppp.ID_LblDescription, _("Desc:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer17->Add(m_lblDescription, 0, wxALIGN_TOP|wxALL, 2);

    m_txtDescription = new wxTextCtrl( itemPanel1, m_ppp.ID_TxtDescription, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer17->Add(m_txtDescription, 1, wxGROW|wxLEFT|wxRIGHT, 1);

    m_gridPositions = new wxGrid( itemPanel1, m_ppp.ID_GridPositions, wxDefaultPosition, wxSize(-1, 22 * 4), wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL );
    m_gridPositions->SetDefaultColSize(75);
    m_gridPositions->SetDefaultRowSize(22);
    m_gridPositions->SetColLabelSize(22);
    m_gridPositions->SetRowLabelSize(0);
    //m_sizerMain->Add(m_gridPositions, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 2);
    m_sizerMain->Add(m_gridPositions, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 2);

  m_gridPositions->CreateGrid(0, GRID_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);

    //m_sizerMain->Add(m_gridPositions, 1, wxGROW|wxALIGN_LEFT|wxALL|wxEXPAND, 2);
    //m_gridPositions->CreateGrid(0, GRID_POSITION_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);
    //m_gridPositions = new wxGrid( itemPanel1, ID_GridPositions, wxDefaultPosition, wxSize(-1, 22 * 4), wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL );  // wxSUNKEN_BORDER|

  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  m_ppp.Bind( wxEVT_CLOSE_WINDOW, &PanelPortfolioPosition_impl::OnClose, this );  // start close of windows and controls

  m_menuGridLabelPositionPopUp = new wxMenu;
  m_menuGridLabelPositionPopUp->Append( m_ppp.ID_MenuAddPosition, "Add Position" );
  m_menuGridLabelPositionPopUp->Append( m_ppp.ID_MenuAddPortfolio, "Add Portfolio" );
  m_menuGridLabelPositionPopUp->Append( m_ppp.ID_MenuClosePortfolio, "Close Portfolio" );

  m_menuGridCellPositionPopUp = new wxMenu;
  m_menuGridCellPositionPopUp->Append( m_ppp.ID_MenuAddPosition, "Add Position" );
  m_menuGridCellPositionPopUp->Append( m_ppp.ID_MenuAddOrder, "Add Order" );
  m_menuGridCellPositionPopUp->Append( m_ppp.ID_MenuCancelOrders, "Cancel Orders" );
  m_menuGridCellPositionPopUp->Append( m_ppp.ID_MenuClosePosition, "Close Position" );
  m_menuGridCellPositionPopUp->Append( m_ppp.ID_MenuAddPortfolio, "Add Portfolio" );
  m_menuGridCellPositionPopUp->Append( m_ppp.ID_MenuClosePortfolio, "Close Portfolio" );
  
  m_ppp.Bind( wxEVT_GRID_LABEL_RIGHT_CLICK, &PanelPortfolioPosition_impl::OnRightClickGridLabel, this ); // add in object for each row, column, cell?
  m_ppp.Bind( wxEVT_GRID_CELL_RIGHT_CLICK,  &PanelPortfolioPosition_impl::OnRightClickGridCell, this ); // add in object for each row, column, cell?
  m_ppp.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelPortfolioPosition_impl::OnPositionPopUpAddPosition, this, m_ppp.ID_MenuAddPosition, -1, 0 );
  m_ppp.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelPortfolioPosition_impl::OnPositionPopUpAddOrder, this, m_ppp.ID_MenuAddOrder, -1, 0 );
  m_ppp.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelPortfolioPosition_impl::OnPositionPopUpCancelOrders, this, m_ppp.ID_MenuCancelOrders, -1, 0 );
  m_ppp.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelPortfolioPosition_impl::OnPositionPopUpClosePosition, this, m_ppp.ID_MenuClosePosition, -1, 0 );
  m_ppp.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelPortfolioPosition_impl::OnPositionPopUpAddPortfolio, this, m_ppp.ID_MenuAddPortfolio, -1, 0 );
  m_ppp.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelPortfolioPosition_impl::OnPositionPopUpClosePortfolio, this, m_ppp.ID_MenuClosePortfolio, -1, 0 );

  m_vPortfolioValues.resize( 4 );

}

void PanelPortfolioPosition_impl::SetPortfolio( pPortfolio_t pPortfolio ) {
  m_pPortfolio = pPortfolio;
  m_lblIdPortfolio->SetLabelText( pPortfolio->GetRow().idPortfolio );
  m_lblCurrency->SetLabelText( pPortfolio->GetRow().sCurrency );
  m_txtDescription->SetValue( pPortfolio->GetRow().sDescription );
  pPortfolio->OnUnRealizedPLUpdate.Add( MakeDelegate( this, &PanelPortfolioPosition_impl::HandleOnUnRealizedPLUpdate ) );
  pPortfolio->OnExecutionUpdate.Add( MakeDelegate( this, &PanelPortfolioPosition_impl::HandleOnExecutionUpdate ) );
  pPortfolio->OnCommissionUpdate.Add( MakeDelegate( this, &PanelPortfolioPosition_impl::HandleOnCommissionUpdate ) );
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
    
//  m_ppp.SetAutoLayout( true );
//  m_ppp.Layout();
//  wxSize size = m_ppp.GetSize();
//  size.x += 10;
//  m_ppp.SetSize( size );
  }
}

void PanelPortfolioPosition_impl::HandleOnUnRealizedPLUpdate( const Portfolio& ) {
}

void PanelPortfolioPosition_impl::HandleOnExecutionUpdate( const Portfolio& ) {
}

void PanelPortfolioPosition_impl::HandleOnCommissionUpdate( const Portfolio& ) {
}

void PanelPortfolioPosition_impl::OnRightClickGridLabel( wxGridEvent& event ) {
  m_ppp.PopupMenu( m_menuGridLabelPositionPopUp );
}

void PanelPortfolioPosition_impl::OnRightClickGridCell( wxGridEvent& event ) {
  m_nRowRightClick = event.GetRow();
  m_ppp.PopupMenu( m_menuGridCellPositionPopUp );
}

void PanelPortfolioPosition_impl::OnPositionPopUpAddPosition( wxCommandEvent& event ) {
  pInstrument_t pInstrument = m_ppp.m_fSelectInstrument();
  if ( 0 != pInstrument.use_count() ) {
    namespace ph = std::placeholders;
    m_ppp.m_fConstructPosition( pInstrument, m_pPortfolio, 
      std::bind( &PanelPortfolioPosition_impl::AddPosition, this, ph::_1 ) );
  }
}

//void PanelPortfolioPosition_impl::OnDialogInstrumentSelectDone( ou::tf::DialogBase::DataExchange* ) {
//  m_pdialogInstrumentSelect->SetOnDoneHandler( 0 );
//  m_pdialogInstrumentSelect->SetDataExchange( 0 );
//  if ( m_DialogInstrumentSelect_DataExchange.bOk ) {
//    std::cout << "Requested symbol: " << m_DialogInstrumentSelect_DataExchange.sSymbolName << std::endl;
//    std::string s( m_DialogInstrumentSelect_DataExchange.sSymbolName );
//    if ( 0 != m_delegateConstructPosition ) {
//      m_delegateConstructPosition( s, m_pPortfolio, MakeDelegate( this, &PanelPortfolioPosition_impl::AddPosition ) ); 
//    }
//  }
//  m_pdialogInstrumentSelect->Destroy();
//  m_pdialogInstrumentSelect = 0;
//  m_bDialogActive = false;
//}

void PanelPortfolioPosition_impl::OnPositionPopUpAddOrder( wxCommandEvent& event ) {
  std::cout << "add order" << std::endl;
  if ( !m_bDialogActive ) {
    m_bDialogActive = true;
    m_pdialogSimpleOneLineOrder = new ou::tf::DialogSimpleOneLineOrder( &m_ppp );
    m_pdialogSimpleOneLineOrder->SetDataExchange( &m_DialogSimpleOneLineOrder_DataExchange );
    m_pdialogSimpleOneLineOrder->SetOnDoneHandler( MakeDelegate( this, &PanelPortfolioPosition_impl::OnDialogSimpleOneLineOrderDone ) );
    m_pdialogSimpleOneLineOrder->Show( true );
  }
}

void PanelPortfolioPosition_impl::OnPositionPopUpCancelOrders( wxCommandEvent& event ) {
  m_vPositions[ m_nRowRightClick ].GetPosition()->CancelOrders();
  std::cout << "cancel orders" << std::endl;
}

void PanelPortfolioPosition_impl::OnPositionPopUpClosePosition( wxCommandEvent& event ) {
  m_vPositions[ m_nRowRightClick ].GetPosition()->ClosePosition();
  std::cout << "close position"  << std::endl;
}

void PanelPortfolioPosition_impl::OnPositionPopUpAddPortfolio( wxCommandEvent& event ) {
  std::cout << "add portfoio" << std::endl;
  if ( !m_bDialogActive ) {
    m_bDialogActive = true;
    m_pdialogNewPortfolio = new ou::tf::DialogNewPortfolio( &m_ppp );
    m_pdialogNewPortfolio->SetDataExchange( &m_DialogNewPortfolio_DataExchange );
    m_pdialogNewPortfolio->SetOnDoneHandler( MakeDelegate( this, &PanelPortfolioPosition_impl::OnDialogNewPortfolioDone ) );
    m_pdialogNewPortfolio->Show( true );
  }
}

void PanelPortfolioPosition_impl::OnPositionPopUpClosePortfolio( wxCommandEvent& event ) {
  std::cout << "close portfoio" << std::endl;
}

void PanelPortfolioPosition_impl::OnDialogNewPortfolioDone( ou::tf::DialogBase::DataExchange* ) {
  m_pdialogNewPortfolio->SetOnDoneHandler( 0 );
  m_pdialogNewPortfolio->SetDataExchange( 0 );
  if ( m_DialogNewPortfolio_DataExchange.bOk ) {
    if ( nullptr != m_ppp.m_fConstructPortfolio ) {
      std::string sPortfolioId( m_DialogNewPortfolio_DataExchange.sPortfolioId );
      std::string sDescription( m_DialogNewPortfolio_DataExchange.sDescription );
      m_ppp.m_fConstructPortfolio( m_ppp, sPortfolioId, sDescription );
    }
  }
  m_pdialogNewPortfolio->Destroy();
  m_pdialogNewPortfolio = 0;
  m_bDialogActive = false;
}

void PanelPortfolioPosition_impl::OnDialogSimpleOneLineOrderDone( ou::tf::DialogBase::DataExchange* ) {
  if ( m_DialogSimpleOneLineOrder_DataExchange.bOk ) {
    // compose order and send it off
    // need to know for which position the order is meant
    ou::tf::OrderSide::enumOrderSide eOrderSide;
//    ou::tf::OrderType::enumOrderType eOrderType;
    pPosition_t pPosition( m_vPositions[ m_nRowRightClick ].GetPosition() );
    bool bOk( true );
    bOk = m_nRowRightClick < m_vPositions.size();
    if ( bOk ) {
      if ( "BUY" == m_DialogSimpleOneLineOrder_DataExchange.sBuySell ) {
        eOrderSide = ou::tf::OrderSide::Buy;
      }
      else {
        if ( "SELL" == m_DialogSimpleOneLineOrder_DataExchange.sBuySell ) {
          eOrderSide = ou::tf::OrderSide::Sell;
        }
        else {
          std::cout << "Unknown buy/sell type" << std::endl;
          bOk = false;
        }
      }
    }
    if ( bOk ) {
      bOk = ( 0 < m_DialogSimpleOneLineOrder_DataExchange.nQuantity );
    }
    if ( bOk ) {
      if ( "MKT" == m_DialogSimpleOneLineOrder_DataExchange.sLmtMktStp ) {
        pPosition->PlaceOrder( OrderType::Market, eOrderSide, m_DialogSimpleOneLineOrder_DataExchange.nQuantity );
      }
      else {
        if ( 0.0 >= m_DialogSimpleOneLineOrder_DataExchange.dblPrice1 ) {
          std::cout << "Price1 not greater than 0.0" << std::endl;
          bOk = false;
        }
        else {
          if ( "LMT" == m_DialogSimpleOneLineOrder_DataExchange.sLmtMktStp ) {
              pPosition->PlaceOrder( OrderType::Limit, eOrderSide, m_DialogSimpleOneLineOrder_DataExchange.nQuantity, m_DialogSimpleOneLineOrder_DataExchange.dblPrice1 );
          }
          else {
            if ( "STP" == m_DialogSimpleOneLineOrder_DataExchange.sLmtMktStp ) {
              pPosition->PlaceOrder( OrderType::Stop, eOrderSide, m_DialogSimpleOneLineOrder_DataExchange.nQuantity, m_DialogSimpleOneLineOrder_DataExchange.dblPrice1 );
            }
            else {
              std::cout << "Unknown order type" << std::endl;
              bOk = false;
            } // unknown order type
          } // not limit
        }
      }
    }
  }
  m_pdialogSimpleOneLineOrder->SetOnDoneHandler( 0 );
  m_pdialogSimpleOneLineOrder->SetDataExchange( 0 );
  m_pdialogSimpleOneLineOrder->Destroy();
  m_pdialogSimpleOneLineOrder = 0;
  m_bDialogActive = false;
}

void PanelPortfolioPosition_impl::AddPosition( pPosition_t pPosition ) {

  m_gridPositions->AppendRows( 1 );

  int row( m_vPositions.size() );

  m_vPositions.push_back( structPosition( pPosition, *m_gridPositions, row ) );

  UpdateGui();
}

void PanelPortfolioPosition_impl::UpdateGui( void ) {

  for ( vPositions_t::iterator iter = m_vPositions.begin(); m_vPositions.end() != iter; ++iter ) {
    // todo maybe use BeginBatch/EndBatch on grid?  Creates even more flicker.  Things are good for now.
    iter->UpdateGui();
  }

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;
  m_pPortfolio->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );

  m_vPortfolioValues[ 0 ].SetValue( dblUnRealized );
  if ( m_vPortfolioValues[ 0 ].Changed() ) m_txtUnRealizedPL->SetValue( m_vPortfolioValues[ 0 ].GetText() );

  m_vPortfolioValues[ 1 ].SetValue( dblRealized );
  if ( m_vPortfolioValues[ 1 ].Changed() ) m_txtRealizedPL  ->SetValue( m_vPortfolioValues[ 1 ].GetText() );

  m_vPortfolioValues[ 2 ].SetValue( dblCommissionsPaid );
  if ( m_vPortfolioValues[ 2 ].Changed() ) m_txtCommission  ->SetValue( m_vPortfolioValues[ 2 ].GetText() );

  m_vPortfolioValues[ 3 ].SetValue( dblTotal );
  if ( m_vPortfolioValues[ 3 ].Changed() ) m_txtTotal       ->SetValue( m_vPortfolioValues[ 3 ].GetText() );
  /*
  m_pPanelPortfolioStats->SetStats( 
    boost::lexical_cast<std::string>( m_dblMinPL ),
    boost::lexical_cast<std::string>( dblCurrent ),
    boost::lexical_cast<std::string>( m_dblMaxPL )
    );
    */
}

void PanelPortfolioPosition_impl::OnClose( wxCloseEvent& event ) {

  // todo:  don't close if dialog is still open.

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

} // namespace tf
} // namespace ou
