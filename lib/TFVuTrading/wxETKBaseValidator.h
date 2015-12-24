// from https://forums.wxwidgets.org/viewtopic.php?t=21056

 /////////////////////////////////////////////////////////////////////////////
// Name:        wxETKIntegerValidator.h
// Library:     wxETK
// Purpose:     Header of validators.
// Author:      Stéphane Château
// Modified by:
// Created:     03/06/2011
// Licence:     LGPL
/////////////////////////////////////////////////////////////////////////////
#ifndef WXETK_VALIDATORS_H
#define WXETK_VALIDATORS_H

//#include "wxETKTypes.h"
#include <wx/colour.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                       //
//                                        wxETKBaseValidator                                             //
//                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Base class for all wxETK validator classes.
 *
 * This class records colors for all validators: error color and mandatory color.
 *
 * All control who have wxFILTER_EMPTY as filter are set to specific background color (if set only).
 * When validate, if error occurs, the background of the control change to specific error color (if set too).
 * When a control is in error, modifying it (by past or TransferToWindow) changing the background back to
 * default color even the control is not yet valid.
 *
 * @author Stéphane Château
 * @version Name : wxETK<br>
 *          Revision : <b>1.0</b>
 */
class wxETKBaseValidator
{
private:
    /**
     * Color for mandatory controls.
     *
     * Is used to display color controls that the value is mandatory (wxFILTER_EMPTY enabled).
     */
    static wxColour                     m_clMandatory;

    /**
     * Color for error controls.
     *
     * Modify controls who are validate failed to this color when validate failed.
     * Changing content of these controls make original color back, even the control is not yet valid.
     */
    static wxColour                     m_clError;

    /**
     * Indicate the control error color before changing the color.
     */
    wxColour                            m_clOldBkColor;

    /**
     * Indicate if the control is on error or not.
     *
     * When the user change the control, this flag is reset.
     */
    bool                                m_bIsOnError;

    /**
     * Text control pointer.
     *
     * Used only for wxTextCtrl type for uppercase filter.
     */
    wxTextCtrl *                        m_pTextCtrl;

public:
    /// @name Constructor / Destructor.
    //@{
    /**
     * Standard constructor.
     */
    wxETKBaseValidator(wxWindowBase *_pValidatorWindow);

    /**
     * Copy constructor.
     *
     * @param _rValidator Instance to copy into this.
     */
    wxETKBaseValidator(const wxETKBaseValidator &_rValidator);

    /**
     * Destructor.
     */
    virtual ~wxETKBaseValidator();
    //@}

    /**
     * Copy into this.
     *
     * @param _rValidator Element to copy into this.
     * @return Always true.
     */
    bool                                Copy(const wxETKBaseValidator& _rValidator);

    /**
     * Get color for mandatory controls.
     *
     * @return The color of the background mandatory control.
     */
    static wxColour                     GetColorForMandatory();

    /**
     * Set color for mandatory controls.
     *
     * @param _rclMandatory Color set for mandator controls, if not set the color of the control don't change.
     */
    static void                         SetColorForMandatory(const wxColour &_rclMandatory);

    /**
     * Indicate if the color for mandatory is set or not.
     *
     * @return true if the color has been set, false else.
     */
    static bool                         IsColorForMandatory();

    /**
     * Get color for errors controls.
     *
     * @return The color of the background error control.
     */
    static wxColour                     GetColorForError();

    /**
     * Set color for errors controls.
     *
     * @param _rclError Color set for error, if not set the color of the control don't change if error
     *                  occurs when validate.
     */
    static void                         SetColorForError(const wxColour &_rclError);

    /**
     * Indicate if the color for error is set or not.
     *
     * @return true if the color has been set, false else.
     */
    static bool                         IsColorForError();

    /**
     * Set the text control only if not already set.
     *
     * @param _pValidatorWindow Validator window to record.
     */
    void                                SetTextCtrlIfNeeded(wxWindowBase *_pValidatorWindow);

protected:
    /**
     * Clear the error if exists.
     *
     * Put the control back to normal color.
     */
    void                                ClearError();

    /**
     * Set the error if not already set.
     *
     * Put the control to error color only if set, record the original one before changing it.
     */
    void                                SetError();

    /**
     * Is mandatory to be able to know the validator.
     *
     * Used to set and clear error, etc.
     *
     * @return The Validator.
     */
    wxTextCtrl *                        GetTextCtrl() const;

private:
    DECLARE_ABSTRACT_CLASS(wxETKBaseValidator);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                       //
//                                        wxETKTextValidator                                             //
//                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if wxCHECK_VERSION(2,9,0) // version 2.9.0
    // Not defined in wxWidgets : set ctrl to uppercase
    #define wxFILTER_UPPERCASE              ((wxTextValidatorStyle) 0x8000)
    // Not defined in wxWidgets : set min lenght
    #define wxFILTER_MIN_LENGTH             ((wxTextValidatorStyle) 0x4000)
    // Not defined in wxWidgets : set max lenght
    #define wxFILTER_MAX_LENGTH             ((wxTextValidatorStyle) 0x2000)
    // Not defined in wxWidgets : set only A-Z and a-z char allowed
    #define wxFILTER_ALPHA_STRICT           ((wxTextValidatorStyle) 0x1000)
    // Not defined in wxWidgets : set only A-Z and a-z and 0-9 char allowed
    #define wxFILTER_ALPHANUMERIC_STRICT    ((wxTextValidatorStyle) 0x0800)
#else
    // Not defined in 2.8.x : different values as 2.9.0
    #define wxFILTER_EMPTY                  0x8000
    // Not defined in wxWidgets : set ctrl to uppercase
    #define wxFILTER_UPPERCASE              0x4000
    // Not defined in wxWidgets : set min lenght
    #define wxFILTER_MIN_LENGTH             0x2000
    // Not defined in wxWidgets : set max lenght
    #define wxFILTER_MAX_LENGTH             0x1000
    // Not defined in wxWidgets : set only A-Z and a-z char allowed
    #define wxFILTER_ALPHA_STRICT           0x0800
    // Not defined in wxWidgets : set only A-Z and a-z and 0-9 char allowed
    #define wxFILTER_ALPHANUMERIC_STRICT    0x0400
#endif

/**
 * Text validator.
 *
 * Based on wxETKBaseValidator this class add new kind of filters like:
 * <ul>
 *  <li>wxFILTER_EMPTY: if the control is empty, the validate failed. It is already implements into
 *                      wxWidgets 2.9.x but this class implements it for previous versions.</li>
 *  <li>wxFILTER_UPPERCASE: all typing into the control is put automatically in uppercase. This not use
 *                          a specific Windows style but catch all event when modifying the control content,
 *                          so it is multi-plateform.</li>
 *  <li>wxFILTER_MIN_LENGTH: let specify a min length of the control content, else the validate failed.</li>
 *  <li>wxFILTER_MAX_LENGTH: let specify a min length of the control content, else the validate failed.
 *                           Call SetMaxLength on the wxTextCtrl (cannot write more than this length), or could
 *                           only verify this length at validation (else do both controls).</li>
 *  <li>wxFILTER_ALPHA_STRICT: accept only a-z + A-Z chars.</li>
 *  <li>wxFILTER_ALPHANUMERIC_STRICT: accept only a-z + A-Z + 0-9 chars.</li>
 * </ul>
 *
 * @author Stéphane Château
 * @version Name : wxETK<br>
 *          Revision : <b>1.0</b>
 */
class wxETKTextValidator : public wxTextValidator
                                       , public wxETKBaseValidator
{
private:
    /**
     * Min text length.
     */
    size_t                              m_stMinLength;

    /**
     * Min text length.
     */
    size_t                              m_stMaxLength;

public:
    /// @name Constructor / Destructor.
    //@{
    /**
     * Constructor.
     *
     * Construct the text validator.
     *
     * @param _lStyle Text validator style.
     * @param _pStringVal Pointer on string.
     * @param _pValidatorWindow Pointer on window. It is used ONLY on wxTextCtrl
     *                          for wxFILTER_UPPERCASE only.
     */
    wxETKTextValidator(long _lStyle = wxFILTER_NONE, wxString *_pStringVal = NULL,wxWindowBase *_pValidatorWindow = NULL);

    /**
     * Copy constructor.
     *
     * @param _rValidator Element to copy into this.
     */
    wxETKTextValidator(const wxETKTextValidator& _rValidator);

    /**
     * Destructor.
     */
    virtual ~wxETKTextValidator();
    //@}

    /**
     * Set the min length of this element.
     *
     * Only valid when _pValidatorWindow is pass into constructor. Only wxTextCtrl base class are allowed.
     *
     * @param _stMinLength Min length.
     */
    void                                SetMinLength(size_t _stMinLength);

    /**
     * Set the max length of this element.
     *
     * Only valid when _pValidatorWindow is pass into constructor.
     * Only wxTextCtrl base class are allowed.
     * The check is done when validate and the SetMaxLength function is called on
     * wxTextCtrl to limit string length by wxWidgets.
     *
     * @param _stMaxLength Max length.
     * @param _bSetMaxLenOnTextCtrl If true, call SetMaxLength on the wxTextCtrl, else only verify
     *                              on validation, else do both controls.
     */
    void                                SetMaxLength(int _stMaxLength,bool _bSetMaxLenOnTextCtrl=true);
protected:
#if !wxCHECK_VERSION(2,9,0) // version < 2.9.0
    /**
     * Test if style exist in style bit field of this component.
     *
     * @param iStyleToTest Style field to test.
     * @return true if this field is set, false else.
     */
    bool                                HasFlag(int iStyleToTest) const   { return (GetStyle() & iStyleToTest) != 0; }
#endif

    /**
     * Check is the string is valid, depending of filter value.
     *
     * Called only in wxWidget 2.9.x.
     *
     * @return The error message if the contents of '_strVal' are invalid.
     *         NB: this format string should always contain exactly one '%s'.
     */
    virtual wxString                    IsValid(const wxString& _strVal) const;

    /** Called when the value in the window must be validated.
     *
     * This function can pop up an error message.
     *
     * @param _pParent Control to validate.
     * @return true is the validate is ok, false else.
     */
    virtual bool                        Validate(wxWindow *_pParent);

    /**
     * Called to transfer data to the window.
     *
     * Clear error if validator is in error.
     */
    virtual bool                        TransferToWindow();

    /**
     * Used to change control color when it is "on error".
     *
     * When char is pressed, if m_bIsOnError is true, set background back
     * to m_clOldBkColor and reset m_bIsOnError.
     *
     * @param _rEvent Char event.
     */
    void                                OnChar(wxKeyEvent& _rEvent);

    /**
     * Called when the user press key on number text ctrl.
     *
     * If key is 'a' to 'z', transform to 'A' to 'Z'.
     *
     * @param _rCommandEvent Command event.
     */
    void                                OnCommandUpdateChangeToUppercase(wxCommandEvent &_rCommandEvent);

    /**
     * Make a clone of this validator (or return NULL).
     *
     * Currently necessary if you're passing a reference to a validator.
     * Another possibility is to always pass a pointer to a new validator
     * (so the calling code can use a copy constructor of the relevant class).
     *
     * @return a copy of this.
     */
    virtual wxObject *                  Clone() const;

    /**
     * Copy into this.
     *
     * @param _rValidator Element to copy into this.
     * @return Always true.
     */
    bool                                Copy(const wxETKTextValidator& _rValidator);
private:
#if wxCHECK_VERSION(2,9,0) // version 2.9.0
    // Defines only in version < 2.9.0
    wxDECLARE_NO_ASSIGN_CLASS(wxETKTextValidator);
#endif
    DECLARE_DYNAMIC_CLASS(wxETKTextValidator)
    DECLARE_EVENT_TABLE()
};

#endif // WXETK_VALIDATORS_H

 