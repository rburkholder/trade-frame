/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include <wx/dataview.h>

class VuOrders: public wxDataViewListCtrl {
public:
  VuOrders(void);
  VuOrders(wxWindow *parent, wxWindowID id, 
    const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
    long style=0, const wxValidator &validator=wxDefaultValidator);
  ~VuOrders(void);
protected:
private:
};

