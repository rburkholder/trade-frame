/////////////////////////////////////////////////////////////////////////////
// Name:        PanelFinancialChart.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     11/11/2011 21:43:29
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "PanelFinancialChart.h"

////@begin XPM images
////@end XPM images


/*
 * PanelFinancialChart type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelFinancialChart, wxPanel )


/*
 * PanelFinancialChart event table definition
 */

BEGIN_EVENT_TABLE( PanelFinancialChart, wxPanel )

////@begin PanelFinancialChart event table entries
////@end PanelFinancialChart event table entries

END_EVENT_TABLE()


/*
 * PanelFinancialChart constructors
 */

PanelFinancialChart::PanelFinancialChart()
{
    Init();
}

PanelFinancialChart::PanelFinancialChart( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelFinancialChart creator
 */

bool PanelFinancialChart::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelFinancialChart creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelFinancialChart creation
    return true;
}


/*
 * PanelFinancialChart destructor
 */

PanelFinancialChart::~PanelFinancialChart()
{
////@begin PanelFinancialChart destruction
////@end PanelFinancialChart destruction
}


/*
 * Member initialisation
 */

void PanelFinancialChart::Init()
{
////@begin PanelFinancialChart member initialisation
    m_winChart = NULL;
////@end PanelFinancialChart member initialisation
}


/*
 * Control creation for PanelFinancialChart
 */

void PanelFinancialChart::CreateControls()
{    
////@begin PanelFinancialChart content construction
    PanelFinancialChart* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_winChart = new wxWindow( itemPanel1, ID_CHART, wxDefaultPosition, wxSize(160, 90), wxNO_BORDER );
    itemBoxSizer2->Add(m_winChart, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Connect events and objects
    m_winChart->Connect(ID_CHART, wxEVT_SIZE, wxSizeEventHandler(PanelFinancialChart::OnSize), NULL, this);
    m_winChart->Connect(ID_CHART, wxEVT_PAINT, wxPaintEventHandler(PanelFinancialChart::OnPaint), NULL, this);
////@end PanelFinancialChart content construction
}


/*
 * Should we show tooltips?
 */

bool PanelFinancialChart::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelFinancialChart::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelFinancialChart bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelFinancialChart bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelFinancialChart::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelFinancialChart icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelFinancialChart icon retrieval
}






/*
 * wxEVT_PAINT event handler for ID_CHART
 */

void PanelFinancialChart::OnPaint( wxPaintEvent& event )
{
////@begin wxEVT_PAINT event handler for ID_CHART in PanelFinancialChart.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(wxDynamicCast(event.GetEventObject(), wxWindow));
////@end wxEVT_PAINT event handler for ID_CHART in PanelFinancialChart. 
}


/*
 * wxEVT_SIZE event handler for ID_CHART
 */

void PanelFinancialChart::OnSize( wxSizeEvent& event )
{
////@begin wxEVT_SIZE event handler for ID_CHART in PanelFinancialChart.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_SIZE event handler for ID_CHART in PanelFinancialChart. 
}

