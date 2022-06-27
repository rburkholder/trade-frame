/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    PanelStatistics.hpp
 * Author:  raymond@burkholder.net
 * Project: DepthOfMarket
 * Created: April 11, 2022  13:01
 */

#pragma once

#include <wx/panel.h>

class wxTextCtrl;
class wxStaticText;

#define SYMBOL_PANELSTATISTICS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELSTATISTICS_TITLE _("Message Statistics")
#define SYMBOL_PANELSTATISTICS_IDNAME ID_PanelStatistics
#define SYMBOL_PANELSTATISTICS_SIZE wxSize(400, 300)
#define SYMBOL_PANELSTATISTICS_POSITION wxDefaultPosition

class PanelStatistics: public wxPanel {
public:

  struct values_t {
    size_t nTicks;

    size_t nL1MsgBid;
    size_t nL1MsgAsk;
    size_t nL1MsgTtl;

    size_t nL2MsgBid;
    size_t nL2MsgAsk;
    size_t nL2MsgTtl;

    size_t nL2AddBid;
    size_t nL2AddAsk;
    size_t nL2AddTtl;

    size_t nL2UpdBid;
    size_t nL2UpdAsk;
    size_t nL2UpdTtl;

    size_t nL2DelBid;
    size_t nL2DelAsk;
    size_t nL2DelTtl;

    size_t nLvl1AskTick;
    size_t nLvl1AskAdd;
    size_t nLvl1AskDel;

    size_t nLvl1UnkTick;
    size_t nLvl1UnkAdd;
    size_t nLvl1UnkDel;

    size_t nLvl1BidTick;
    size_t nLvl1BidAdd;
    size_t nLvl1BidDel;

    double dblB0;
    double dblB1;
    double dblR;

    values_t() { Zero(); }
    void Zero() {
      nTicks = 0;
      nL1MsgBid = nL1MsgAsk = nL1MsgTtl = 0;
      nL2MsgBid = nL2MsgAsk = nL2MsgTtl = 0;
      nL2AddBid = nL2AddAsk = nL2AddTtl = 0;
      nL2UpdBid = nL2UpdAsk = nL2UpdTtl = 0;
      nL2DelBid = nL2DelAsk = nL2DelTtl = 0;

      nLvl1AskTick = nLvl1AskAdd = nLvl1AskDel = 0;
      nLvl1UnkTick = nLvl1UnkAdd = nLvl1UnkDel = 0;
      nLvl1BidTick = nLvl1BidAdd = nLvl1BidDel = 0;

      dblB0 = dblB1 = dblR = 0.0;
    }
  };

  PanelStatistics();
  PanelStatistics(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELSTATISTICS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELSTATISTICS_POSITION,
   const wxSize& size = SYMBOL_PANELSTATISTICS_SIZE,
   long style = SYMBOL_PANELSTATISTICS_STYLE
   );
  virtual ~PanelStatistics();

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELSTATISTICS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELSTATISTICS_POSITION,
   const wxSize& size = SYMBOL_PANELSTATISTICS_SIZE,
   long style = SYMBOL_PANELSTATISTICS_STYLE
   );

  void Update( const values_t& );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_PanelStatistics
  , id_txtTicks
  , id_txtL1MsgBid, id_txtL1MsgAsk, id_txtL1MsgTtl
  , id_txtL2MsgBid, id_txtL2MsgAsk, id_txtL2MsgTtl
  , id_txtL2AddBid, id_txtL2AddAsk, id_txtL2AddTtl
  , id_txtL2UpdBid, id_txtL2UpdAsk, id_txtL2UpdTtl
  , id_txtL2DelBid, id_txtL2DelAsk, id_txtL2DelTtl

  , id_txtLvl1AskTick, id_txtLvl1AskAdd, id_txtLvl1AskDel
  , id_txtLvl1UknTick, id_txtLvl1UknAdd, id_txtLvl1UknDel
  , id_txtLvl1BidTick, id_txtLvl1BidAdd, id_txtLvl1BidDel

  , id_txtB0, id_txtB1, id_txtR
  };

    wxTextCtrl* m_txtTicks;
    wxTextCtrl* m_txtL1MsgBid;
    wxTextCtrl* m_txtL1MsgAsk;
    wxTextCtrl* m_txtL1MsgTtl;

    wxTextCtrl* m_txtL2MsgBid;
    wxTextCtrl* m_txtL2MsgAsk;
    wxTextCtrl* m_txtL2MsgTtl;

    wxTextCtrl* m_txtL2AddBid;
    wxTextCtrl* m_txtL2AddAsk;
    wxTextCtrl* m_txtL2AddTtl;

    wxTextCtrl* m_txtL2UpdBid;
    wxTextCtrl* m_txtL2UpdAsk;
    wxTextCtrl* m_txtL2UpdTtl;

    wxTextCtrl* m_txtL2DelBid;
    wxTextCtrl* m_txtL2DelAsk;
    wxTextCtrl* m_txtL2DelTtl;

    wxTextCtrl* m_txtLvl1AskTick;
    wxTextCtrl* m_txtLvl1AskAdd;
    wxTextCtrl* m_txtLvl1AskDel;

    wxTextCtrl* m_txtLvl1UknTick;
    wxTextCtrl* m_txtLvl1UknAdd;
    wxTextCtrl* m_txtLvl1UknDel;

    wxTextCtrl* m_txtLvl1BidTick;
    wxTextCtrl* m_txtLvl1BidAdd;
    wxTextCtrl* m_txtLvl1BidDel;

    wxTextCtrl* m_txtB0;
    wxTextCtrl* m_txtB1;
    wxTextCtrl* m_txtR;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

};
