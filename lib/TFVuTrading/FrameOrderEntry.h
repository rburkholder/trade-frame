/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
/*
 * File:    FrameOrderEntry.h
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: January 7, 2020, 20:30
 */

 #pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/frame.h>

#define SYMBOL_FRAMEORDERENTRY_STYLE wxCAPTION|wxRESIZE_BORDER|wxSTAY_ON_TOP|wxTAB_TRAVERSAL
#define SYMBOL_FRAMEORDERENTRY_TITLE _("Order Entry")
#define SYMBOL_FRAMEORDERENTRY_IDNAME ID_FRAMEORDERENTRY
#define SYMBOL_FRAMEORDERENTRY_SIZE wxDefaultSize
#define SYMBOL_FRAMEORDERENTRY_POSITION wxDefaultPosition

class wxRadioButton;
class wxTextCtrl;
class wxRadioButton;
class wxButton;

class FrameOrderEntry: public wxFrame {
  friend class boost::serialization::access;
public:

  FrameOrderEntry();
  FrameOrderEntry(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMEORDERENTRY_IDNAME,
    const wxString& caption = SYMBOL_FRAMEORDERENTRY_TITLE,
    const wxPoint& pos = SYMBOL_FRAMEORDERENTRY_POSITION,
    const wxSize& size = SYMBOL_FRAMEORDERENTRY_SIZE,
    long style = SYMBOL_FRAMEORDERENTRY_STYLE );
  virtual ~FrameOrderEntry(void);

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMEORDERENTRY_IDNAME,
    const wxString& caption = SYMBOL_FRAMEORDERENTRY_TITLE,
    const wxPoint& pos = SYMBOL_FRAMEORDERENTRY_POSITION,
    const wxSize& size = SYMBOL_FRAMEORDERENTRY_SIZE,
    long style = SYMBOL_FRAMEORDERENTRY_STYLE );


  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:
  enum {
      ID_Null=wxID_HIGHEST
    , ID_FRAMEORDERENTRY
     , ID_radioDay
    , ID_radioGTC
    , ID_radioMarket
    , ID_radioLimit
    , ID_radioStop
    , ID_txtProfitPrice
    , ID_txtLimitPrice
    , ID_txtStopPrice
    , ID_txtQuantity
    , ID_radioLong
    , ID_radioShort
    , ID_btnUpdate
    , ID_btnSend
    , ID_btnCancel
  };

    wxRadioButton* m_radioExpiryDay;
    wxRadioButton* m_radioExpiryGTC;
    wxRadioButton* m_radioOrderMarket;
    wxRadioButton* m_radioOrderLimit;
    wxRadioButton* m_radioOrderStop;
    wxTextCtrl* m_txtProfitPrice;
    wxTextCtrl* m_txtLimitPrice;
    wxTextCtrl* m_txtStopPrice;
    wxTextCtrl* m_txtQuantity;
    wxRadioButton* m_radioLong;
    wxRadioButton* m_radioShort;
    wxButton* m_btnUpdate;
    wxButton* m_btnSend;
    wxButton* m_btnCancel;

  void Init();
  void CreateControls();
  void OnClose( wxCloseEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemResources>(*this);
    wxPoint point = this->GetPosition();
    ar & point.x;
    ar & point.y;
    wxSize size = this->GetSize();
    ar & size.x;
    ar & size.y;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemResources>(*this);
    int x, y;
    ar & x;
    ar & y;
    wxPoint point( x, y );
    this->SetPosition( point );
    ar & x;
    ar & y;
    wxSize size( x, y );
    this->SetSize( size );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(FrameOrderEntry, 1)