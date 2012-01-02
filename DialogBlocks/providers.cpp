

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
    m_btnD1IQFeed = NULL;
    m_btnD2IQFeed = NULL;
    m_btnXIQFeed = NULL;
    m_btnIQFeed = NULL;
    m_btnD1IB = NULL;
    m_btnD2IB = NULL;
    m_btnXIB = NULL;
    m_btnIB = NULL;
    m_btnD1Simulator = NULL;
    m_btnD2Simulator = NULL;
    m_btnXSimulator = NULL;
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

    m_btnD1IQFeed = new wxRadioButton( itemFrame1, ID_RBTN_D1_IQF, _("D1"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD1IQFeed->SetValue(false);
    itemBoxSizer3->Add(m_btnD1IQFeed, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_btnD2IQFeed = new wxRadioButton( itemFrame1, ID_RBTN_D2_IQF, _("D2"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD2IQFeed->SetValue(false);
    itemBoxSizer3->Add(m_btnD2IQFeed, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnXIQFeed = new wxRadioButton( itemFrame1, ID_RBTN_X_IQF, _("X"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnXIQFeed->SetValue(false);
    m_btnXIQFeed->Enable(false);
    itemBoxSizer3->Add(m_btnXIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIQFeed = new wxButton( itemFrame1, ID_BtnIQFeed, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(m_btnIQFeed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemFrame1, wxID_LblIQFeed, _("IQF"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer9, 0, wxALIGN_LEFT|wxALL, 5);

    m_btnD1IB = new wxRadioButton( itemFrame1, ID_RBTN_D1_IB, _("D1"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD1IB->SetValue(false);
    itemBoxSizer9->Add(m_btnD1IB, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_btnD2IB = new wxRadioButton( itemFrame1, ID_RBTN_D2_IB, _("D2"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD2IB->SetValue(false);
    itemBoxSizer9->Add(m_btnD2IB, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnXIB = new wxRadioButton( itemFrame1, ID_RBTN_X_IB, _("X"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnXIB->SetValue(false);
    itemBoxSizer9->Add(m_btnXIB, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnIB = new wxButton( itemFrame1, ID_BtnInteractiveBrokers, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(m_btnIB, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText14 = new wxStaticText( itemFrame1, wxID_LblInteractiveBrokers, _("IB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer15, 0, wxALIGN_LEFT|wxALL, 5);

    m_btnD1Simulator = new wxRadioButton( itemFrame1, ID_RBTN_D1_SIM, _("D1"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD1Simulator->SetValue(false);
    itemBoxSizer15->Add(m_btnD1Simulator, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_btnD2Simulator = new wxRadioButton( itemFrame1, ID_RBTN_D2_SIM, _("D2"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnD2Simulator->SetValue(false);
    m_btnD2Simulator->Enable(false);
    itemBoxSizer15->Add(m_btnD2Simulator, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnXSimulator = new wxRadioButton( itemFrame1, ID_RBTN_X_SIM, _("X"), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE );
    m_btnXSimulator->SetValue(false);
    itemBoxSizer15->Add(m_btnXSimulator, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_btnSimulator = new wxButton( itemFrame1, ID_BtnSimulation, _("Turn On"), wxDefaultPosition, wxDefaultSize, 0 );
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
