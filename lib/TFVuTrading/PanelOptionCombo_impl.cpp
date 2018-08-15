/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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

#include <set>
#include <functional>

#include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/statline.h>

#include <TFVuTrading/DragDropInstrumentTarget.h>
#include <wx-3.0/wx/mousestate.h>

#include "PanelOptionCombo_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelOptionCombo_impl::PanelOptionCombo_impl( PanelOptionCombo& poc )
: 
  m_poc( poc )
{

  m_bDialogActive = false;

    m_sizerMain = NULL;
    m_sizerHeader = NULL;
    m_lblCurrency = NULL;
    m_lblIdPortfolio = NULL;
    m_txtDescription = NULL;
    m_sizerPortfolioStats = NULL;
    m_gridPortfolioStats = NULL;
    m_sizerGridPositions = NULL;
    m_gridPositions = NULL;

    m_menuGridLabelPositionPopUp = NULL;
    m_menuGridCellPositionPopUp = NULL;

    //m_pdialogInstrumentSelect = 0;
    m_pdialogSimpleOneLineOrder = 0;

    m_nRowRightClick = -1;

}

PanelOptionCombo_impl::~PanelOptionCombo_impl( void ) {
  std::for_each( m_vPositions.begin(), m_vPositions.end(), [this]( vPositions_t::value_type& vt ){
    if ( nullptr != m_poc.m_fRemoveFromEngine ) {
      m_poc.m_fRemoveFromEngine( vt.GetPositionGreek()->GetOption(), vt.GetPositionGreek()->GetUnderlying() );
    }
  });
  m_vPositions.clear();
}

void PanelOptionCombo_impl::CreateControls() {    

    PanelOptionCombo* itemPanel1 = &m_poc;

    m_sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(m_sizerMain);

    m_sizerHeader = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(m_sizerHeader, 0, wxGROW|wxALL, 1);

    m_lblCurrency = new wxStaticText( itemPanel1, m_poc.ID_LblCurrency, _("currency"), wxDefaultPosition, wxDefaultSize, 0 );
    m_sizerHeader->Add(m_lblCurrency, 0, wxALIGN_TOP|wxALL, 2);

    m_lblIdPortfolio = new wxStaticText( itemPanel1, m_poc.ID_LblIdPortfolio, _("portfolio"), wxDefaultPosition, wxDefaultSize, 0 );
    m_sizerHeader->Add(m_lblIdPortfolio, 0, wxALIGN_TOP|wxALL, 2);

    m_txtDescription = new wxTextCtrl( itemPanel1, m_poc.ID_TxtDescription, _("description"), wxDefaultPosition, wxSize(-1, 30), wxTE_MULTILINE|wxTE_READONLY );
    m_sizerHeader->Add(m_txtDescription, 1, wxALIGN_TOP|wxALL, 2);

    m_sizerPortfolioStats = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(m_sizerPortfolioStats, 0, wxGROW|wxALL, 1);

    m_gridPortfolioStats = new wxGrid( itemPanel1, m_poc.ID_GridPortfolioDetails, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE );
    m_gridPortfolioStats->SetDefaultColSize(50);
    m_gridPortfolioStats->SetDefaultRowSize(25);
    m_gridPortfolioStats->SetColLabelSize(25);
    m_gridPortfolioStats->SetRowLabelSize(0);
    m_sizerPortfolioStats->Add(m_gridPortfolioStats, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 1);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(itemBoxSizer12, 0, wxGROW|wxALL, 1);

    wxStaticLine* itemStaticLine1 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer12->Add(itemStaticLine1, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 1);

    m_sizerGridPositions = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(m_sizerGridPositions, 1, wxGROW|wxALL, 1);

    m_gridPositions = new wxGrid( itemPanel1, m_poc.ID_GridPositions, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxVSCROLL );
    m_gridPositions->SetDefaultColSize(50);
    m_gridPositions->SetDefaultRowSize(22);
    m_gridPositions->SetColLabelSize(0);
    m_gridPositions->SetRowLabelSize(0);
    m_sizerGridPositions->Add(m_gridPositions, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 1);

  m_gridPortfolioStats->CreateGrid(1, GRID_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);
  m_gridPositions->CreateGrid(0, GRID_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);

  //int ix( 0 );
  //BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )
    
#undef GRID_EMIT_SetColSettings    
#define GRID_EMIT_SetColSettings( z, n, VAR ) \
  m_gridPortfolioStats->SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_gridPortfolioStats->SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );
  int ix = 0;
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  m_menuGridLabelPositionPopUp = new wxMenu;
  m_menuGridLabelPositionPopUp->Append( m_poc.ID_MenuAddPosition, "Add Greek Position" );
  m_menuGridLabelPositionPopUp->Append( m_poc.ID_MenuAddPortfolio, "Add Greek Portfolio" );
  m_menuGridLabelPositionPopUp->Append( m_poc.ID_MenuClosePortfolio, "Close Greek Portfolio" );

  m_menuGridCellPositionPopUp = new wxMenu;
  m_menuGridCellPositionPopUp->Append( m_poc.ID_MenuAddPosition, "Add Greek Position" );
  m_menuGridCellPositionPopUp->Append( m_poc.ID_MenuAddOrder, "Add Order" );
  m_menuGridCellPositionPopUp->Append( m_poc.ID_MenuCancelOrders, "Cancel Orders" );
  m_menuGridCellPositionPopUp->Append( m_poc.ID_MenuClosePosition, "Close Greek Position" );
  m_menuGridCellPositionPopUp->Append( m_poc.ID_MenuAddPortfolio, "Add Greek Portfolio" );
  m_menuGridCellPositionPopUp->Append( m_poc.ID_MenuClosePortfolio, "Close Greek Portfolio" );
  
  // watch out for the std::move operations?:  needed in some places?, not in others?
  // create empty DragDropInstrument with correct type in order to receive the desired initiate call when dropped
  // GridOptionChain_impl::OnGridCellBeginDrag creates  source DragDropInstrument
  typedef DragDropInstrument::pOptionInstrument_t pOptionInstrument_t;
  typedef DragDropInstrument::pUnderlyingInstrument_t pUnderlyingInstrument_t;
  DragDropInstrumentTarget* pddDataInstrumentTarget = new DragDropInstrumentTarget( new DragDropInstrument( DragDropInstrument::fOnOptionUnderlyingRetrieveInitiate_t() ) );
  pddDataInstrumentTarget->m_fOnOptionUnderlyingRetrieveComplete = [this]( pOptionInstrument_t pOptionInstrument, pUnderlyingInstrument_t pUnderlyingInstrument ) { 
    //std::cout << "pddDataInstrumentTarget symbol name: " << pOptionInstrument->GetInstrumentName() << std::endl; 
    AddOptionUnderlyingPosition( pOptionInstrument, pUnderlyingInstrument );
  };
  //if ( nullptr != m_ddDataInstrumentTarget.m_fOnInstrumentRetrieveInitiate ) {
  //  m_ddDataInstrumentTarget.m_fOnInstrumentRetrieveInitiate( [](pInstrument_t pInstrument){
  //    std::cout << "symbol name: " << pInstrument->GetInstrumentName() << std::endl;
  //  });
  //}
  m_poc.SetDropTarget( pddDataInstrumentTarget ); // wxDropTarget takes possession

  m_gridPositions->Bind( wxEVT_MOUSEWHEEL,             &PanelOptionCombo_impl::OnMouseWheel, this );
  m_poc.Bind( wxEVT_GRID_LABEL_RIGHT_CLICK, &PanelOptionCombo_impl::OnRightClickGridLabel, this ); // add in object for each row, column, cell?
  m_poc.Bind( wxEVT_GRID_CELL_RIGHT_CLICK,  &PanelOptionCombo_impl::OnRightClickGridCell, this ); // add in object for each row, column, cell?
  m_poc.Bind( wxEVT_GRID_COL_SIZE,          &PanelOptionCombo_impl::OnGridColSize, this );
  m_poc.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpAddPosition, this, m_poc.ID_MenuAddPosition, -1, 0 );
  m_poc.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpAddOrder, this, m_poc.ID_MenuAddOrder, -1, 0 );
  m_poc.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpCancelOrders, this, m_poc.ID_MenuCancelOrders, -1, 0 );
  m_poc.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpClosePosition, this, m_poc.ID_MenuClosePosition, -1, 0 );
  m_poc.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpAddPortfolio, this, m_poc.ID_MenuAddPortfolio, -1, 0 );
  m_poc.Bind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpClosePortfolio, this, m_poc.ID_MenuClosePortfolio, -1, 0 );
  
  m_poc.Bind( wxEVT_DESTROY, &PanelOptionCombo_impl::HandleWindowDestroy, this );

  m_vPortfolioModelCell.resize( GRID_ARRAY_COL_COUNT );
  m_vPortfolioCalcs.resize( GRID_ARRAY_COL_COUNT );

}

void PanelOptionCombo_impl::SaveColumnSizes( ou::tf::GridColumnSizer& gcs ) const {
  gcs.SaveColumnSizes( *m_gridPositions );
}

void PanelOptionCombo_impl::SetColumnSizes( ou::tf::GridColumnSizer& gcs ) {
  gcs.SetColumnSizes( *m_gridPositions );
  gcs.SetColumnSizes( *m_gridPortfolioStats );
}
	
void PanelOptionCombo_impl::HandleWindowDestroy( wxWindowDestroyEvent& event ) {
  
  m_poc.SetDropTarget( nullptr );
  
  if ( nullptr != m_menuGridLabelPositionPopUp ) {
    delete m_menuGridLabelPositionPopUp;
    m_menuGridLabelPositionPopUp = nullptr;
  }

  if ( nullptr != m_menuGridCellPositionPopUp ) {
    delete m_menuGridCellPositionPopUp;
    m_menuGridCellPositionPopUp = nullptr;
  }

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  //event.Skip();  // auto followed by Destroy();

  m_gridPositions->Unbind( wxEVT_MOUSEWHEEL,             &PanelOptionCombo_impl::OnMouseWheel, this );
  m_poc.Unbind( wxEVT_GRID_LABEL_RIGHT_CLICK, &PanelOptionCombo_impl::OnRightClickGridLabel, this ); // add in object for each row, column, cell?
  m_poc.Unbind( wxEVT_GRID_CELL_RIGHT_CLICK,  &PanelOptionCombo_impl::OnRightClickGridCell, this ); // add in object for each row, column, cell?
  m_poc.Unbind( wxEVT_GRID_COL_SIZE,          &PanelOptionCombo_impl::OnGridColSize, this );
  m_poc.Unbind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpAddPosition, this, m_poc.ID_MenuAddPosition, -1, 0 );
  m_poc.Unbind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpAddOrder, this, m_poc.ID_MenuAddOrder, -1, 0 );
  m_poc.Unbind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpCancelOrders, this, m_poc.ID_MenuCancelOrders, -1, 0 );
  m_poc.Unbind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpClosePosition, this, m_poc.ID_MenuClosePosition, -1, 0 );
  m_poc.Unbind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpAddPortfolio, this, m_poc.ID_MenuAddPortfolio, -1, 0 );
  m_poc.Unbind( wxEVT_COMMAND_MENU_SELECTED,  &PanelOptionCombo_impl::OnPositionPopUpClosePortfolio, this, m_poc.ID_MenuClosePortfolio, -1, 0 );
  
  m_poc.Unbind( wxEVT_DESTROY, &PanelOptionCombo_impl::HandleWindowDestroy, this );
}

void PanelOptionCombo_impl::SetPortfolioGreek( pPortfolioGreek_t pPortfolioGreek ) {
  m_pPortfolioGreek = pPortfolioGreek;
  m_lblIdPortfolio->SetLabelText( m_pPortfolioGreek->GetRow().idPortfolio );
  m_lblCurrency->SetLabelText( m_pPortfolioGreek->GetRow().sCurrency );
  m_txtDescription->SetValue( m_pPortfolioGreek->GetRow().sDescription );
  pPortfolioGreek->OnUnRealizedPLUpdate.Add( MakeDelegate( this, &PanelOptionCombo_impl::HandleOnUnRealizedPLUpdate ) );
  pPortfolioGreek->OnExecutionUpdate.Add( MakeDelegate( this, &PanelOptionCombo_impl::HandleOnExecutionUpdate ) );
  pPortfolioGreek->OnCommissionUpdate.Add( MakeDelegate( this, &PanelOptionCombo_impl::HandleOnCommissionUpdate ) );
  if ( ou::tf::Portfolio::Master == pPortfolioGreek->GetRow().ePortfolioType ) {
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

void PanelOptionCombo_impl::OnMouseWheel( wxMouseEvent& event ) {
  int delta = event.GetWheelDelta();
  int rotation = event.GetWheelRotation(); // has positive, negative, use delta to normalize
  bool bShift = event.ShiftDown();
  bool bControl = event.ControlDown();
  bool bAlt = event.AltDown();
  wxPoint point( event.GetPosition() );
  wxGridCellCoords coords( m_gridPositions->XYToCell( point ) );
//  std::cout 
//      << "Wheel: " << delta << "," << rotation 
//      << ",sca:" << bShift << bControl << bAlt
//      << ",pos:" << point.x << "," << point.y
//      << ",cell:" << coords.GetRow() << "," << coords.GetCol()
//      << std::endl;
  
  if ( ( 1 == coords.GetCol() ) || ( 2 == coords.GetCol() ) ) {
    m_vPositions[ coords.GetRow() ].GetPositionGreek()->PositionPendingDelta( 0 > rotation ? -1 : 1 );
  }
  
  
  //DrawChart();
  //event.Skip();
}

void PanelOptionCombo_impl::HandleOnUnRealizedPLUpdate( const Portfolio& ) {
}

void PanelOptionCombo_impl::HandleOnExecutionUpdate( const Portfolio& ) {
}

void PanelOptionCombo_impl::HandleOnCommissionUpdate( const Portfolio& ) {
}

void PanelOptionCombo_impl::OnRightClickGridLabel( wxGridEvent& event ) {
  m_poc.PopupMenu( m_menuGridLabelPositionPopUp );
}

void PanelOptionCombo_impl::OnRightClickGridCell( wxGridEvent& event ) {
  m_nRowRightClick = event.GetRow();
  m_poc.PopupMenu( m_menuGridCellPositionPopUp );
}

void PanelOptionCombo_impl::OnGridColSize( wxGridSizeEvent& event ) {
  if ( nullptr != m_poc.m_fColumnWidthChanged ) {
    m_poc.m_fColumnWidthChanged( event.GetRowOrCol(), m_gridPositions->GetColSize( event.GetRowOrCol() ), m_poc );
  }
}

//void PanelOptionCombo_impl::OnDialogInstrumentSelectDone( ou::tf::DialogBase::DataExchange* ) {
//  m_pdialogInstrumentSelect->SetOnDoneHandler( 0 );
//  m_pdialogInstrumentSelect->SetDataExchange( 0 );
//  if ( m_DialogInstrumentSelect_DataExchange.bOk ) {
//    std::cout << "Requested symbol: " << m_DialogInstrumentSelect_DataExchange.sSymbolName << std::endl;
//    std::string s( m_DialogInstrumentSelect_DataExchange.sSymbolName );
//    if ( 0 != m_delegateConstructPosition ) {
//      m_delegateConstructPosition( s, m_pPortfolio, MakeDelegate( this, &PanelOptionCombo_impl::AddPosition ) ); 
//    }
//  }
//  m_pdialogInstrumentSelect->Destroy();
//  m_pdialogInstrumentSelect = 0;
//  m_bDialogActive = false;
//}

void PanelOptionCombo_impl::OnPositionPopUpAddOrder( wxCommandEvent& event ) {
  std::cout << "add order" << std::endl;
  if ( !m_bDialogActive ) {
    m_bDialogActive = true;
    m_pdialogSimpleOneLineOrder = new ou::tf::DialogSimpleOneLineOrder( &m_poc );
    m_pdialogSimpleOneLineOrder->SetDataExchange( &m_DialogSimpleOneLineOrder_DataExchange );
    m_pdialogSimpleOneLineOrder->SetOnDoneHandler( MakeDelegate( this, &PanelOptionCombo_impl::OnDialogSimpleOneLineOrderDone ) );
    m_pdialogSimpleOneLineOrder->Show( true );
  }
}

void PanelOptionCombo_impl::OnPositionPopUpCancelOrders( wxCommandEvent& event ) {
  m_vPositions[ m_nRowRightClick ].GetPositionGreek()->CancelOrders();
  std::cout << "cancel orders" << std::endl;
}

void PanelOptionCombo_impl::OnPositionPopUpClosePosition( wxCommandEvent& event ) {
  m_vPositions[ m_nRowRightClick ].GetPositionGreek()->ClosePosition();
  std::cout << "close position"  << std::endl;
}

void PanelOptionCombo_impl::OnPositionPopUpAddPortfolio( wxCommandEvent& event ) {
  std::cout << "add portfolio" << std::endl;
  if ( !m_bDialogActive ) {
    m_bDialogActive = true;
    m_pdialogNewPortfolio = new ou::tf::DialogNewPortfolio( &m_poc );
    m_pdialogNewPortfolio->SetDataExchange( &m_DialogNewPortfolio_DataExchange );
    m_pdialogNewPortfolio->SetOnDoneHandler( MakeDelegate( this, &PanelOptionCombo_impl::OnDialogNewPortfolioDone ) );
    m_pdialogNewPortfolio->Show( true );
  }
}

void PanelOptionCombo_impl::OnPositionPopUpClosePortfolio( wxCommandEvent& event ) {
  std::cout << "close portfoio" << std::endl;
}

void PanelOptionCombo_impl::OnDialogNewPortfolioDone( ou::tf::DialogBase::DataExchange* ) {
  m_pdialogNewPortfolio->SetOnDoneHandler( 0 );
  m_pdialogNewPortfolio->SetDataExchange( 0 );
  if ( m_DialogNewPortfolio_DataExchange.bOk ) {
    if ( nullptr != m_poc.m_fConstructPortfolioGreek ) {
      std::string sPortfolioId( m_DialogNewPortfolio_DataExchange.sPortfolioId );
      std::string sDescription( m_DialogNewPortfolio_DataExchange.sDescription );
      m_poc.m_fConstructPortfolioGreek( m_poc, sPortfolioId, sDescription );
    }
  }
  m_pdialogNewPortfolio->Destroy();
  m_pdialogNewPortfolio = 0;
  m_bDialogActive = false;
}

void PanelOptionCombo_impl::OnDialogSimpleOneLineOrderDone( ou::tf::DialogBase::DataExchange* ) {
  if ( m_DialogSimpleOneLineOrder_DataExchange.bOk ) {
    // compose order and send it off
    // need to know for which position the order is meant
    ou::tf::OrderSide::enumOrderSide eOrderSide;
//    ou::tf::OrderType::enumOrderType eOrderType;
    pPositionGreek_t pPositionGreek( m_vPositions[ m_nRowRightClick ].GetPositionGreek() );
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
        pPositionGreek->PlaceOrder( OrderType::Market, eOrderSide, m_DialogSimpleOneLineOrder_DataExchange.nQuantity );
      }
      else {
        if ( 0.0 >= m_DialogSimpleOneLineOrder_DataExchange.dblPrice1 ) {
          std::cout << "Price1 not greater than 0.0" << std::endl;
          bOk = false;
        }
        else {
          if ( "LMT" == m_DialogSimpleOneLineOrder_DataExchange.sLmtMktStp ) {
              pPositionGreek->PlaceOrder( OrderType::Limit, eOrderSide, m_DialogSimpleOneLineOrder_DataExchange.nQuantity, m_DialogSimpleOneLineOrder_DataExchange.dblPrice1 );
          }
          else {
            if ( "STP" == m_DialogSimpleOneLineOrder_DataExchange.sLmtMktStp ) {
              pPositionGreek->PlaceOrder( OrderType::Stop, eOrderSide, m_DialogSimpleOneLineOrder_DataExchange.nQuantity, m_DialogSimpleOneLineOrder_DataExchange.dblPrice1 );
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

void PanelOptionCombo_impl::OnPositionPopUpAddPosition( wxCommandEvent& event ) {
  if ( nullptr != m_poc.m_fSelectInstrument ) {
    pInstrument_t pInstrument = m_poc.m_fSelectInstrument();
    std::cout << "needs rework as an underlying needs to be present" << std::endl;
//    if ( 0 != pInstrument.use_count() ) {
//      AddInstrumentToPosition( pInstrument );
//    }
  }
  else {
    std::cout << "PanelOptionCombo_impl::OnPositionPopUpAddPosition: no fSelectInstrument" << std::endl;
  }
}

//void PanelOptionCombo_impl::AddInstrumentToPosition( pInstrument_t pInstrument ) {
//  if ( nullptr != m_poc.m_fConstructPositionGreek) {
//    namespace ph = std::placeholders;
//    m_poc.m_fConstructPositionGreek( pInstrument, m_pPortfolioGreek, 
//      std::bind( &PanelOptionCombo_impl::AddPositionGreek, this, ph::_1 ) );
//  }
//  else {
//    std::cout << "PanelOptionCombo_impl::AddInstrumentToPosition: no m_fConstructPositionGreek" << std::endl;
//  }
//}

// TODO: will need to change to use various instances rather than just m_pPortfolioGreek
void PanelOptionCombo_impl::AddOptionUnderlyingPosition( pInstrument_t pOption, pInstrument_t pUnderlying ) {
  if ( nullptr != m_poc.m_fConstructPositionGreek) {
    namespace ph = std::placeholders;
    m_poc.m_fConstructPositionGreek( pOption, pUnderlying, m_pPortfolioGreek, 
      std::bind( &PanelOptionCombo_impl::AddPositionGreek, this, ph::_1 ) );
  }
  else {
    std::cout << "PanelOptionCombo_impl::AddInstrumentToPosition: no m_fConstructPositionGreek" << std::endl;
  }
}

void PanelOptionCombo_impl::AddPositionGreek( pPositionGreek_t pPositionGreek ) {

  m_gridPositions->AppendRows( 1 );

  int row( m_vPositions.size() );

  m_vPositions.push_back( structPosition( pPositionGreek, *m_gridPositions, row ) );
  if ( nullptr != m_poc.m_fRegisterWithEngine ) {
    m_poc.m_fRegisterWithEngine( pPositionGreek->GetOption(), pPositionGreek->GetUnderlying() );
  }

  UpdateGui();
}

void PanelOptionCombo_impl::UpdateGui( void ) {

  for ( vPositions_t::iterator iter = m_vPositions.begin(); m_vPositions.end() != iter; ++iter ) {
    // todo maybe use BeginBatch/EndBatch on grid?  Creates even more flicker.  Things are good for now.
    iter->UpdateGui();
  }

//  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;
//  m_pPortfolioGreek->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );

//  m_vPortfolioModelCell[ 0 ].SetValue( dblUnRealized );
//  if ( m_vPortfolioModelCell[ 0 ].Changed() ) m_txtUnRealizedPL->SetValue( m_vPortfolioModelCell[ 0 ].GetText() );

//  m_vPortfolioModelCell[ 1 ].SetValue( dblRealized );
//  if ( m_vPortfolioModelCell[ 1 ].Changed() ) m_txtRealizedPL  ->SetValue( m_vPortfolioModelCell[ 1 ].GetText() );

//  m_vPortfolioModelCell[ 2 ].SetValue( dblCommissionsPaid );
//  if ( m_vPortfolioModelCell[ 2 ].Changed() ) m_txtCommission  ->SetValue( m_vPortfolioModelCell[ 2 ].GetText() );

//  m_vPortfolioModelCell[ 3 ].SetValue( dblTotal );
//  if ( m_vPortfolioModelCell[ 3 ].Changed() ) m_txtTotal       ->SetValue( m_vPortfolioModelCell[ 3 ].GetText() );
  
  
  std::for_each( m_vPortfolioCalcs.begin(), m_vPortfolioCalcs.end(), [](vPortfolioCalcs_t::value_type& vt){ vt = 0.0; } );
  
  // TODO: migrate to m_pPortfolioGreek?
  std::for_each( m_vPositions.begin(), m_vPositions.end(), [this](const vPositions_t::value_type& vt){
    const pPositionGreek_t pPositionGreek = vt.GetPositionGreek();
    const pOption_t pOption = pPositionGreek->GetOption();
    const ou::tf::PositionGreek::TableRowDef& row( pPositionGreek->GetRow() );
    ou::tf::Quote quote( pOption->LastQuote() );
    ou::tf::Greek greek( pOption->LastGreek() );
    boost::uint32_t nPending( row.nPositionPending );
    
    m_vPortfolioCalcs[ COL_Quan ] += nPending;
    switch ( row.eOrderSidePending ) {
      case OrderSide::Buy:
        m_vPortfolioCalcs[ COL_ConsVlu ] += nPending * quote.Ask();
        m_vPortfolioCalcs[ COL_ImpVol ]  += nPending * greek.ImpliedVolatility();
        m_vPortfolioCalcs[ COL_Delta ]   += nPending * greek.Delta();
        m_vPortfolioCalcs[ COL_Gamma ]   += nPending * greek.Gamma();
        m_vPortfolioCalcs[ COL_Theta ]   += nPending * greek.Theta();
        m_vPortfolioCalcs[ COL_Vega ]    += nPending * greek.Vega();
        m_vPortfolioCalcs[ COL_Rho ]     += nPending * greek.Rho();
        break;
      case OrderSide::Sell:
        m_vPortfolioCalcs[ COL_ConsVlu ] -= nPending * quote.Bid();
        m_vPortfolioCalcs[ COL_ImpVol ]  -= nPending * greek.ImpliedVolatility();
        m_vPortfolioCalcs[ COL_Delta ]   -= nPending * greek.Delta();
        m_vPortfolioCalcs[ COL_Gamma ]   -= nPending * greek.Gamma();
        m_vPortfolioCalcs[ COL_Theta ]   -= nPending * greek.Theta();
        m_vPortfolioCalcs[ COL_Vega ]    -= nPending * greek.Vega();
        m_vPortfolioCalcs[ COL_Rho ]     -= nPending * greek.Rho();
        break;
    }
  } );
  
  typedef std::set<int> setIndexes_t;
  static const setIndexes_t setIndexes = { COL_Quan, COL_ConsVlu, COL_ImpVol, COL_Delta, COL_Gamma, COL_Theta, COL_Vega, COL_Rho };
  
  std::for_each( setIndexes.begin(), setIndexes.end(), [this](setIndexes_t::value_type ix){
    m_vPortfolioModelCell[ ix ].SetValue( m_vPortfolioCalcs[ ix ] );
    if ( m_vPortfolioModelCell[ ix ].Changed() ) m_gridPortfolioStats->SetCellValue( 0, ix, m_vPortfolioModelCell[ ix ].GetText() );
  } );
  
}

} // namespace tf
} // namespace ou
