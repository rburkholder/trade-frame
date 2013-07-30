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

#include "StdAfx.h"

#include "VuBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

VuBase::VuBase(void) {
}

VuBase::VuBase( wxWindow *parent, wxWindowID id, 
    const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator )
  : wxDataViewCtrl( parent, id, pos, size, style, validator ) {
}

VuBase::~VuBase(void) {
}

} // namespace tf
} // namespace ou
