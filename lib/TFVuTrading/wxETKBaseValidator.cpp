/////////////////////////////////////////////////////////////////////////////
// Name:        wxETKIntegerValidator.cpp
// Library:     wxETK
// Purpose:     Implementation of validators.
// Author:      Stéphane Château
// Modified by:
// Created:     03/06/2011
// Licence:     LGPL
/////////////////////////////////////////////////////////////////////////////

#include <wx/log.h>
#include <wx/msgdlg.h> 

#include "wxETKBaseValidator.h"

bool wxETKIsAlphaStrict(const wxChar& _rChar)
{
    return (    (_rChar>=_T('A') && _rChar<=_T('Z'))
             || (_rChar>=_T('a') && _rChar<=_T('z'))
           );
}

bool wxETKIsAlphaNumrericStrict(const wxChar& _rChar)
{
    return (    (_rChar>=_T('A') && _rChar<=_T('Z'))
             || (_rChar>=_T('a') && _rChar<=_T('z'))
             || (_rChar>=_T('0') && _rChar<=_T('9'))
           );
}

// IRIX mipsPro refuses to compile wxStringCheck<func>() if func is inline so
// let's work around this by using this non-template function instead of
// wxStringCheck(). And while this might be fractionally less efficient because
// the function call won't be inlined like this, we don't care enough about
// this to add extra #ifs for non-IRIX case.
namespace
{

bool wxETKCheckString(bool (*func)(const wxChar&), const wxString& str)
{
    for ( wxString::const_iterator i = str.begin(); i != str.end(); ++i )
    {
        if ( !func(*i) )
            return false;
    }

    return true;
}

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                       //
//                                        wxETKBaseValidator                                             //
//                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
wxColour wxETKBaseValidator::m_clMandatory;
wxColour wxETKBaseValidator::m_clError;

wxClassInfo wxETKBaseValidator::ms_classInfo(wxT("wxETKBaseValidator"),
                                             NULL,
                                             NULL,
                                             (int) sizeof(wxETKBaseValidator),
                                             NULL);

wxClassInfo *wxETKBaseValidator::GetClassInfo() const
    { return &wxETKBaseValidator::ms_classInfo; }

wxETKBaseValidator::wxETKBaseValidator(wxWindowBase *_pValidatorWindow)
    : m_bIsOnError(false)
    , m_pTextCtrl(NULL)
{
    SetTextCtrlIfNeeded(_pValidatorWindow);
}

wxETKBaseValidator::wxETKBaseValidator(const wxETKBaseValidator &_rValidator)
{
    Copy(_rValidator);
}

wxETKBaseValidator::~wxETKBaseValidator()
{
}

bool wxETKBaseValidator::Copy(const wxETKBaseValidator& _rValidator)
{
    m_bIsOnError   = _rValidator.m_bIsOnError;
    m_clOldBkColor = _rValidator.m_clOldBkColor;
    m_pTextCtrl    = _rValidator.m_pTextCtrl;

    return true;
}

wxColour wxETKBaseValidator::GetColorForMandatory()
{
    wxASSERT(IsColorForMandatory());
    return m_clMandatory;
}

void wxETKBaseValidator::SetColorForMandatory(const wxColour &_rclMandatory)
{
    m_clMandatory = _rclMandatory;
}

bool wxETKBaseValidator::IsColorForMandatory()
{
    return m_clMandatory.IsOk();
}

wxColour wxETKBaseValidator::GetColorForError()
{
    wxASSERT(IsColorForError());
    return m_clError;
}

void wxETKBaseValidator::SetColorForError(const wxColour &_rclError)
{
    m_clError = _rclError;
}

bool wxETKBaseValidator::IsColorForError()
{
    return m_clError.IsOk();
}

void wxETKBaseValidator::SetTextCtrlIfNeeded(wxWindowBase *_pValidatorWindow)
{
    if (_pValidatorWindow != NULL)
    {   // Only if pointer is valid
        if (m_pTextCtrl == NULL)
        {   // Only if not already set
            if (_pValidatorWindow!=NULL && _pValidatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
            {
                m_pTextCtrl = dynamic_cast<wxTextCtrl *>(_pValidatorWindow);
                wxASSERT(m_pTextCtrl != NULL); // Must not be NULL
            }
        }
        else
        {   // Must be the same pointer else there is a problem
            wxASSERT(m_pTextCtrl == _pValidatorWindow); // Must be the same
        }
    }
}

void wxETKBaseValidator::ClearError()
{
    if (m_bIsOnError)
    {
        if (GetTextCtrl() != NULL && IsColorForError())
        {   // Put background color back
            GetTextCtrl()->SetBackgroundColour(m_clOldBkColor);
            GetTextCtrl()->Refresh();
        }
        m_bIsOnError = false;
    }
}

void wxETKBaseValidator::SetError()
{
    wxASSERT(GetTextCtrl() != NULL);

    if (GetTextCtrl() != NULL)
    {
        if (IsColorForError())
        {
            if (!m_bIsOnError)
            {
                m_clOldBkColor = GetTextCtrl()->GetBackgroundColour();
                m_bIsOnError   = true;
            }
            GetTextCtrl()->SetBackgroundColour(GetColorForError());
            GetTextCtrl()->Refresh();
        }
        GetTextCtrl()->SetFocus();
    }
}

wxTextCtrl * wxETKBaseValidator::GetTextCtrl() const
{
    return m_pTextCtrl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                       //
//                                        wxETKTextValidator                                             //
//                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS2(wxETKTextValidator,wxTextValidator,wxETKBaseValidator)

BEGIN_EVENT_TABLE(wxETKTextValidator,wxTextValidator)
    EVT_CHAR(wxETKTextValidator::OnChar)
END_EVENT_TABLE()

wxETKTextValidator::wxETKTextValidator(long _lStyle, wxString *_pStringVal,wxWindowBase *_pValidatorWindow)
    : wxTextValidator(_lStyle & ~(wxFILTER_MAX_LENGTH | wxFILTER_MIN_LENGTH),_pStringVal)
    , wxETKBaseValidator(_pValidatorWindow)
    , m_stMinLength(0)
    , m_stMaxLength(0)
{
    if (HasFlag(wxFILTER_EMPTY) && IsColorForMandatory())
    {
        wxASSERT(GetTextCtrl() != NULL); // Must never be null, MUST BE wxTextCtrl window

        if (GetTextCtrl() != NULL)
        {   // Set the specific color for mandatory
            GetTextCtrl()->SetBackgroundColour(GetColorForMandatory());
        }
    }
    if (HasFlag(wxFILTER_UPPERCASE))
    {
        wxASSERT(GetTextCtrl() != NULL); // Must never be null, MUST BE wxTextCtrl window

        if (GetTextCtrl() != NULL)
        {
            GetTextCtrl()->Connect(GetTextCtrl()->GetId(), wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction) &wxETKTextValidator::OnCommandUpdateChangeToUppercase);
        }
        else
        {   // Remove filter
            wxLogWarning(wxT("Only wxTextCtrl control could have wxFILTER_UPPERCASE style, removing it!"));
            SetStyle(GetStyle() & ~wxFILTER_UPPERCASE); // Remove uppercase style on window that is not a wxTextCtrl
        }
    }
}

wxETKTextValidator::wxETKTextValidator(const wxETKTextValidator& _rValidator)
    : wxTextValidator()
    , wxETKBaseValidator(_rValidator.GetTextCtrl())
{
    wxTextValidator::Copy(_rValidator);
    wxETKBaseValidator::Copy(_rValidator);
    Copy(_rValidator);
}

wxETKTextValidator::~wxETKTextValidator()
{
}

void wxETKTextValidator::SetMinLength(size_t _stMinLength)
{
    wxASSERT(GetTextCtrl() != NULL);
    if (GetTextCtrl() != NULL)
    {
        m_stMinLength = _stMinLength;
        SetStyle(GetStyle() | wxFILTER_MIN_LENGTH); // Add style
    }
}

void wxETKTextValidator::SetMaxLength(int _stMaxLength,bool _bSetMaxLenOnTextCtrl)
{
    wxASSERT(GetTextCtrl() != NULL);
    if (GetTextCtrl() != NULL)
    {
        if (_bSetMaxLenOnTextCtrl)
        {   // Call directly on control
            GetTextCtrl()->SetMaxLength(_stMaxLength);
        }
        m_stMaxLength = _stMaxLength;
        SetStyle(GetStyle() | wxFILTER_MAX_LENGTH); // Add style
    }
}

wxString wxETKTextValidator::IsValid(const wxString& _strVal) const
{
    if ( HasFlag(wxFILTER_ALPHANUMERIC_STRICT) && !wxETKCheckString(wxETKIsAlphaNumrericStrict, _strVal) )
        return _("'%s' should only contain alphabetic characters without accent or numeric characters.");
    if ( HasFlag(wxFILTER_ALPHA_STRICT)        && !wxETKCheckString(wxETKIsAlphaStrict, _strVal) )
        return _("'%s' should only contain alphabetic characters without accent.");

#if wxCHECK_VERSION(2,9,0) // version >= 2.9.0
    return wxTextValidator::IsValid(_strVal);
#else
    return wxEmptyString;
#endif
}

void wxETKTextValidator::OnChar(wxKeyEvent& event)
{
    ClearError();
    bool bError = false;

    // we don't filter special keys and delete
    int iKeyCode = event.GetKeyCode();
    if (iKeyCode < WXK_SPACE || iKeyCode == WXK_DELETE || iKeyCode >= WXK_START)
    {
        event.Skip();
        return;
    }

    if (HasFlag(wxFILTER_UPPERCASE))
    {   // Transform downcase to uppercase. If only some char are allowed like A,B,C, typing
        // 'b' will be refused by base class because here the 'b' is in minus. Transform it to
        // uppercase before this event if filtered by base class

        if (iKeyCode>=_T('a') && iKeyCode<=_T('z'))
        {
            event.m_uniChar = toupper(event.m_uniChar);
            iKeyCode = event.m_keyCode = toupper(iKeyCode);
        }
    }

    wxString str((wxChar) iKeyCode, 1);
    if (!IsValid(str).IsEmpty())
    {
        bError = true;
    }

    if (!bError)
    {   // Char is accepted, throw this event to other class who want this event
        event.Skip();
    }
    else
    {   // Char is refused, ignore it: eat message
        if (!wxTextValidator::IsSilent())
        {
            wxBell();
        }
    }
}

bool wxETKTextValidator::Validate(wxWindow *_pParent)
{
    bool bRet = wxTextValidator::Validate(_pParent);
    SetTextCtrlIfNeeded(GetWindow());
    if (bRet && GetTextCtrl()!=NULL)
    {   // Bloc that will display error message box, already done if error when validate base class
        wxString strError;
#if !wxCHECK_VERSION(2,9,0) // version < 2.9.0
        // wxFILTER_EMPTY is not implemented in wxWidgets 2.8.x or lesser
        if (HasFlag(wxFILTER_EMPTY) && GetTextCtrl()->GetValue().IsEmpty())
        {
            strError = _("Required information entry is empty.");
            bRet = false;
        }
        // IsValid is not called in wxWidgets 2.8.x or lesser (doesn't exists)
        // else error already displayed to the user
        if (bRet && !(strError=IsValid(GetTextCtrl()->GetValue())).IsEmpty())
        {
            strError = wxString::Format(strError,GetTextCtrl()->GetValue().wx_str());
            bRet = false;
        }
#endif
        if (bRet && HasFlag(wxFILTER_MIN_LENGTH) && GetTextCtrl()->GetValue().Len()<m_stMinLength)
        {
            strError = wxString::Format(wxPLURAL("Information entry must be at least %d char length.",
                                                 "Information entry must be at least %d chars length.",
                                                 m_stMinLength
                                                ),
                                        m_stMinLength
                                       );
            bRet = false;
        }
        if (bRet && HasFlag(wxFILTER_MAX_LENGTH) && GetTextCtrl()->GetValue().Len()>m_stMaxLength)
        {
            strError = wxString::Format(wxPLURAL("Information entry must max %d char length.",
                                                 "Information entry must max %d chars length.",
                                                 m_stMaxLength
                                                ),
                                        m_stMaxLength
                                       );
            bRet = false;
        }
        if (!bRet)
        {
            wxMessageBox(strError,wxGetTranslation(_T("Validation conflict")),wxOK | wxICON_EXCLAMATION, _pParent);
            //                                                   ^
            // Already translated by wxWidgets ------------------|
        }
    }
    if (!bRet)
    {
        SetError();
    }
    else
    {   // Put background color back (if needed)
        ClearError();
    }

    return bRet;
}

bool wxETKTextValidator::TransferToWindow()
{
    bool bRet = wxTextValidator::TransferToWindow();
    if (bRet)
    {
        ClearError();
    }
    return bRet;
}

bool wxETKTextValidator::Copy(const wxETKTextValidator& _rValidator)
{
    if (!wxTextValidator::Copy(_rValidator) || !wxETKBaseValidator::Copy(_rValidator))
    {
        return false;
    }
    m_stMinLength = _rValidator.m_stMinLength;
    m_stMaxLength = _rValidator.m_stMaxLength;

    return true;
}

wxObject *wxETKTextValidator::Clone() const
{
    return new wxETKTextValidator(*this);
}

void wxETKTextValidator::OnCommandUpdateChangeToUppercase(wxCommandEvent &_rCommandEvent)
{
    wxTextCtrl *pTextCtrl = dynamic_cast<wxTextCtrl *>(_rCommandEvent.GetEventObject());
    if (pTextCtrl != NULL)
    {
        if (!pTextCtrl->IsModified())
            return;
        long insertionPoint = pTextCtrl->GetInsertionPoint();
        pTextCtrl->ChangeValue(pTextCtrl->GetValue().Upper());
        pTextCtrl->SetInsertionPoint(insertionPoint);
    }
    _rCommandEvent.Skip();
}
