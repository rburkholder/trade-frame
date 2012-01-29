

/*
 * Providers type definition
 */

IMPLEMENT_DYNAMIC_CLASS( Providers, wxWindow )


/*
 * Providers event table definition
 */

BEGIN_EVENT_TABLE( Providers, wxWindow )

////@begin Providers event table entries
////@end Providers event table entries

END_EVENT_TABLE()


/*
 * Providers constructors
 */

Providers::Providers()
{
    Init();
}

Providers::Providers(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * Providers creator
 */

bool Providers::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin Providers creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end Providers creation
    return true;
}


/*
 * Providers destructor
 */

Providers::~Providers()
{
////@begin Providers destruction
////@end Providers destruction
}


/*
 * Member initialisation
 */

void Providers::Init()
{
////@begin Providers member initialisation
    m_cbIQFeedD1 = NULL;
    m_cbIQFeedD2 = NULL;
    m_cbIQFeedX = NULL;
    m_btnIQFeed = NULL;
    m_cbIBD1 = NULL;
    m_cbIBD2 = NULL;
    m_cbIBX = NULL;
    m_btnIB = NULL;
    m_cbSimD1 = NULL;
    m_cbSimD2 = NULL;
    m_cbSimX = NULL;
    m_btnSimulator = NULL;
////@end Providers member initialisation
}


/*
 * Control creation for Providers
 */

void Providers::CreateControls()
{    
////@begin Providers content construction
    Providers* itemFrame1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbIQFeedD1 = new wxCheckBox( itemFrame1, ID_CB_IQF_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedD1->SetValue(true);
    itemBoxSizer3->Add(m_cbIQFeedD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbIQFeedD2 = new wxCheckBox( itemFrame1, ID_CB_IQF_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedD2->SetValue(false);
    itemBoxSizer3->Add(m_cbIQFeedD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbIQFeedX = new wxCheckBox( itemFrame1, ID_CB_IQF_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIQFeedX->SetValue(false);
    m_cbIQFeedX->Enable(false);
    itemBoxSizer3->Add(m_cbIQFeedX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIQFeed = new wxButton( itemFrame1, ID_BtnIQFeed, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnIQFeed->Enable(false);
    itemBoxSizer3->Add(m_btnIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemFrame1, wxID_LblIQFeed, _("IQF"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbIBD1 = new wxCheckBox( itemFrame1, ID_CB_IB_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBD1->SetValue(false);
    itemBoxSizer9->Add(m_cbIBD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbIBD2 = new wxCheckBox( itemFrame1, ID_CB_IB_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBD2->SetValue(true);
    itemBoxSizer9->Add(m_cbIBD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbIBX = new wxCheckBox( itemFrame1, ID_CB_IB_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbIBX->SetValue(true);
    itemBoxSizer9->Add(m_cbIBX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIB = new wxButton( itemFrame1, ID_BtnInteractiveBrokers, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnIB->Enable(false);
    itemBoxSizer9->Add(m_btnIB, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemFrame1, wxID_LblInteractiveBrokers, _("IB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer15, 0, wxALIGN_LEFT|wxALL, 5);

    m_cbSimD1 = new wxCheckBox( itemFrame1, ID_CB_SIM_D1, _("D1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimD1->SetValue(false);
    itemBoxSizer15->Add(m_cbSimD1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_cbSimD2 = new wxCheckBox( itemFrame1, ID_CB_SIM_D2, _("D2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimD2->SetValue(false);
    m_cbSimD2->Enable(false);
    itemBoxSizer15->Add(m_cbSimD2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_cbSimX = new wxCheckBox( itemFrame1, ID_CB_SIM_X, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbSimX->SetValue(false);
    itemBoxSizer15->Add(m_cbSimX, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnSimulator = new wxButton( itemFrame1, ID_BtnSimulation, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnSimulator->Enable(false);
    itemBoxSizer15->Add(m_btnSimulator, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText20 = new wxStaticText( itemFrame1, wxID_LblSimulation, _("Sim"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(itemStaticText20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end Providers content construction
}


/*
 * Should we show tooltips?
 */

bool Providers::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap Providers::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Providers bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Providers bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon Providers::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Providers icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Providers icon retrieval
}
